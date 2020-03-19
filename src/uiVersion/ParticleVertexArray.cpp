#include <stdexcept>
#include "ParticleVertexArray.hpp"

ParticleVertexArray::ParticleVertexArray(FluidSolver::IParticleCollection *particleCollection) {
    this->particleCollection = particleCollection;
}

void ParticleVertexArray::Update(FluidSolver::IParticleSelection *particleSelection) {
    // update the internal particle count
    this->vaoParticleCount = particleCollection->GetSize();

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
    if (indexBuffer->GetElementCount() < this->vaoParticleCount) {
        std::vector<uint32_t> indices(this->vaoParticleCount);
        for (uint32_t i = 0; i < this->vaoParticleCount; i++) {
            indices[i] = i;
        }
        indexBuffer->SetData(indices);
    }

    // update other buffers or data
    OnUpdate();
}

void ParticleVertexArray::Draw() {
    vao->Draw(GL_POINTS, vaoParticleCount);
}

void ParticleVertexArray::Generate() {
    // update the particle count variable
    this->vaoParticleCount = particleCollection->GetSize();

    // create indices
    std::vector<uint32_t> indices(this->vaoParticleCount);
    for (uint32_t i = 0; i < this->vaoParticleCount; i++) {
        indices[i] = i;
    }
    indexBuffer = new Engine::Graphics::Buffer::IndexBuffer<uint32_t>(indices);

    // create selection data buffer
    selectionData.resize(this->vaoParticleCount);
    selectionBuffer = new Engine::Graphics::Buffer::VertexBuffer<int8_t>(selectionData,
                                                                         Engine::Graphics::Buffer::Buffer::DataModeDynamic);

    this->OnGenerate();


}


ParticleVertexArray::~ParticleVertexArray() {
    delete vao;

    delete indexBuffer;
    delete selectionBuffer;
}

void ParticleVertexArray::UpdateSelectionData(FluidSolver::IParticleSelection *particleSelection) {
    size_t size = particleCollection->GetSize();
    selectionData.resize(size);

#pragma omp parallel for
    for (int64_t particleIndex = 0; particleIndex < size; particleIndex++) {
        bool selected = particleSelection->IsParticleSelected(particleIndex, particleCollection);
        selectionData[particleIndex] = selected ? 1 : 0;
    }
}

Engine::Graphics::Buffer::IndexBuffer<uint32_t> *ParticleVertexArray::GetIndexBuffer() {
    return indexBuffer;
}

Engine::Graphics::Buffer::VertexBuffer<int8_t> *ParticleVertexArray::GetSelectionBuffer() {
    return selectionBuffer;
}

uint32_t ParticleVertexArray::GetVaoParticleCount() {
    return vaoParticleCount;
}

ParticleVertexArray *
ParticleVertexArray::CreateFromParticleCollection(FluidSolver::IParticleCollection *particleCollection) {
    auto striped = dynamic_cast<FluidSolver::StripedParticleCollection *>(particleCollection);
    if (striped != nullptr) {
        return new ParticleVertexArrayForStripedParticleCollection(striped);
    }

    auto compact = dynamic_cast<FluidSolver::CompactParticleCollection *>(particleCollection);
    if (compact != nullptr) {
        return new ParticleVertexArrayForCompactParticleCollection(compact);
    }

    throw std::invalid_argument(
            "Could not find a type of particle vertex array matching the given type of the particle collection!");
}

void ParticleVertexArrayForStripedParticleCollection::OnGenerate() {
    // create particle buffers
    positionBuffer = new Engine::Graphics::Buffer::VertexBuffer<glm::vec2>(stripedParticleCollection->PositionData(),
                                                                           stripedParticleCollection->GetSize(),
                                                                           Engine::Graphics::Buffer::Buffer::DataModeDynamic);
    velocityBuffer = new Engine::Graphics::Buffer::VertexBuffer<glm::vec2>(stripedParticleCollection->VelocityData(),
                                                                           stripedParticleCollection->GetSize(),
                                                                           Engine::Graphics::Buffer::Buffer::DataModeDynamic);
    accelerationBuffer = new Engine::Graphics::Buffer::VertexBuffer<glm::vec2>(
            stripedParticleCollection->AccelerationData(), stripedParticleCollection->GetSize(),
            Engine::Graphics::Buffer::Buffer::DataModeDynamic);

    massBuffer = new Engine::Graphics::Buffer::VertexBuffer<float>(stripedParticleCollection->MassData(),
                                                                   stripedParticleCollection->GetSize(),
                                                                   Engine::Graphics::Buffer::Buffer::DataModeDynamic);
    densityBuffer = new Engine::Graphics::Buffer::VertexBuffer<float>(stripedParticleCollection->DensityData(),
                                                                      stripedParticleCollection->GetSize(),
                                                                      Engine::Graphics::Buffer::Buffer::DataModeDynamic);
    pressureBuffer = new Engine::Graphics::Buffer::VertexBuffer<float>(stripedParticleCollection->PressureData(),
                                                                       stripedParticleCollection->GetSize(),
                                                                       Engine::Graphics::Buffer::Buffer::DataModeDynamic);

    typeBuffer = new Engine::Graphics::Buffer::VertexBuffer<uint8_t>(stripedParticleCollection->TypeData(),
                                                                     stripedParticleCollection->GetSize(),
                                                                     Engine::Graphics::Buffer::Buffer::DataModeDynamic);



    // create vertex array object
    this->vao = new Engine::Graphics::Buffer::VertexArray({

                                                                  Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                          GetIndexBuffer()),
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
                                                                          GetSelectionBuffer(),
                                                                          7, 1, 0, sizeof(int8_t),
                                                                          Engine::ComponentTypeByte
                                                                  )

                                                          });


}

ParticleVertexArrayForStripedParticleCollection::ParticleVertexArrayForStripedParticleCollection(
        FluidSolver::StripedParticleCollection *stripedParticleCollection) : ParticleVertexArray(
        stripedParticleCollection) {
    this->stripedParticleCollection = stripedParticleCollection;

    // call generate
    Generate();
}

ParticleVertexArrayForStripedParticleCollection::~ParticleVertexArrayForStripedParticleCollection() {
    delete positionBuffer;
    delete velocityBuffer;
    delete accelerationBuffer;
    delete massBuffer;
    delete pressureBuffer;
    delete densityBuffer;
    delete typeBuffer;
}

void ParticleVertexArrayForStripedParticleCollection::OnUpdate() {
    if (positionBuffer->GetElementCount() >= this->GetVaoParticleCount()) {
        // the size of the buffer is large enough for the particles: Update the data because it is faster
        positionBuffer->UpdateData(stripedParticleCollection->PositionData(), stripedParticleCollection->GetSize());
        velocityBuffer->UpdateData(stripedParticleCollection->VelocityData(), stripedParticleCollection->GetSize());
        accelerationBuffer->UpdateData(stripedParticleCollection->AccelerationData(),
                                       stripedParticleCollection->GetSize());

        massBuffer->UpdateData(stripedParticleCollection->MassData(), stripedParticleCollection->GetSize());
        densityBuffer->UpdateData(stripedParticleCollection->DensityData(), stripedParticleCollection->GetSize());
        pressureBuffer->UpdateData(stripedParticleCollection->PressureData(), stripedParticleCollection->GetSize());

        typeBuffer->UpdateData(stripedParticleCollection->TypeData(), stripedParticleCollection->GetSize());
    } else {
        // the buffer is too small, set the data to force a resize
        positionBuffer->SetData(stripedParticleCollection->PositionData(), stripedParticleCollection->GetSize());
        velocityBuffer->SetData(stripedParticleCollection->VelocityData(), stripedParticleCollection->GetSize());
        accelerationBuffer->SetData(stripedParticleCollection->AccelerationData(),
                                    stripedParticleCollection->GetSize());

        massBuffer->SetData(stripedParticleCollection->MassData(), stripedParticleCollection->GetSize());
        densityBuffer->SetData(stripedParticleCollection->DensityData(), stripedParticleCollection->GetSize());
        pressureBuffer->SetData(stripedParticleCollection->PressureData(), stripedParticleCollection->GetSize());

        typeBuffer->SetData(stripedParticleCollection->TypeData(), stripedParticleCollection->GetSize());
    }
}

ParticleVertexArrayForCompactParticleCollection::~ParticleVertexArrayForCompactParticleCollection() {
    delete vertexBuffer;
}

void ParticleVertexArrayForCompactParticleCollection::OnUpdate() {
    if (vertexBuffer->GetElementCount() >= this->GetVaoParticleCount()) {
        // the size of the buffer is large enough for the particles: Update the data because it is faster
        vertexBuffer->UpdateData(compactParticleCollection->GetData(), compactParticleCollection->GetSize());
    } else {
        // the buffer is too small, set the data to force a resize
        vertexBuffer->SetData(compactParticleCollection->GetData(), compactParticleCollection->GetSize());
    }
}

void ParticleVertexArrayForCompactParticleCollection::OnGenerate() {
    // create particle buffer
    vertexBuffer = new Engine::Graphics::Buffer::VertexBuffer<FluidSolver::FluidParticle>(
            compactParticleCollection->GetData(), compactParticleCollection->GetSize(),
            Engine::Graphics::Buffer::Buffer::DataModeDynamic);

    // create vertex array object
    vao = new Engine::Graphics::Buffer::VertexArray({

                                                            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                    GetIndexBuffer()),
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
                                                                    GetSelectionBuffer(),
                                                                    7, 1, 0, sizeof(int8_t),
                                                                    Engine::ComponentTypeByte
                                                            )

                                                    });

}

ParticleVertexArrayForCompactParticleCollection::ParticleVertexArrayForCompactParticleCollection(
        FluidSolver::CompactParticleCollection *compactParticleCollection) : ParticleVertexArray(
        compactParticleCollection) {
    this->compactParticleCollection = compactParticleCollection;
    Generate();
}
