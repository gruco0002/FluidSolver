//
// Created by corbi on 24.04.2019.
//

#include "ParticleVertexArray.hpp"

ParticleVertexArray::ParticleVertexArray(FluidSolver::SimpleParticleCollection *simpleParticleCollection) {
    this->simpleParticleCollection = simpleParticleCollection;
    Generate();
}

void ParticleVertexArray::Update() {
    this->particleCount = simpleParticleCollection->GetSize();
    if (vertexBuffer->GetElementCount() >= this->particleCount) {
        // the size of the buffer is large enough for the particles: Update the data because it is faster
        vertexBuffer->UpdateData(simpleParticleCollection->GetParticles());
    } else {
        // the buffer is too small, set the data to force a resize
        vertexBuffer->SetData(simpleParticleCollection->GetParticles());
    }
}

void ParticleVertexArray::Draw() {
    vao->Draw(GL_POINTS, particleCount);
}

void ParticleVertexArray::Generate() {
    this->particleCount = simpleParticleCollection->GetSize();

    vertexBuffer = new Engine::Graphics::Buffer::VertexBuffer<FluidSolver::SimpleParticleCollection::FluidParticle>(
            simpleParticleCollection->GetParticles(), Engine::Graphics::Buffer::Buffer::DataModeDynamic);
    std::vector<uint32_t> indices(this->particleCount);
    for (uint32_t i = 0; i < this->particleCount; i++) {
        indices[i] = i;
    }
    indexBuffer = new Engine::Graphics::Buffer::IndexBuffer<uint32_t>(indices);


    vao = new Engine::Graphics::Buffer::VertexArray({

                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    indexBuffer),

                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    vertexBuffer,
                                                                    0, 2, offsetof(
                                                                            FluidSolver::SimpleParticleCollection::FluidParticle,
                                                                            Position),
                                                                    sizeof(FluidSolver::SimpleParticleCollection::FluidParticle),
                                                                    Engine::ComponentTypeFloat

                                                            ),
                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    vertexBuffer,
                                                                    1, 2, offsetof(
                                                                            FluidSolver::SimpleParticleCollection::FluidParticle,
                                                                            Velocity),
                                                                    sizeof(FluidSolver::SimpleParticleCollection::FluidParticle),
                                                                    Engine::ComponentTypeFloat

                                                            ),

                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    vertexBuffer,
                                                                    2, 2, offsetof(
                                                                            FluidSolver::SimpleParticleCollection::FluidParticle,
                                                                            Acceleration),
                                                                    sizeof(FluidSolver::SimpleParticleCollection::FluidParticle),
                                                                    Engine::ComponentTypeFloat

                                                            ),

                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    vertexBuffer,
                                                                    3, 1, offsetof(
                                                                            FluidSolver::SimpleParticleCollection::FluidParticle,
                                                                            Mass),
                                                                    sizeof(FluidSolver::SimpleParticleCollection::FluidParticle),
                                                                    Engine::ComponentTypeFloat

                                                            ),
                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    vertexBuffer,
                                                                    4, 1, offsetof(
                                                                            FluidSolver::SimpleParticleCollection::FluidParticle,
                                                                            Pressure),
                                                                    sizeof(FluidSolver::SimpleParticleCollection::FluidParticle),
                                                                    Engine::ComponentTypeFloat

                                                            ),
                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    vertexBuffer,
                                                                    5, 1, offsetof(
                                                                            FluidSolver::SimpleParticleCollection::FluidParticle,
                                                                            Density),
                                                                    sizeof(FluidSolver::SimpleParticleCollection::FluidParticle),
                                                                    Engine::ComponentTypeFloat

                                                            ),
                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    vertexBuffer,
                                                                    6, 1, offsetof(
                                                                            FluidSolver::SimpleParticleCollection::FluidParticle,
                                                                            Type),
                                                                    sizeof(FluidSolver::SimpleParticleCollection::FluidParticle),
                                                                    Engine::ComponentTypeUnsignedByte
                                                            ),


                                                    });

}

void ParticleVertexArray::Delete() {
    delete vao;
    delete vertexBuffer;
    delete indexBuffer;
}

ParticleVertexArray::~ParticleVertexArray() {
    Delete();
}
