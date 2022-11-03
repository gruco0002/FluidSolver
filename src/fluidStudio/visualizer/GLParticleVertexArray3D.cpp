#include "GLParticleVertexArray3D.hpp"

#include <stdexcept>

namespace LibFluid {

    ParticleVertexArray3D::ParticleVertexArray3D(std::shared_ptr<LibFluid::ParticleCollection> particleCollection) {
        this->particleCollection = particleCollection;
    }

    void ParticleVertexArray3D::Update() {
        // update the internal particle count
        this->vaoParticleCount = particleCollection->size();


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

    void ParticleVertexArray3D::Draw() {
        vao->Draw(GL_POINTS, vaoParticleCount);
    }

    void ParticleVertexArray3D::Generate() {
        // update the particle count variable
        this->vaoParticleCount = particleCollection->size();

        // create indices
        std::vector<uint32_t> indices(this->vaoParticleCount);
        for (uint32_t i = 0; i < this->vaoParticleCount; i++) {
            indices[i] = i;
        }
        indexBuffer = new Engine::Graphics::Buffer::IndexBuffer<uint32_t>(indices);

        this->OnGenerate();
    }


    ParticleVertexArray3D::~ParticleVertexArray3D() {
        delete vao;

        delete indexBuffer;
    }


    Engine::Graphics::Buffer::IndexBuffer<uint32_t>* ParticleVertexArray3D::GetIndexBuffer() {
        return indexBuffer;
    }

    uint32_t ParticleVertexArray3D::GetVaoParticleCount() {
        return vaoParticleCount;
    }

    ParticleVertexArray3D* ParticleVertexArray3D::CreateFromParticleCollection(
            std::shared_ptr<LibFluid::ParticleCollection> particleCollection) {
        return new ParticleVertexArray3DForCollection(particleCollection);
    }

    void ParticleVertexArray3DForCollection::OnGenerate() {
        FLUID_ASSERT(stripedParticleCollection->is_type_present<LibFluid::MovementData3D>());
        FLUID_ASSERT(stripedParticleCollection->is_type_present<LibFluid::ParticleInfo>());

        // create particle buffers
        movementBuffer = new Engine::Graphics::Buffer::VertexBuffer<LibFluid::MovementData3D>(
                &stripedParticleCollection->get<LibFluid::MovementData3D>(0), stripedParticleCollection->size(),
                Engine::Graphics::Buffer::Buffer::DataModeDynamic);

        infoBuffer = new Engine::Graphics::Buffer::VertexBuffer<LibFluid::ParticleInfo>(
                &stripedParticleCollection->get<LibFluid::ParticleInfo>(0), stripedParticleCollection->size(),
                Engine::Graphics::Buffer::Buffer::DataModeDynamic);

        // create vertex array object
        this->vao = new Engine::Graphics::Buffer::VertexArray({

                Engine::Graphics::Buffer::VertexArray::BufferBinding(GetIndexBuffer()),
                Engine::Graphics::Buffer::VertexArray::BufferBinding(
                        movementBuffer, 1, 3, offsetof(LibFluid::MovementData3D, position),
                        sizeof(LibFluid::MovementData3D), Engine::ComponentTypeFloat),
                Engine::Graphics::Buffer::VertexArray::BufferBinding(
                        movementBuffer, 2, 3, offsetof(LibFluid::MovementData3D, velocity),
                        sizeof(LibFluid::MovementData3D), Engine::ComponentTypeFloat),
                Engine::Graphics::Buffer::VertexArray::BufferBinding(
                        movementBuffer, 3, 3, offsetof(LibFluid::MovementData3D, acceleration),
                        sizeof(LibFluid::MovementData3D), Engine::ComponentTypeFloat),
                Engine::Graphics::Buffer::VertexArray::BufferBinding(
                        infoBuffer, 0, 1, offsetof(LibFluid::ParticleInfo, type), sizeof(LibFluid::ParticleInfo),
                        Engine::ComponentTypeUnsignedByte),
                Engine::Graphics::Buffer::VertexArray::BufferBinding(
                        infoBuffer, 4, 1, offsetof(LibFluid::ParticleInfo, tag), sizeof(LibFluid::ParticleInfo),
                        Engine::ComponentTypeUInt)});
    }


    ParticleVertexArray3DForCollection::~ParticleVertexArray3DForCollection() {
        delete movementBuffer;
        delete infoBuffer;
    }

    void ParticleVertexArray3DForCollection::OnUpdate() {
        FLUID_ASSERT(stripedParticleCollection->is_type_present<LibFluid::MovementData3D>());
        FLUID_ASSERT(stripedParticleCollection->is_type_present<LibFluid::ParticleInfo>());

        if (movementBuffer->GetElementCount() >= this->GetVaoParticleCount()) {
            // the size of the buffer is large enough for the particles: Update the data because it is faster
            movementBuffer->UpdateData(&stripedParticleCollection->get<LibFluid::MovementData3D>(0),
                    stripedParticleCollection->size());
            infoBuffer->UpdateData(&stripedParticleCollection->get<LibFluid::ParticleInfo>(0),
                    stripedParticleCollection->size());
        } else {
            // the buffer is too small, set the data to force a resize
            movementBuffer->SetData(&stripedParticleCollection->get<LibFluid::MovementData3D>(0),
                    stripedParticleCollection->size());
            infoBuffer->SetData(&stripedParticleCollection->get<LibFluid::ParticleInfo>(0),
                    stripedParticleCollection->size());
        }
    }

    ParticleVertexArray3DForCollection::ParticleVertexArray3DForCollection(std::shared_ptr<LibFluid::ParticleCollection> collection)
        : ParticleVertexArray3D(collection) {
        this->stripedParticleCollection = collection;
        Generate();
    }
} // namespace LibFluid