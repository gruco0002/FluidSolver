//
// Created by corbi on 24.04.2019.
//

#include "ParticleVertexArray.hpp"

ParticleVertexArray::ParticleVertexArray(FluidSolver::ParticleCollection *simpleParticleCollection) {
    this->simpleParticleCollection = simpleParticleCollection;
    Generate();
}

void ParticleVertexArray::Update(FluidSolver::IParticleSelection *particleSelection) {
    this->particleCount = simpleParticleCollection->GetSize();
    if (positionBuffer->GetElementCount() >= this->particleCount) {
        // the size of the buffer is large enough for the particles: Update the data because it is faster
        positionBuffer->UpdateData(simpleParticleCollection->PositionData(), simpleParticleCollection->GetSize());
        velocityBuffer->UpdateData(simpleParticleCollection->VelocityData(), simpleParticleCollection->GetSize());
        accelerationBuffer->UpdateData(simpleParticleCollection->AccelerationData(),
                                       simpleParticleCollection->GetSize());

        massBuffer->UpdateData(simpleParticleCollection->MassData(), simpleParticleCollection->GetSize());
        densityBuffer->UpdateData(simpleParticleCollection->DensityData(), simpleParticleCollection->GetSize());
        pressureBuffer->UpdateData(simpleParticleCollection->PressureData(), simpleParticleCollection->GetSize());

        typeBuffer->UpdateData(simpleParticleCollection->TypeData(), simpleParticleCollection->GetSize());
    } else {
        // the buffer is too small, set the data to force a resize
        positionBuffer->SetData(simpleParticleCollection->PositionData(), simpleParticleCollection->GetSize());
        velocityBuffer->SetData(simpleParticleCollection->VelocityData(), simpleParticleCollection->GetSize());
        accelerationBuffer->SetData(simpleParticleCollection->AccelerationData(), simpleParticleCollection->GetSize());

        massBuffer->SetData(simpleParticleCollection->MassData(), simpleParticleCollection->GetSize());
        densityBuffer->SetData(simpleParticleCollection->DensityData(), simpleParticleCollection->GetSize());
        pressureBuffer->SetData(simpleParticleCollection->PressureData(), simpleParticleCollection->GetSize());

        typeBuffer->SetData(simpleParticleCollection->TypeData(), simpleParticleCollection->GetSize());
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

    // create particle buffers
    positionBuffer = new Engine::Graphics::Buffer::VertexBuffer<glm::vec2>(simpleParticleCollection->PositionData(),
                                                                           simpleParticleCollection->GetSize(),
                                                                           Engine::Graphics::Buffer::Buffer::DataModeDynamic);
    velocityBuffer = new Engine::Graphics::Buffer::VertexBuffer<glm::vec2>(simpleParticleCollection->VelocityData(),
                                                                           simpleParticleCollection->GetSize(),
                                                                           Engine::Graphics::Buffer::Buffer::DataModeDynamic);
    accelerationBuffer = new Engine::Graphics::Buffer::VertexBuffer<glm::vec2>(
            simpleParticleCollection->AccelerationData(), simpleParticleCollection->GetSize(),
            Engine::Graphics::Buffer::Buffer::DataModeDynamic);

    massBuffer = new Engine::Graphics::Buffer::VertexBuffer<float>(simpleParticleCollection->MassData(),
                                                                   simpleParticleCollection->GetSize(),
                                                                   Engine::Graphics::Buffer::Buffer::DataModeDynamic);
    densityBuffer = new Engine::Graphics::Buffer::VertexBuffer<float>(simpleParticleCollection->DensityData(),
                                                                      simpleParticleCollection->GetSize(),
                                                                      Engine::Graphics::Buffer::Buffer::DataModeDynamic);
    pressureBuffer = new Engine::Graphics::Buffer::VertexBuffer<float>(simpleParticleCollection->PressureData(),
                                                                       simpleParticleCollection->GetSize(),
                                                                       Engine::Graphics::Buffer::Buffer::DataModeDynamic);

    typeBuffer = new Engine::Graphics::Buffer::VertexBuffer<uint8_t>(simpleParticleCollection->TypeData(),
                                                                     simpleParticleCollection->GetSize(),
                                                                     Engine::Graphics::Buffer::Buffer::DataModeDynamic);

    // create indices
    std::vector<uint32_t> indices(this->particleCount);
    for (uint32_t i = 0; i < this->particleCount; i++) {
        indices[i] = i;
    }
    indexBuffer = new Engine::Graphics::Buffer::IndexBuffer<uint32_t>(indices);

    // create selection data buffer
    selectionData.resize(this->particleCount);
    selectionBuffer = new Engine::Graphics::Buffer::VertexBuffer<int8_t>(selectionData,
                                                                         Engine::Graphics::Buffer::Buffer::DataModeDynamic);


    // create vertex array
    vao = new Engine::Graphics::Buffer::VertexArray({

                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    indexBuffer),
                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    positionBuffer,
                                                                    0, 2, 0, sizeof(glm::vec2),
                                                                    Engine::ComponentTypeFloat
                                                            ),
                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    velocityBuffer,
                                                                    1, 2, 0, sizeof(glm::vec2),
                                                                    Engine::ComponentTypeFloat
                                                            ),
                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    accelerationBuffer,
                                                                    2, 2, 0, sizeof(glm::vec2),
                                                                    Engine::ComponentTypeFloat
                                                            ),
                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    massBuffer,
                                                                    3, 1, 0, sizeof(float),
                                                                    Engine::ComponentTypeFloat
                                                            ),
                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    pressureBuffer,
                                                                    4, 1, 0, sizeof(float),
                                                                    Engine::ComponentTypeFloat
                                                            ),
                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    densityBuffer,
                                                                    5, 1, 0, sizeof(float),
                                                                    Engine::ComponentTypeFloat
                                                            ),
                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    typeBuffer,
                                                                    6, 1, 0, sizeof(uint8_t),
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

    delete positionBuffer;
    delete velocityBuffer;
    delete accelerationBuffer;
    delete massBuffer;
    delete pressureBuffer;
    delete densityBuffer;
    delete typeBuffer;


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
