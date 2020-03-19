//
// Created by corbi on 24.04.2019.
//

#ifndef FLUIDSOLVER_PARTICLEVERTEXARRAY_HPP
#define FLUIDSOLVER_PARTICLEVERTEXARRAY_HPP


#include <core/fluidSolver/particleCollection/ParticleCollection.hpp>
#include <engine/graphics/buffer/VertexBuffer.hpp>
#include <engine/graphics/buffer/IndexBuffer.hpp>
#include <engine/graphics/buffer/VertexArray.hpp>
#include <core/selection/IParticleSelection.hpp>

class ParticleVertexArray {

/**
 * Layout Location|type|name
 *
 * 0|vec2|position
 * 1|vec2|velocity
 * 2|vec2|acceleration
 * 3|float|mass
 * 4|float|pressure
 * 5|float|density
 * 6|uint8|type
 */


public:

    explicit ParticleVertexArray(FluidSolver::ParticleCollection *simpleParticleCollection);

    void Update(FluidSolver::IParticleSelection *particleSelection);

    void Draw();

    virtual ~ParticleVertexArray();

private:

    std::vector<int8_t> selectionData;

    void UpdateSelectionData(FluidSolver::IParticleSelection *particleSelection);

    uint32_t particleCount = 0;

    FluidSolver::ParticleCollection *simpleParticleCollection = nullptr;

    void Generate();


    Engine::Graphics::Buffer::VertexBuffer<glm::vec2> *positionBuffer = nullptr;
    Engine::Graphics::Buffer::VertexBuffer<glm::vec2> *velocityBuffer = nullptr;
    Engine::Graphics::Buffer::VertexBuffer<glm::vec2> *accelerationBuffer = nullptr;
    Engine::Graphics::Buffer::VertexBuffer<float> *massBuffer = nullptr;
    Engine::Graphics::Buffer::VertexBuffer<float> *pressureBuffer = nullptr;
    Engine::Graphics::Buffer::VertexBuffer<float> *densityBuffer = nullptr;
    Engine::Graphics::Buffer::VertexBuffer<uint8_t> *typeBuffer = nullptr;

    Engine::Graphics::Buffer::IndexBuffer<uint32_t> *indexBuffer = nullptr;
    Engine::Graphics::Buffer::VertexArray *vao = nullptr;

    Engine::Graphics::Buffer::VertexBuffer<int8_t> *selectionBuffer = nullptr;

};


#endif //FLUIDSOLVER_PARTICLEVERTEXARRAY_HPP
