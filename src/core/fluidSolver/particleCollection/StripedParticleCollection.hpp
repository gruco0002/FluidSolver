
#ifndef FLUIDSOLVER_STRIPEDPARTICLECOLLECTION_HPP
#define FLUIDSOLVER_STRIPEDPARTICLECOLLECTION_HPP

#include <vector>
#include <functional>
#include "IParticleCollection.hpp"

namespace FluidSolver {

    /**
     * The striped particle collection manages its data as separate arrays and is therefore called striped.
     * Each particle attribute is organized in its own dynamic array.
     * For more information see FluidSolver::IParticleCollection
     */
    class StripedParticleCollection : public IParticleCollection {

    private:

        std::vector<uint64_t> sortKeys;
        std::vector<size_t> particleIDToParticleIndexMap;
        std::vector<size_t> particleIndexToParticleIDMap;

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
    protected:
        uint64_t GetSortKey(size_t index) override;

    private:
        void PrecalculateSortKeys(const sortKeyFunction_t &sortKeyFunction) override;

    protected:
        void SwapElements(size_t i, size_t j) override;
        virtual void AddEntryToEachArray();

    public:



        size_t AddParticle(const FluidParticle &fluidParticle) override ;

        size_t AddEmptyParticle() override ;

    public:

         uint8_t * TypeData();
         glm::vec2* PositionData();
         glm::vec2* VelocityData();
         glm::vec2* AccelerationData();
         float * MassData();
         float * PressureData();
         float * DensityData();

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

        size_t GetParticleID(size_t index) override;

        size_t GetIndex(size_t particleID) override;


    };

}


#endif //FLUIDSOLVER_STRIPEDPARTICLECOLLECTION_HPP
