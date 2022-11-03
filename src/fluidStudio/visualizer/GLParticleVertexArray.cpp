#include "GLParticleVertexArray.hpp"

#include <stdexcept>

namespace LibFluid {

    ParticleVertexArray::ParticleVertexArray(std::shared_ptr<LibFluid::ParticleCollection> particleCollection)
    {
        this->particleCollection = particleCollection;
    }

    void ParticleVertexArray::Update(void* particleSelection)
    {
        // update the internal particle count
        this->vaoParticleCount = particleCollection->size();

        /*
        // first update the selection data, then update the buffer

         * UpdateSelectionData(particleSelection);
        if (selectionBuffer->GetElementCount() >=
         * this->selectionData.size()) {
            // the size of the buffer is large enough for the particles: Update
         * the data because it is faster
            selectionBuffer->UpdateData(selectionData);
        } else {
 //
         * the buffer is too small, set the data to force a resize
            selectionBuffer->SetData(selectionData);

         * }
         */

        // resize index buffer iff needed
        if (indexBuffer->GetElementCount() < this->vaoParticleCount)
        {
            std::vector<uint32_t> indices(this->vaoParticleCount);
            for (uint32_t i = 0; i < this->vaoParticleCount; i++)
            {
                indices[i] = i;
            }
            indexBuffer->SetData(indices);
        }

        // update other buffers or data
        OnUpdate();
    }

    void ParticleVertexArray::Draw()
    {
        vao->Draw(GL_POINTS, vaoParticleCount);
    }

    void ParticleVertexArray::Generate()
    {
        // update the particle count variable
        this->vaoParticleCount = particleCollection->size();

        // create indices
        std::vector<uint32_t> indices(this->vaoParticleCount);
        for (uint32_t i = 0; i < this->vaoParticleCount; i++)
        {
            indices[i] = i;
        }
        indexBuffer = new Engine::Graphics::Buffer::IndexBuffer<uint32_t>(indices);

        // create selection data buffer
        /*selectionData.resize(this->vaoParticleCount);
        selectionBuffer = new
         * Engine::Graphics::Buffer::VertexBuffer<int8_t>(selectionData,

         * Engine::Graphics::Buffer::Buffer::DataModeDynamic);*/

        this->OnGenerate();
    }


    ParticleVertexArray::~ParticleVertexArray()
    {
        delete vao;

        delete indexBuffer;
        delete selectionBuffer;
    }

    /*void ParticleVertexArray::UpdateSelectionData(FluidSolver::IParticleSelection *particleSelection) {
        size_t
     * size = particleCollection->size();
        selectionData.resize(size);

    #pragma omp parallel for
        for
     * (size_t particleIndex = 0; particleIndex < size; particleIndex++) {
            bool selected =
     * particleSelection->IsParticleSelected(particleIndex, particleCollection);
 selectionData[particleIndex] =
     * selected ? 1 : 0;
        }
    }*/

    Engine::Graphics::Buffer::IndexBuffer<uint32_t>* ParticleVertexArray::GetIndexBuffer()
    {
        return indexBuffer;
    }

    Engine::Graphics::Buffer::VertexBuffer<int8_t>* ParticleVertexArray::GetSelectionBuffer()
    {
        return selectionBuffer;
    }

    uint32_t ParticleVertexArray::GetVaoParticleCount()
    {
        return vaoParticleCount;
    }

    ParticleVertexArray* ParticleVertexArray::CreateFromParticleCollection(
        std::shared_ptr<LibFluid::ParticleCollection> particleCollection)
    {
        return new ParticleVertexArrayForCollection(particleCollection);

        throw std::invalid_argument(
            "Could not find a type of particle vertex array matching the given type of the particle collection!");
    }

    void ParticleVertexArrayForCollection::OnGenerate()
    {
        FLUID_ASSERT(stripedParticleCollection->is_type_present<LibFluid::MovementData>());
        FLUID_ASSERT(stripedParticleCollection->is_type_present<LibFluid::ParticleData>());
        FLUID_ASSERT(stripedParticleCollection->is_type_present<LibFluid::ParticleInfo>());

        // create particle buffers
        movementBuffer = new Engine::Graphics::Buffer::VertexBuffer<LibFluid::MovementData>(
            &stripedParticleCollection->get<LibFluid::MovementData>(0), stripedParticleCollection->size(),
            Engine::Graphics::Buffer::Buffer::DataModeDynamic);

        dataBuffer = new Engine::Graphics::Buffer::VertexBuffer<LibFluid::ParticleData>(
            &stripedParticleCollection->get<LibFluid::ParticleData>(0), stripedParticleCollection->size(),
            Engine::Graphics::Buffer::Buffer::DataModeDynamic);
        infoBuffer = new Engine::Graphics::Buffer::VertexBuffer<LibFluid::ParticleInfo>(
            &stripedParticleCollection->get<LibFluid::ParticleInfo>(0), stripedParticleCollection->size(),
            Engine::Graphics::Buffer::Buffer::DataModeDynamic);

        // create vertex array object
        this->vao = new Engine::Graphics::Buffer::VertexArray({

            Engine::Graphics::Buffer::VertexArray::BufferBinding(GetIndexBuffer()),
            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                movementBuffer, 0, 2, offsetof(LibFluid::MovementData, position), sizeof(LibFluid::MovementData),
                Engine::ComponentTypeFloat),
            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                movementBuffer, 1, 2, offsetof(LibFluid::MovementData, velocity), sizeof(LibFluid::MovementData),
                Engine::ComponentTypeFloat),
            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                movementBuffer, 2, 2, offsetof(LibFluid::MovementData, acceleration),
                sizeof(LibFluid::MovementData), Engine::ComponentTypeFloat),
            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                dataBuffer, 3, 1, offsetof(LibFluid::ParticleData, mass), sizeof(LibFluid::ParticleData),
                Engine::ComponentTypeFloat),
            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                dataBuffer, 4, 1, offsetof(LibFluid::ParticleData, pressure), sizeof(LibFluid::ParticleData),
                Engine::ComponentTypeFloat),
            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                dataBuffer, 5, 1, offsetof(LibFluid::ParticleData, density), sizeof(LibFluid::ParticleData),
                Engine::ComponentTypeFloat),
            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                infoBuffer, 6, 1, offsetof(LibFluid::ParticleInfo, type), sizeof(LibFluid::ParticleInfo),
                Engine::ComponentTypeUnsignedByte) //,
            /*Engine::Graphics::Buffer::VertexArray::BufferBinding(
 GetSelectionBuffer(),
 7, 1, 0,
               sizeof(int8_t),
 Engine::ComponentTypeByte
 )*/

        });
    }


    ParticleVertexArrayForCollection::~ParticleVertexArrayForCollection()
    {
        delete movementBuffer;
        delete dataBuffer;
        delete infoBuffer;
    }

    void ParticleVertexArrayForCollection::OnUpdate()
    {
        FLUID_ASSERT(stripedParticleCollection->is_type_present<LibFluid::MovementData>());
        FLUID_ASSERT(stripedParticleCollection->is_type_present<LibFluid::ParticleData>());
        FLUID_ASSERT(stripedParticleCollection->is_type_present<LibFluid::ParticleInfo>());

        if (movementBuffer->GetElementCount() >= this->GetVaoParticleCount())
        {
            // the size of the buffer is large enough for the particles: Update the data because it is faster
            movementBuffer->UpdateData(&stripedParticleCollection->get<LibFluid::MovementData>(0),
                                       stripedParticleCollection->size());
            dataBuffer->UpdateData(&stripedParticleCollection->get<LibFluid::ParticleData>(0),
                                   stripedParticleCollection->size());
            infoBuffer->UpdateData(&stripedParticleCollection->get<LibFluid::ParticleInfo>(0),
                                   stripedParticleCollection->size());
        }
        else
        {
            // the buffer is too small, set the data to force a resize
            movementBuffer->SetData(&stripedParticleCollection->get<LibFluid::MovementData>(0),
                                    stripedParticleCollection->size());
            dataBuffer->SetData(&stripedParticleCollection->get<LibFluid::ParticleData>(0),
                                stripedParticleCollection->size());
            infoBuffer->SetData(&stripedParticleCollection->get<LibFluid::ParticleInfo>(0),
                                stripedParticleCollection->size());
        }
    }

    ParticleVertexArrayForCollection::ParticleVertexArrayForCollection(std::shared_ptr<LibFluid::ParticleCollection> collection)
        : ParticleVertexArray(collection)
    {
        this->stripedParticleCollection = collection;
        Generate();
    }
} // namespace FluidSolver