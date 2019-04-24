//
// Created by corbi on 24.04.2019.
//

#include "ParticleVertexArray.hpp"

ParticleVertexArray::ParticleVertexArray(FluidSolver::SimpleParticleCollection *simpleParticleCollection) {
    this->simpleParticleCollection = simpleParticleCollection;
    Generate();
}

void ParticleVertexArray::Update() {
    vertexBuffer->UpdateData(simpleParticleCollection->GetParticles());
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
