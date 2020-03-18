//
// Created by corbi on 24.04.2019.
//

#include "ParticleVertexArray.hpp"

ParticleVertexArray::ParticleVertexArray(FluidSolver::SimpleParticleCollection *simpleParticleCollection) {
    this->simpleParticleCollection = simpleParticleCollection;
    Generate();
}

void ParticleVertexArray::Update(FluidSolver::IParticleSelection *particleSelection) {
    this->particleCount = simpleParticleCollection->GetSize();
    if (vertexBuffer->GetElementCount() >= this->particleCount) {
        // the size of the buffer is large enough for the particles: Update the data because it is faster
        vertexBuffer->UpdateData(simpleParticleCollection->GetParticles());
    } else {
        // the buffer is too small, set the data to force a resize
        vertexBuffer->SetData(simpleParticleCollection->GetParticles());
    }

    // first update the selection data, then update the buffer
    UpdateSelectionData(particleSelection);
    if (selectionBuffer->GetElementCount() >= this->selectionData.size()) {
        // the size of the buffer is large enough for the particles: Update the data because it is faster
        selectionBuffer->UpdateData(selectionData);
    } else {
        // the buffer is too small, set the data to force a resize
        selectionBuffer->SetData(selectionData);
    }

    // resize index buffer iff needed
    if (indexBuffer->GetElementCount() < this->particleCount) {
        std::vector<uint32_t> indices(this->particleCount);
        for (uint32_t i = 0; i < this->particleCount; i++) {
            indices[i] = i;
        }
        indexBuffer->SetData(indices);
    }

}

void ParticleVertexArray::Draw() {
    vao->Draw(GL_POINTS, particleCount);
}

void ParticleVertexArray::Generate() {
    this->particleCount = simpleParticleCollection->GetSize();

    vertexBuffer = new Engine::Graphics::Buffer::VertexBuffer<FluidSolver::FluidParticle>(
            simpleParticleCollection->GetParticles(), Engine::Graphics::Buffer::Buffer::DataModeDynamic);
    std::vector<uint32_t> indices(this->particleCount);
    for (uint32_t i = 0; i < this->particleCount; i++) {
        indices[i] = i;
    }
    indexBuffer = new Engine::Graphics::Buffer::IndexBuffer<uint32_t>(indices);

    selectionData.resize(this->particleCount);
    selectionBuffer = new Engine::Graphics::Buffer::VertexBuffer<int8_t>(selectionData,
                                                                         Engine::Graphics::Buffer::Buffer::DataModeDynamic);


    vao = new Engine::Graphics::Buffer::VertexArray({

                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    indexBuffer),

                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    vertexBuffer,
                                                                    0, 2, offsetof(
                                                                            FluidSolver::FluidParticle,
                                                                            Position),
                                                                    sizeof(FluidSolver::FluidParticle),
                                                                    Engine::ComponentTypeFloat

                                                            ),
                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    vertexBuffer,
                                                                    1, 2, offsetof(
                                                                            FluidSolver::FluidParticle,
                                                                            Velocity),
                                                                    sizeof(FluidSolver::FluidParticle),
                                                                    Engine::ComponentTypeFloat

                                                            ),

                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    vertexBuffer,
                                                                    2, 2, offsetof(
                                                                            FluidSolver::FluidParticle,
                                                                            Acceleration),
                                                                    sizeof(FluidSolver::FluidParticle),
                                                                    Engine::ComponentTypeFloat

                                                            ),

                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    vertexBuffer,
                                                                    3, 1, offsetof(
                                                                            FluidSolver::FluidParticle,
                                                                            Mass),
                                                                    sizeof(FluidSolver::FluidParticle),
                                                                    Engine::ComponentTypeFloat

                                                            ),
                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    vertexBuffer,
                                                                    4, 1, offsetof(
                                                                            FluidSolver::FluidParticle,
                                                                            Pressure),
                                                                    sizeof(FluidSolver::FluidParticle),
                                                                    Engine::ComponentTypeFloat

                                                            ),
                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    vertexBuffer,
                                                                    5, 1, offsetof(
                                                                            FluidSolver::FluidParticle,
                                                                            Density),
                                                                    sizeof(FluidSolver::FluidParticle),
                                                                    Engine::ComponentTypeFloat

                                                            ),
                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    vertexBuffer,
                                                                    6, 1, offsetof(
                                                                            FluidSolver::FluidParticle,
                                                                            Type),
                                                                    sizeof(FluidSolver::FluidParticle),
                                                                    Engine::ComponentTypeUnsignedByte
                                                            ),
                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    selectionBuffer,
                                                                    7, 1, 0, sizeof(int8_t),
                                                                    Engine::ComponentTypeByte
                                                            )

                                                    });

}


ParticleVertexArray::~ParticleVertexArray() {
    delete vao;
    delete vertexBuffer;
    delete indexBuffer;
    delete selectionBuffer;
}

void ParticleVertexArray::UpdateSelectionData(FluidSolver::IParticleSelection *particleSelection) {
    size_t size = simpleParticleCollection->GetSize();
    selectionData.resize(size);

#pragma omp parallel for
    for (int64_t particleIndex = 0; particleIndex < size; particleIndex++) {
        bool selected = particleSelection->IsParticleSelected(particleIndex, simpleParticleCollection);
        selectionData[particleIndex] = selected ? 1 : 0;
    }
}
