#ifndef FLUIDSOLVER_PARTICLECOLLECTIONALGORITHM_HPP
#define FLUIDSOLVER_PARTICLECOLLECTIONALGORITHM_HPP

#include "ParticleCollection.hpp"

#include <functional>

namespace FluidSolver
{

    namespace ParticleCollectionAlgorithm
    {


        class Sort {
          public:
            using key_function_t = std::function<uint64_t(ParticleCollection& collection, const pIndex_t)>;


            struct SortInfo
            {
                uint64_t key;
            };


            /// Adds the appropriate component to a particle collection in order to be able to sort it.
            /// \param collection
            void adapt_collection(ParticleCollection& collection);

            void merge_sort(ParticleCollection& collection, const key_function_t& key);

            void insertion_sort(ParticleCollection& collection, const key_function_t& key);

          private:
            void precalculate_keys(ParticleCollection& collection, const key_function_t& key);
        };


    } // namespace ParticleCollectionAlgorithm


} // namespace FluidSolver


#endif // FLUIDSOLVER_PARTICLECOLLECTIONALGORITHM_HPP