
#ifndef FLUIDSOLVER_PARTICLECOLLECTION_HPP
#define FLUIDSOLVER_PARTICLECOLLECTION_HPP

#include <vector>
#include <functional>
#include "IParticleCollection.hpp"
#include "SimpleParticleCollection.hpp"

namespace FluidSolver {
    class ParticleCollection : public IParticleCollection {

    public:
        typedef std::function<uint64_t(const size_t)> sortKeyFunction_t;

    private:

        std::vector<uint64_t> sortKeys;
        std::vector<size_t> particleIndexToDataIndexMap;

        std::vector<uint8_t> typeData;
        std::vector<glm::vec2> positionData;
        std::vector<glm::vec2> velocityData;
        std::vector<glm::vec2> accelerationData;
        std::vector<float> massData;
        std::vector<float> pressureData;
        std::vector<float> densityData;
        std::vector<glm::vec2> nonPressureAccelerationData;
        std::vector<glm::vec2> predictedVelocityData;
        std::vector<float> sourceTermData;
        std::vector<float> diagonalElementData;

        size_t size = 0;



        void PrecalculateSortKeys(const sortKeyFunction_t &sortKeyFunction);

    protected:
        virtual void SwapElements(size_t i, size_t j);

        virtual void AddEntryToEachArray();

    public:
        void InsertionSort(const sortKeyFunction_t &sortKeyFunction);
        void MergeSort(const sortKeyFunction_t &sortKeyFunction);

        void AddParticles(const std::vector<SimpleParticleCollection::FluidParticle> &particles);
        size_t AddParticle(const SimpleParticleCollection::FluidParticle &fluidParticle);

        size_t AddEmptyParticle();


    public:

        float GetMass(uint32_t index) override;
        void SetMass(uint32_t index, float value) override;
        float GetPressure(uint32_t index) override;
        void SetPressure(uint32_t index, float value) override;
        glm::vec2 GetPosition(uint32_t index) override;
        void SetPosition(uint32_t index, glm::vec2 value) override;
        glm::vec2 GetVelocity(uint32_t index) override;
        void SetVelocity(uint32_t index, glm::vec2 value) override;
        glm::vec2 GetAcceleration(uint32_t index) override;
        void SetAcceleration(uint32_t index, glm::vec2 value) override;
        uint32_t GetSize() override;
        float GetDensity(uint32_t index) override;
        void SetDensity(uint32_t index, float value) override;
        ParticleType GetParticleType(uint32_t index) override;
        void SetParticleType(uint32_t index, ParticleType value) override;
        glm::vec2 GetNonPressureAcceleration(uint32_t index) override;
        void SetNonPressureAcceleration(uint32_t index, glm::vec2 value) override;
        glm::vec2 GetPredictedVelocity(uint32_t index) override;
        void SetPredictedVelocity(uint32_t index, glm::vec2 value) override;
        float GetSourceTerm(uint32_t index) override;
        void SetSourceTerm(uint32_t index, float value) override;
        float GetDiagonalElement(uint32_t index) override;
        void SetDiagonalElement(uint32_t index, float value) override;


    };

}


#endif //FLUIDSOLVER_PARTICLECOLLECTION_HPP
