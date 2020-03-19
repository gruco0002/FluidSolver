#ifndef FLUIDSOLVER_IPARTICLECOLLECTION_HPP
#define FLUIDSOLVER_IPARTICLECOLLECTION_HPP

#include <functional>
#include <vector>
#include "libraries/glm/glm.hpp"


namespace FluidSolver {

    enum ParticleType {
        ParticleTypeNormal = 0,
        ParticleTypeBoundary = 1,
        ParticleTypeDead = 2,
    };


    struct FluidParticle {
        glm::vec2 Position;
        glm::vec2 Velocity;
        glm::vec2 Acceleration;

        float Mass;
        float Pressure;
        float Density;

        uint8_t Type;
        glm::vec2 PredictedVelocity = glm::vec2(0.0f);
        glm::vec2 NonPressureAcceleration = glm::vec2(0.0f);
        float SourceTerm = 0.0f;
        float DiagonalElement = 0.0f;

        bool operator==(const FluidParticle &rhs) const;

        bool operator!=(const FluidParticle &rhs) const;


    };

    /**
     * Particle collections are data structures that contain the particle data.
     * Each particle has an index. This index can change arbitrarily due to sorting or other things.
     * Therefore a consistent particle id is introduced. The particle id is attached to the particle data and is not
     * affected by sorting or other internal structure related data movement.
     * The getter and setter of the individual values of the particle use the index.
     * To obtain the index of a particle with a certain id, use the GetIndex(size_t particleID) method. Vice versa
     * there exists a GetParticleID(size_t index) function to obtain the particle id.
     *
     * It is recommended to iterate over the particle collection via the indices and not the ids since the data is
     * contained compact in memory and always sorted by the index in an array like structure. Iterating over the indices
     * is therefore more cache efficient.
     *
     * The particle collection can be sorted with a own custom search key that is calculated by a functional. There are
     * two possibilities for sorting the collection: merge sort and insertion sort. Use merge sort for an initial sort
     * or if you know, that the collection is very unsorted. However in most cases the particle collection stays roughly
     * sorted due to temporal or spatial coherence of the particles in the system. Therefore using insertion sort on the
     * nearly sorted collection can be quite more efficient.
     *
     * The GetSize() function returns the size of the collection. Particles with indices from (including) 0 to
     * (excluding) size are contained in the collection and can be accessed. Access outside this range is, due to
     * performance not checked and can result in memory corruption. Particle IDs exist in the same range.
     */
    class IParticleCollection {
    public:
        /**
         * Functional that should return a sort key value for the given particle index.
         */
        typedef std::function<uint64_t(const size_t)> sortKeyFunction_t;

    protected:

        /**
         * Returns the sort key of a given particle.
         * @param index Particle index
         * @return Sort key.
         */
        virtual uint64_t GetSortKey(size_t index) = 0;

        /**
         * This function is called by all sorting algorithms to precalculate the sort keys in order to save performance.
         * @param sortKeyFunction Functional that should return a sort key value for the given particle index.
         */
        virtual void PrecalculateSortKeys(const sortKeyFunction_t &sortKeyFunction) = 0;

        /**
         * Swaps the data of two given particles. This function is used by sort functions.
         * @param i Index of the first particle
         * @param j Index of the second particle
         */
        virtual void SwapElements(size_t i, size_t j) = 0;


    public:

        /**
         * Executes an insertion sort for the particle collection. The collection is sorted by the sort key in ascending
         * order. The sort key is calculated by the given functional.
         * @param sortKeyFunction Functional that should return a sort key value for the given particle index.
         * @remarks The functional can be executed in parallel and should therefore be thread safe. The sort keys could
         * also be precalculated before the sorting itself happens.
         * @note Insertion sort should be used if the collection is mostly sorted in regards to the given sort key.
         */
        void InsertionSort(const sortKeyFunction_t &sortKeyFunction);

        /**
         * Executes a merge sort for the particle collection. The collection is sorted by the sort key in ascending
         * order. The sort key is calculated by the given functional.
         * @param sortKeyFunction Functional that should return a sort key value for the given particle index.
         * @remarks The functional can be executed in parallel and should therefore be thread safe. The sort keys could
         * also be precalculated before the sorting itself happens.
         * @note Merge sort should be used if the collection is not mostly sorted or the sort state is unknown in
         * regards to the given sort key.
         */
        void MergeSort(const sortKeyFunction_t &sortKeyFunction);

    public:
        virtual ~IParticleCollection() = default;

        virtual size_t GetParticleID(size_t index) = 0;

        virtual size_t GetIndex(size_t particleID) = 0;

        /**
         * Adds a vector of particles to the collection. The indices of the added particles are not returned.
         * @param particles The particles that should be added to the particle collection.
         * @note If the particle indices are needed for further calculations you should consider using the AddParticle
         * function of the collection.
         */
        virtual void AddParticles(const std::vector<FluidParticle> &particles);

        /**
         * Adds a particle with the given data and returns its index.
         * @param fluidParticle The particle data that should be added to a new particle in the collection.
         * @return Particle Index.
         */
        virtual size_t AddParticle(const FluidParticle &fluidParticle) = 0;

        /**
         * Add a particle with no data to the collection and returns its index.
         * @return Particle index.
         */
        virtual size_t AddEmptyParticle() = 0;


    public:

        virtual float GetMass(uint32_t index) = 0;

        virtual void SetMass(uint32_t index, float value) = 0;

        virtual float GetPressure(uint32_t index) = 0;

        virtual void SetPressure(uint32_t index, float value) = 0;

        virtual glm::vec2 GetPosition(uint32_t index) = 0;

        virtual void SetPosition(uint32_t index, glm::vec2 value) = 0;

        virtual glm::vec2 GetVelocity(uint32_t index) = 0;

        virtual void SetVelocity(uint32_t index, glm::vec2 value) = 0;

        virtual glm::vec2 GetAcceleration(uint32_t index) = 0;

        virtual void SetAcceleration(uint32_t index, glm::vec2 value) = 0;

        virtual uint32_t GetSize() = 0;

        virtual float GetDensity(uint32_t index) = 0;

        virtual void SetDensity(uint32_t index, float value) = 0;

        virtual ParticleType GetParticleType(uint32_t index) = 0;

        virtual void SetParticleType(uint32_t index, ParticleType value) = 0;

        virtual glm::vec2 GetNonPressureAcceleration(uint32_t index) = 0;

        virtual void SetNonPressureAcceleration(uint32_t index, glm::vec2 value) = 0;

        virtual glm::vec2 GetPredictedVelocity(uint32_t index) = 0;

        virtual void SetPredictedVelocity(uint32_t index, glm::vec2 value) = 0;

        virtual float GetSourceTerm(uint32_t index) = 0;

        virtual void SetSourceTerm(uint32_t index, float value) = 0;

        virtual float GetDiagonalElement(uint32_t index) = 0;

        virtual void SetDiagonalElement(uint32_t index, float value) = 0;


    };

}
#endif //FLUIDSOLVER_IPARTICLECOLLECTION_HPP
