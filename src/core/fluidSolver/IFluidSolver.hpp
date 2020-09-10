#ifndef FLUIDSOLVER_IFLUIDSOLVER_HPP
#define FLUIDSOLVER_IFLUIDSOLVER_HPP

#include "ParticleCollection.hpp"
#include "FluidAssert.hpp"

namespace FluidSolver {

    /**
     * Interface for a fluid solver.
     * @tparam Kernel 2D SPH Kernel
     * @tparam NeighborhoodSearch An object that implements a neighborhood search on the particle collection
     * The objects has to implement the following behaviour:
     * Types:
     *      NeighborsIterator
     *          An iterator that allows to iterate over the neighbors.
     *          Dereferencing the iterator yields a size_t particle index.
     *      Neighbors
     *          An object that can be queried for an iterator to iterate over the neighbors.
     *          It must therefore support the following functions:
     *          NeighborsIterator begin();
     *          NeighborsIterator end();
     * Fields:
     *      ParticleCollection* collection;
     *      float search_radius;
     * Functions:
     *      void find_neighbors();
     *      void get_neighbors(size_t particleIndex);
     *      void get_neighbors(const glm::vec2 &position);
     *
     */
    template <typename Kernel, typename NeighborhoodSearch>
    class IFluidSolver {
    public:
        struct SimulationParameters{
            float rest_density = 1.0f;
            float gravity = 9.81f;
            float particle_size = 1.0f;
        } parameters;

        Kernel kernel;

        NeighborhoodSearch neighborhood_search;

        ParticleCollection *collection = nullptr;

        virtual void execute_simulation_step(float timestep) = 0;

        virtual ~IFluidSolver() = default;

    };


}
#endif //FLUIDSOLVER_IFLUIDSOLVER_HPP
