#ifndef FLUIDSOLVER_IFLUIDSOLVER_HPP
#define FLUIDSOLVER_IFLUIDSOLVER_HPP

#include "ParticleCollection.hpp"
#include "core/FluidInclude.hpp"
#include "core/fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp"

#include <memory>

namespace FluidSolver
{

    class IFluidSolverBase {
      public:
        struct SimulationParameters
        {
            pFloat rest_density = 1.0f;
            pFloat gravity = 9.81f;
            pFloat particle_size = 1.0f;
        } parameters;

        virtual void execute_simulation_step(pFloat timestep) = 0;

        virtual void initialize() = 0;

        virtual ~IFluidSolverBase() = default;

        virtual NeighborhoodInterface create_neighborhood_interface() = 0;

        std::shared_ptr<ParticleCollection> collection = nullptr;
    };

    /**
     * Interface for a fluid solver.
     * This is only there to define how every fluid solver should be
     * structured.
     *
     * @tparam Kernel 2D/3D SPH Kernel
     * @tparam NeighborhoodSearch An object that
     * implements a neighborhood search on the particle collection
     * The objects has to implement the following
     * behaviour:
     * Types:
     *      NeighborsIterator
     *          An iterator that allows to iterate over
     * the neighbors.
     *          Dereferencing the iterator yields a size_t particle index.
     *      Neighbors

     * *          An object that can be queried for an iterator to iterate over the neighbors.
     *          It must
     * therefore support the following functions:
     *          NeighborsIterator begin();
     * NeighborsIterator
     * end();
     * Fields:
     *      ParticleCollection* collection;
     *      float search_radius;
     *
     * Functions:
     *      void find_neighbors();
     *      Neighbors get_neighbors(size_t particleIndex);
     *
     * Neighbors get_neighbors(const glm::vec2 &position); (or in the case of 3d glm::vec3)
     *      void
     * initialize();
     *
     *      NeighborhoodInterface create_interface();
     *
     */
    template <typename Kernel, typename NeighborhoodSearch> class IFluidSolver : public IFluidSolverBase {
      public:
        Kernel kernel;

        NeighborhoodSearch neighborhood_search;

        static_assert((sizeof(Kernel) + sizeof(NeighborhoodSearch)) == 0, "You should not use this to inherit from!");
    };


} // namespace FluidSolver
#endif // FLUIDSOLVER_IFLUIDSOLVER_HPP
