#include "GLParticleVertexArray.hpp"

#include <stdexcept>

namespace FluidSolver
{

    ParticleVertexArray::ParticleVertexArray(FluidSolver::ParticleCollection* particleCollection)
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
     * (pIndex_t particleIndex = 0; particleIndex < size; particleIndex++) {
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
        FluidSolver::ParticleCollection* particleCollection)
    {
        return new ParticleVertexArrayForCollection(particleCollection);

        throw std::invalid_argument(
            "Could not find a type of particle vertex array matching the given type of the particle collection!");
    }

    void ParticleVertexArrayForCollection::OnGenerate()
    {
        FLUID_ASSERT(stripedParticleCollection->is_type_present<FluidSolver::MovementData>());
        FLUID_ASSERT(stripedParticleCollection->is_type_present<FluidSolver::ParticleData>());
        FLUID_ASSERT(stripedParticleCollection->is_type_present<FluidSolver::ParticleInfo>());

        // create particle buffers
        movementBuffer = new Engine::Graphics::Buffer::VertexBuffer<FluidSolver::MovementData>(
            &stripedParticleCollection->get<FluidSolver::MovementData>(0), stripedParticleCollection->size(),
            Engine::Graphics::Buffer::Buffer::DataModeDynamic);

        dataBuffer = new Engine::Graphics::Buffer::VertexBuffer<FluidSolver::ParticleData>(
            &stripedParticleCollection->get<FluidSolver::ParticleData>(0), stripedParticleCollection->size(),
            Engine::Graphics::Buffer::Buffer::DataModeDynamic);
        infoBuffer = new Engine::Graphics::Buffer::VertexBuffer<FluidSolver::ParticleInfo>(
            &stripedParticleCollection->get<FluidSolver::ParticleInfo>(0), stripedParticleCollection->size(),
            Engine::Graphics::Buffer::Buffer::DataModeDynamic);

        // create vertex array object
        this->vao = new Engine::Graphics::Buffer::VertexArray({

            Engine::Graphics::Buffer::VertexArray::BufferBinding(GetIndexBuffer()),
            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                movementBuffer, 0, 2, offsetof(FluidSolver::MovementData, position), sizeof(FluidSolver::MovementData),
                Engine::ComponentTypeFloat),
            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                movementBuffer, 1, 2, offsetof(FluidSolver::MovementData, velocity), sizeof(FluidSolver::MovementData),
                Engine::ComponentTypeFloat),
            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                movementBuffer, 2, 2, offsetof(FluidSolver::MovementData, acceleration),
                sizeof(FluidSolver::MovementData), Engine::ComponentTypeFloat),
            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                dataBuffer, 3, 1, offsetof(FluidSolver::ParticleData, mass), sizeof(FluidSolver::ParticleData),
                Engine::ComponentTypeFloat),
            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                dataBuffer, 4, 1, offsetof(FluidSolver::ParticleData, pressure), sizeof(FluidSolver::ParticleData),
                Engine::ComponentTypeFloat),
            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                dataBuffer, 5, 1, offsetof(FluidSolver::ParticleData, density), sizeof(FluidSolver::ParticleData),
                Engine::ComponentTypeFloat),
            Engine::Graphics::Buffer::VertexArray::BufferBinding(
                infoBuffer, 6, 1, offsetof(FluidSolver::ParticleInfo, type), sizeof(FluidSolver::ParticleInfo),
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
        FLUID_ASSERT(stripedParticleCollection->is_type_present<FluidSolver::MovementData>());
        FLUID_ASSERT(stripedParticleCollection->is_type_present<FluidSolver::ParticleData>());
        FLUID_ASSERT(stripedParticleCollection->is_type_present<FluidSolver::ParticleInfo>());

        if (movementBuffer->GetElementCount() >= this->GetVaoParticleCount())
        {
            // the size of the buffer is large enough for the particles: Update the data because it is faster
            movementBuffer->UpdateData(&stripedParticleCollection->get<FluidSolver::MovementData>(0),
                                       stripedParticleCollection->size());
            dataBuffer->UpdateData(&stripedParticleCollection->get<FluidSolver::ParticleData>(0),
                                   stripedParticleCollection->size());
            infoBuffer->UpdateData(&stripedParticleCollection->get<FluidSolver::ParticleInfo>(0),
                                   stripedParticleCollection->size());
        }
        else
        {
            // the buffer is too small, set the data to force a resize
            movementBuffer->SetData(&stripedParticleCollection->get<FluidSolver::MovementData>(0),
                                    stripedParticleCollection->size());
            dataBuffer->SetData(&stripedParticleCollection->get<FluidSolver::ParticleData>(0),
                                stripedParticleCollection->size());
            infoBuffer->SetData(&stripedParticleCollection->get<FluidSolver::ParticleInfo>(0),
                                stripedParticleCollection->size());
        }
    }

    ParticleVertexArrayForCollection::ParticleVertexArrayForCollection(FluidSolver::ParticleCollection* collection)
        : ParticleVertexArray(collection)
    {
        this->stripedParticleCollection = collection;
        Generate();
    }
} // namespace FluidSolver