#ifndef FLUIDSOLVER_PARTICLEVERTEXARRAY_HPP
#define FLUIDSOLVER_PARTICLEVERTEXARRAY_HPP


#include "FluidAssert.hpp"
#include "fluidSolver/ParticleCollection.hpp"

#include <engine/graphics/buffer/IndexBuffer.hpp>
#include <engine/graphics/buffer/VertexArray.hpp>
#include <engine/graphics/buffer/VertexBuffer.hpp>
#include <memory>

namespace FluidSolver
{

    /**
     * The particle vertex array creates a vertex array object (vao) of point sprites for rendering particles with
     * opengl.
     *
     * This abstract class delivers a base construct for deriving special classes that can deal with
     * different data layouts
     * of the particle collection for more performance.
     *
     * The derived class has to call
     * the Generate() function in its constructor!
     *
     * ParticleVertexArray::OnGenerate is called once to allow the
     * derived class to generate its own buffers and to generate
     * the vao with its buffer bindings. The index and
     * selection buffer is created beforehand and can be used and bound to
     * the vao.
     *
     * ParticleVertexArray::OnUpdate is called every time the particle collection was updated. After updating the index
     * and
     * selection buffer as well as the vao particle count field, OnUpdate is called such that the derived
     * classes can update
     * their buffers. It is recommended to check the new vao particle count against the current
     * buffers size to ensure safe
     * operation.
     *
     * The vao particle count can change depending on the particle
     * collection with every update statement. However it must
     * not represent the buffer size (in terms of element
     * count) it can be very usual, that for a short period of time the
     * amount of particles becomes smaller but
     * returns to its original size after that. Therefore reducing buffer sizes
     * immediately is not recommended since
     * an enlargement later on can cause huge memory traffic and performance loss.
     * The abstract class ensures that
     * this does not happen by not reducing the size of the index buffer and selection
     * buffer. Hence the count
     * variable is used for drawing only the amount of particles that are currently existing and
     * not the whole
     * buffer and for checks regarding the update of the buffer.
     *
     * The abstract class deletes its own resources
     * like the vao, the index buffer and the selection buffer. All other
     * resources created by derived classes must
     * be taken care of by the derived class.
     *
     * For easier creation of the correct particle vertex array for a
     * given particle collection a static factory function
     * is provided.
     *
     * @note The attribute layout for the
     * vao has to be in the following order:
     * @verbatim
     * Layout-Location  type    name
     * 0                vec2
     * position
     * 1                vec2    velocity
     * 2                vec2    acceleration
     * 3 float
     * mass
     * 4                float   pressure
     * 5                float   density
     * 6                uint8
     * type
     * @endverbatim
     *
     *
     */
    class ParticleVertexArray {

      public:
        /**
         * Creates the correct instance of a particle vertex array for the given type of particle collection.

         * * @param particleCollection Particle Collection
         * @return Correct type of particle vertex array based on
         * the type of the particle collection.
         */
        static ParticleVertexArray* CreateFromParticleCollection(
            std::shared_ptr<FluidSolver::ParticleCollection> particleCollection);

        /**
         * Should be called if the particle collection, its contained particle data or the selection data
         * changed.
         * The function updates all buffers (and if needed their sizes) to represent the new state.
         *
         * @param particleSelection The current particle selection.
         */
        void Update(void* particleSelection);

        /**
         * Draws the vao as point sprites.
         */
        void Draw();

        virtual ~ParticleVertexArray();

        /**
         * Returns the vao particle count.
         * @return Vao particle count.
         * @note The particle count
         * does not reflect the real buffer sizes (in terms of elements) of the index or selection
         * buffer.

         */
        uint32_t GetVaoParticleCount();

      protected:
        void Generate();

        /**
         * This function is called during update after the index and selection buffer and the vao particle count
         * field are
         * updated.
         */
        virtual void OnUpdate() = 0;

        /**
         * This function is called once during generation after the index and selection buffer are generated.

         * * The function has to create the vao according to its specification and should create any other buffers
         * needed.
         */
        virtual void OnGenerate() = 0;

        /**
         * The default constructor. Should be called by derived classes.
         * @param particleCollection The
         * particle collection that the vao represents.
         */
        explicit ParticleVertexArray(std::shared_ptr<FluidSolver::ParticleCollection> particleCollection);

        /**
         * Returns the buffer object containing the indices for the point primitives that will be drawed.
         *
         * @return Index buffer
         */
        Engine::Graphics::Buffer::IndexBuffer<uint32_t>* GetIndexBuffer();

        /**
         * Returns the buffer object containing the selection information for each particle.
         * @return
         * Selection buffer
         */
        Engine::Graphics::Buffer::VertexBuffer<int8_t>* GetSelectionBuffer();


        /**
         * The underlying opengl vao object. This object should be created by the derived class in the
         * OnGenerate function.
         * The object is deleted by the base class and must not be deleted by a derived
         * class.
         */
        Engine::Graphics::Buffer::VertexArray* vao = nullptr;

      private:
        uint32_t vaoParticleCount = 0;
        std::shared_ptr<FluidSolver::ParticleCollection> particleCollection = nullptr;

        std::vector<int8_t> selectionData;

        // void UpdateSelectionData(FluidSolver::IParticleSelection *particleSelection);

        Engine::Graphics::Buffer::IndexBuffer<uint32_t>* indexBuffer = nullptr;
        Engine::Graphics::Buffer::VertexBuffer<int8_t>* selectionBuffer = nullptr;
    };

    /**
     * A particle vertex array for the striped particle collection.
     */
    class ParticleVertexArrayForCollection : public ParticleVertexArray {

      public:
        explicit ParticleVertexArrayForCollection(std::shared_ptr<FluidSolver::ParticleCollection> collection);


        ~ParticleVertexArrayForCollection() override;

      protected:
        void OnGenerate() override;

        void OnUpdate() override;

      private:
        std::shared_ptr<FluidSolver::ParticleCollection> stripedParticleCollection = nullptr;

        Engine::Graphics::Buffer::VertexBuffer<FluidSolver::MovementData>* movementBuffer = nullptr;
        Engine::Graphics::Buffer::VertexBuffer<FluidSolver::ParticleData>* dataBuffer = nullptr;
        Engine::Graphics::Buffer::VertexBuffer<FluidSolver::ParticleInfo>* infoBuffer = nullptr;
    };
} // namespace FluidSolver

#endif // FLUIDSOLVER_PARTICLEVERTEXARRAY_HPP
