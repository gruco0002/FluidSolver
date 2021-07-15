#ifndef FLUIDSOLVER_PARTICLECOLLECTIONALGORITHM_HPP
#define FLUIDSOLVER_PARTICLECOLLECTIONALGORITHM_HPP

#include "ParticleCollection.hpp"

#include <functional>
#include <memory>

namespace FluidSolver
{

    namespace ParticleCollectionAlgorithm
    {


        class Sort {
          public:
            using key_function_t = std::function<uint64_t(const std::shared_ptr<ParticleCollection>&, const pIndex_t)>;


            struct SortInfo
            {
                uint64_t key;
            };


            /// Adds the appropriate component to a particle collection in order to be able to sort it.
            /// \param collection
            void adapt_collection(std::shared_ptr<ParticleCollection> collection);

            void merge_sort(std::shared_ptr<ParticleCollection>, const key_function_t& key);

            void insertion_sort(std::shared_ptr<ParticleCollection>, const key_function_t& key);

            void quick_sort(std::shared_ptr<ParticleCollection>, const key_function_t& key);

          private:
            void precalculate_keys(std::shared_ptr<ParticleCollection>, const key_function_t& key);
        };


    } // namespace ParticleCollectionAlgorithm


} // namespace FluidSolver


#endif // FLUIDSOLVER_PARTICLECOLLECTIONALGORITHM_HPP
