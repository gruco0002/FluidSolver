//
// Created by corbi on 24.04.2019.
//

#ifndef FLUIDSOLVER_PARTICLEVERTEXARRAY_HPP
#define FLUIDSOLVER_PARTICLEVERTEXARRAY_HPP


#include <core/SimpleParticleCollection.hpp>
#include <engine/graphics/buffer/VertexBuffer.hpp>
#include <engine/graphics/buffer/IndexBuffer.hpp>
#include <engine/graphics/buffer/VertexArray.hpp>

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

    ParticleVertexArray(FluidSolver::SimpleParticleCollection *simpleParticleCollection);

    void Update();

    void Draw();

    virtual ~ParticleVertexArray();

private:

    uint32_t particleCount;

    FluidSolver::SimpleParticleCollection *simpleParticleCollection;

    void Generate();

    void Delete();

    Engine::Graphics::Buffer::VertexBuffer<FluidSolver::SimpleParticleCollection::FluidParticle> *vertexBuffer;
    Engine::Graphics::Buffer::IndexBuffer<uint32_t> *indexBuffer;
    Engine::Graphics::Buffer::VertexArray *vao;

};


#endif //FLUIDSOLVER_PARTICLEVERTEXARRAY_HPP
