#ifndef FLUIDSOLVER_IISPHFLUIDSOLVER_HPP
#define FLUIDSOLVER_IISPHFLUIDSOLVER_HPP

#include <core/fluidSolver/IFluidSolver.hpp>
#include <core/fluidSolver/kernel/CubicSplineKernel.hpp>
#include <core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp>
#include <core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp>
#include <core/parallelization/StdParallelForEach.hpp>

namespace FluidSolver
{

    struct IISPHSettings
    {
        pFloat MaxDensityErrorAllowed = 0.001f;

        size_t MinNumberOfIterations = 2;
        size_t MaxNumberOfIterations = 100;

        pFloat Omega = 0.5f;
        pFloat Gamma = 0.7f;

        pFloat Viscosity = 5.0f;
    };

    struct IISPHParticleData
    {
        vec2 predicted_velocity;
        pFloat source_term;
        pFloat diagonal_element;
    };


    template <typename Kernel = CubicSplineKernel,
              typename NeighborhoodSearch = QuadraticNeighborhoodSearchDynamicAllocated,
              typename parallel = StdParallelForEach>
    class IISPHFluidSolver : public IFluidSolverBase {
      public:
        Kernel kernel;

        NeighborhoodSearch neighborhood_search;

        void initialize() override;

        void execute_simulation_step(pFloat timestep) override;

        NeighborhoodInterface create_neighborhood_interface() override;

      private:
        void CalculateDensity(pIndex_t particleIndex);

        void CalculateNonPressureAccelerationAndPredictedVelocity(pIndex_t particleIndex);

        glm::vec2 ComputeViscosityAcceleration(pIndex_t particleIndex);

        void ComputeSourceTerm(pIndex_t particleIndex);

        void ComputeDiagonalElement(pIndex_t particleIndex);

        void InitializePressure(pIndex_t particleIndex);

        void IntegrateParticle(pIndex_t particleIndex);

        void ComputePressure();

        pFloat current_timestep = 0.0f;

      public:
        IISPHSettings settings;

        static void adapt_collection(ParticleCollection& collection);
    };

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    void IISPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::initialize()
    {
        FLUID_ASSERT(parameters.particle_size > 0.0f);
        FLUID_ASSERT(collection != nullptr);

        if (!collection->is_type_present<IISPHParticleData>())
            adapt_collection(*collection);
        neighborhood_search.collection = collection;
        neighborhood_search.search_radius = parameters.particle_size * 2.0f;
        neighborhood_search.initialize();
        kernel.kernel_support = parameters.particle_size * 2.0f;

        FLUID_ASSERT(collection->is_type_present<MovementData>());
        FLUID_ASSERT(collection->is_type_present<ParticleData>());
        FLUID_ASSERT(collection->is_type_present<ParticleInfo>());
        FLUID_ASSERT(collection->is_type_present<ExternalForces>());
        FLUID_ASSERT(collection->is_type_present<IISPHParticleData>());

        FLUID_ASSERT(settings.MaxNumberOfIterations >= settings.MinNumberOfIterations);
        FLUID_ASSERT(settings.MaxDensityErrorAllowed > 0.0f);
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    void IISPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::execute_simulation_step(pFloat timestep)
    {
        FLUID_ASSERT(collection != nullptr)
        FLUID_ASSERT(collection->is_type_present<MovementData>());
        FLUID_ASSERT(collection->is_type_present<ParticleData>());
        FLUID_ASSERT(collection->is_type_present<ParticleInfo>());
        FLUID_ASSERT(collection->is_type_present<ExternalForces>());
        FLUID_ASSERT(collection->is_type_present<IISPHParticleData>());

        FLUID_ASSERT(settings.MaxNumberOfIterations >= settings.MinNumberOfIterations);
        FLUID_ASSERT(settings.MaxDensityErrorAllowed > 0.0f);

        FLUID_ASSERT(timestep > 0.0f);
        current_timestep = timestep;


        // find neighbors for all particles
        FLUID_ASSERT(neighborhood_search.collection == collection);
        neighborhood_search.find_neighbors();

        // calculating density and non pressure accelerations
        parallel::loop_for(0, collection->size(), [&](pIndex_t i) {
            auto type = collection->get<ParticleInfo>(i).type;
            if (type == ParticleTypeDead)
                return; // we do not want to process dead particles

            CalculateDensity(i);
            CalculateNonPressureAccelerationAndPredictedVelocity(i);
        });

        // compute source term, diagonal element and initialize pressure
        parallel::loop_for(0, collection->size(), [&](pIndex_t i) {
            auto type = collection->get<ParticleInfo>(i).type;
            if (type == ParticleTypeDead)
                return; // we do not want to process dead particles
            if (type == ParticleTypeBoundary)
                return; // we do not want to process boundary particles

            ComputeSourceTerm(i);
            ComputeDiagonalElement(i);
            InitializePressure(i);
        });

        // compute pressure
        ComputePressure();

        // update velocity and position of all particles
        parallel::loop_for(0, collection->size(), [&](pIndex_t i) { IntegrateParticle(i); });
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    inline NeighborhoodInterface IISPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::create_neighborhood_interface()
    {
        return neighborhood_search.create_interface();
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    void IISPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::CalculateDensity(pIndex_t particleIndex)
    {
        auto particleType = collection->get<ParticleInfo>(particleIndex).type;
        if (particleType == ParticleTypeBoundary)
        {
            return; // don't calculate density for the boundary particles
        }
        const glm::vec2& position = collection->get<MovementData>(particleIndex).position;

        float density = 0.0f;
        auto neighbors = neighborhood_search.get_neighbors(particleIndex);
        for (uint32_t neighbor : neighbors)
        {
            auto type = collection->get<ParticleInfo>(neighbor).type;
            if (type == ParticleTypeDead)
            {
                continue; // don*t calculate unnecessary values for dead particles.
            }
            const glm::vec2& neighborPosition = collection->get<MovementData>(neighbor).position;
            float neighborMass = collection->get<ParticleData>(neighbor).mass;
            density += neighborMass * kernel.GetKernelValue(neighborPosition, position);
        }

        collection->get<ParticleData>(particleIndex).density = density;
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    void IISPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::CalculateNonPressureAccelerationAndPredictedVelocity(
        pIndex_t particleIndex)
    {
        auto type = collection->get<ParticleInfo>(particleIndex).type;
        glm::vec2& nonPressureAcc = collection->get<ExternalForces>(particleIndex).non_pressure_acceleration;
        const glm::vec2& velocity = collection->get<MovementData>(particleIndex).velocity;

        // adding gravity to non pressure acceleration
        if (type != ParticleTypeBoundary)
        {
            nonPressureAcc += glm::vec2(0.0f, -parameters.gravity);
            nonPressureAcc += ComputeViscosityAcceleration(particleIndex);
        }

        // calculate predicted velocity
        glm::vec2 predictedVelocity = velocity + current_timestep * nonPressureAcc;

        // set predicted velocity and reset non pressure accelerations
        collection->get<IISPHParticleData>(particleIndex).predicted_velocity = predictedVelocity;
        nonPressureAcc = glm::vec2(0.0f);
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    glm::vec2 IISPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::ComputeViscosityAcceleration(
        pIndex_t particleIndex)
    {
        const MovementData& mData = collection->get<MovementData>(particleIndex);
        const glm::vec2& position = mData.position;
        const glm::vec2& velocity = mData.velocity;


        glm::vec2 tmp = glm::vec2(0.0f);
        auto neighbors = neighborhood_search.get_neighbors(particleIndex);
        for (uint32_t neighbor : neighbors)
        {
            auto type = collection->get<ParticleInfo>(neighbor).type;
            if (type == ParticleTypeDead)
            {
                continue; // don*t calculate unnecessary values for dead particles.
            }

            const MovementData& neighbor_mData = collection->get<MovementData>(neighbor);
            const glm::vec2& neighborPosition = neighbor_mData.position;
            const glm::vec2& neighborVelocity = neighbor_mData.velocity;

            const ParticleData& neighbor_pData = collection->get<ParticleData>(neighbor);
            float neighborMass = neighbor_pData.mass;
            float neighborDensity = neighbor_pData.density;

            if (neighborDensity == 0.0f)
                continue;

            glm::vec2 vij = velocity - neighborVelocity;
            glm::vec2 xij = position - neighborPosition;

            tmp += (neighborMass / neighborDensity) *
                   (glm::dot(vij, xij) /
                    (glm::dot(xij, xij) + 0.01f * parameters.particle_size * parameters.particle_size)) *
                   kernel.GetKernelDerivativeReversedValue(neighborPosition, position);
        }

        glm::vec2 viscosityAcceleration = 2.0f * settings.Viscosity * tmp;
        return viscosityAcceleration;
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    void IISPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::ComputeSourceTerm(pIndex_t particleIndex)
    {
        float particleDensity = collection->get<ParticleData>(particleIndex).density;


        const glm::vec2& particlePredictedVelocity =
            collection->get<IISPHParticleData>(particleIndex).predicted_velocity;
        const glm::vec2& particlePosition = collection->get<MovementData>(particleIndex).position;

        float sum = 0.0f;
        auto neighbors = neighborhood_search.get_neighbors(particleIndex);
        for (uint32_t neighborIndex : neighbors)
        {
            auto neighborType = collection->get<ParticleInfo>(neighborIndex).type;
            if (neighborType == ParticleTypeDead)
                continue; // we do not want to process dead particles

            float neighborMass = collection->get<ParticleData>(neighborIndex).mass;
            const glm::vec2& neighborPredictedVelocity =
                collection->get<IISPHParticleData>(neighborIndex).predicted_velocity;
            const glm::vec2& neighborPosition = collection->get<MovementData>(neighborIndex).position;

            sum += neighborMass * glm::dot((particlePredictedVelocity - neighborPredictedVelocity),
                                           kernel.GetKernelDerivativeReversedValue(neighborPosition, particlePosition));
            // the velocity of a boundary particle in the next timestep is here equal to the predicted velocity of said
            // boundary particle, since there are no other forces that apply to them.
        }

        float sourceTerm = parameters.rest_density - particleDensity - current_timestep * sum;

        collection->get<IISPHParticleData>(particleIndex).source_term = sourceTerm;
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    void IISPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::ComputeDiagonalElement(pIndex_t particleIndex)
    {
        float sum = 0.0f;
        const glm::vec2& particlePosition = collection->get<MovementData>(particleIndex).position;
        float particleMass = collection->get<ParticleData>(particleIndex).mass;

        // first and third part of the sum (since we sum over normal particles and boundary particles)
        auto neighbors = neighborhood_search.get_neighbors(particleIndex);
        for (uint32_t neighborIndex : neighbors)
        {
            auto neighborType = collection->get<ParticleInfo>(neighborIndex).type;
            if (neighborType == ParticleTypeDead)
                continue; // we do not want to process dead particles

            float neighborMass = collection->get<ParticleData>(neighborIndex).mass;
            const glm::vec2& neighborPosition = collection->get<MovementData>(neighborIndex).position;

            glm::vec2 internalSum = glm::vec2(0.0f);
            auto neighborsInternal = neighborhood_search.get_neighbors(particleIndex);
            for (uint32_t internalNeighbor : neighborsInternal)
            {
                auto internalNeighborType = collection->get<ParticleInfo>(internalNeighbor).type;
                if (internalNeighborType == ParticleTypeDead)
                    continue; // we do not want to process dead particles

                float internalNeighborMass = collection->get<ParticleData>(internalNeighbor).mass;
                const glm::vec2& internalNeighborPosition = collection->get<MovementData>(internalNeighbor).position;

                if (internalNeighborType == ParticleTypeNormal)
                {
                    internalSum += internalNeighborMass / parameters.rest_density / parameters.rest_density *
                                   kernel.GetKernelDerivativeReversedValue(internalNeighborPosition, particlePosition);
                }
                else if (internalNeighborType == ParticleTypeBoundary)
                {
                    internalSum += 2.0f * settings.Gamma * internalNeighborMass / parameters.rest_density /
                                   parameters.rest_density *
                                   kernel.GetKernelDerivativeReversedValue(internalNeighborPosition, particlePosition);
                }

                // since we have no factor for boundary particles, they can be handled as normal particles
            }

            sum += neighborMass *
                   glm::dot(-internalSum, kernel.GetKernelDerivativeReversedValue(neighborPosition, particlePosition));
        }

        // second part of the sum
        for (uint32_t neighborIndex : neighbors)
        {
            auto neighborType = collection->get<ParticleInfo>(neighborIndex).type;
            if (neighborType == ParticleTypeDead)
                continue; // we do not want to process dead particles

            if (neighborType != ParticleTypeNormal)
                continue; // this loop only processes normal particles

            float neighborMass = collection->get<ParticleData>(neighborIndex).mass;
            const glm::vec2& neighborPosition = collection->get<MovementData>(neighborIndex).position;

            sum +=
                neighborMass * glm::dot(particleMass / parameters.rest_density / parameters.rest_density *
                                            kernel.GetKernelDerivativeReversedValue(particlePosition, neighborPosition),
                                        kernel.GetKernelDerivativeReversedValue(neighborPosition, particlePosition));
        }


        float diagonalElement = current_timestep * current_timestep * sum;
        collection->get<IISPHParticleData>(particleIndex).diagonal_element = diagonalElement;
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    void IISPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::InitializePressure(pIndex_t particleIndex)
    {
        collection->get<ParticleData>(particleIndex).pressure = 0.0f;
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    void IISPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::IntegrateParticle(pIndex_t particleIndex)
    {
        auto type = collection->get<ParticleInfo>(particleIndex).type;
        if (type == ParticleTypeBoundary)
        {
            return; // don't calculate unnecessary values for the boundary particles.
        }
        if (type == ParticleTypeDead)
        {
            return; // don't calculate unnecessary values for dead particles.
        }

        auto& mData = collection->get<MovementData>(particleIndex);
        // integrate using euler cromer
        mData.velocity = collection->get<IISPHParticleData>(particleIndex).predicted_velocity +
                         current_timestep * mData.acceleration;
        mData.position = mData.position + current_timestep * mData.velocity;
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    void IISPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::ComputePressure()
    {
        size_t iteration = 0;
        float predictedDensityError = 0.0f;

        // iteration
        while ((iteration < settings.MinNumberOfIterations ||
                abs(predictedDensityError) > settings.MaxDensityErrorAllowed) &&
               iteration < settings.MaxNumberOfIterations)
        {

            // pre calculations
            predictedDensityError = 0.0f;
            size_t densityErrorCounter = 0;

            // first loop: compute pressure acceleration
            parallel::loop_for(0, collection->size(), [&](pIndex_t particleIndex) {
                auto particleType = collection->get<ParticleInfo>(particleIndex).type;
                if (particleType == ParticleTypeDead)
                    return; // we do not want to process dead particles
                if (particleType == ParticleTypeBoundary)
                    return;

                float particlePressure = collection->get<ParticleData>(particleIndex).pressure;
                const glm::vec2& particlePosition = collection->get<MovementData>(particleIndex).position;

                glm::vec2 sum = glm::vec2(0.0f);
                auto neighbors = neighborhood_search.get_neighbors(particleIndex);
                for (uint32_t neighborIndex : neighbors)
                {
                    auto neighborType = collection->get<ParticleInfo>(neighborIndex).type;
                    if (neighborType == ParticleTypeDead)
                        continue; // we do not want to process dead particles

                    const glm::vec2& neighborPosition = collection->get<MovementData>(neighborIndex).position;
                    float neighborMass = collection->get<ParticleData>(neighborIndex).mass;

                    if (neighborType == ParticleTypeNormal)
                    {
                        float neighborPressure = collection->get<ParticleData>(neighborIndex).pressure;

                        sum += neighborMass *
                               (particlePressure / parameters.rest_density / parameters.rest_density +
                                neighborPressure / parameters.rest_density / parameters.rest_density) *
                               kernel.GetKernelDerivativeReversedValue(neighborPosition, particlePosition);
                    }
                    else if (neighborType == ParticleTypeBoundary)
                    {
                        // pressure is mirrored here
                        sum += settings.Gamma * neighborMass * 2.0f *
                               (particlePressure / parameters.rest_density / parameters.rest_density) *
                               kernel.GetKernelDerivativeReversedValue(neighborPosition, particlePosition);
                    }
                }

                collection->get<MovementData>(particleIndex).acceleration = (-sum);
            });

            // second loop
            parallel::loop_for(0, collection->size(), [&](pIndex_t particleIndex) {
                auto particleType = collection->get<ParticleInfo>(particleIndex).type;
                if (particleType == ParticleTypeDead)
                    return; // we do not want to process dead particles
                if (particleType == ParticleTypeBoundary)
                    return;

                // First step calculate Ap
                const glm::vec2& particlePressureAcceleration =
                    collection->get<MovementData>(particleIndex).acceleration;
                const glm::vec2& particlePosition = collection->get<MovementData>(particleIndex).position;

                float sum = 0.0f;
                auto neighbors = neighborhood_search.get_neighbors(particleIndex);
                for (uint32_t neighborIndex : neighbors)
                {
                    auto neighborType = collection->get<ParticleInfo>(neighborIndex).type;
                    if (neighborType == ParticleTypeDead)
                        continue; // we do not want to process dead particles

                    float neighborMass = collection->get<ParticleData>(neighborIndex).mass;
                    const glm::vec2& neighborPosition = collection->get<MovementData>(neighborIndex).position;

                    if (neighborType == ParticleTypeNormal)
                    {
                        const glm::vec2& neighborPressureAcceleration =
                            collection->get<MovementData>(neighborIndex).acceleration;

                        sum += neighborMass *
                               glm::dot((particlePressureAcceleration - neighborPressureAcceleration),
                                        kernel.GetKernelDerivativeReversedValue(neighborPosition, particlePosition));
                    }
                    else if (neighborType == ParticleTypeBoundary)
                    {
                        sum += neighborMass *
                               glm::dot(particlePressureAcceleration,
                                        kernel.GetKernelDerivativeReversedValue(neighborPosition, particlePosition));
                    }
                }

                float Ap = current_timestep * current_timestep * sum;

                // Second step: Update pressure
                float particleDiagonalElement = collection->get<IISPHParticleData>(particleIndex).diagonal_element;
                float particleSourceTerm = collection->get<IISPHParticleData>(particleIndex).source_term;
                float& particlePressure = collection->get<ParticleData>(particleIndex).pressure;
                if (std::abs(particleDiagonalElement) > std::numeric_limits<float>::epsilon())
                {
                    float particlePressureNextStep =
                        std::fmax(0.0f, particlePressure +
                                            settings.Omega * ((particleSourceTerm - Ap) / particleDiagonalElement));
                    // update pressure
                    particlePressure = particlePressureNextStep;
                }
                else
                {
                    // otherwise set pressure to zero
                    particlePressure = 0.0f;
                }

                // Info: The calculation of the particle density error is based on the implementation of IISPH by Stefan
                // Band Third step: Calculate predicted density error
                float particleDensityError = std::abs(particleSourceTerm - Ap);
                if (std::abs(particleDiagonalElement) > std::numeric_limits<float>::epsilon())
                {
                    if (std::abs(particlePressure) <= std::numeric_limits<float>::epsilon())
                    {
                        // if the new pressure is zero, we do not have a density error
                        particleDensityError = 0.0f;
                    }

                    predictedDensityError += particleDensityError;
                    densityErrorCounter++;
                }
            });

            if (densityErrorCounter != 0)
            {
                // post calculations: calculate arithmetic average density error
                predictedDensityError = predictedDensityError / (float)densityErrorCounter;
                // std::cout << iteration << "\t" << predictedDensityError << std::endl;
            }
            else
            {
                predictedDensityError = 0.0f;
                // std::cout << iteration << "\t" << "No particles contributing to density error" << std::endl;
            }

            // increate iteration count
            iteration++;
        }
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    void IISPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::adapt_collection(ParticleCollection& collection)
    {
        FLUID_ASSERT(!collection.is_type_present<IISPHParticleData>());
        collection.add_type<IISPHParticleData>();
    }
} // namespace FluidSolver


#endif // FLUIDSOLVER_IISPHFLUIDSOLVER_HPP
