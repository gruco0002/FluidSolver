#pragma once

#include "core/FluidAssert.hpp"
#include "core/fluidSolver/ParticleCollection.hpp"
#include "engine/graphics/buffer/IndexBuffer.hpp"
#include "engine/graphics/buffer/VertexArray.hpp"
#include "engine/graphics/buffer/VertexBuffer.hpp"

namespace FluidSolver
{

    /**
	 * 
	 * @note The attribute layout for the vao has to be in the following order:
	 * @verbatim
	 *
     *
     * Layout-Location  type    name
	 * 0                uint8   type
	 * 1                vec3    position	 
	 * 2
     * vec3    velocity
	 * 3                vec3    acceleration
	 * 
	 * @endverbatim
	 **/

    class ParticleVertexArray3D {

      public:
        static ParticleVertexArray3D* CreateFromParticleCollection(FluidSolver::ParticleCollection* particleCollection);


        void Update();

        /**
		 * Draws the vao as point sprites.
		 */
        void Draw();

        virtual ~ParticleVertexArray3D();

        /**
		 * Returns the vao particle count.
		 * @return Vao particle count.
		 * @note The particle count

         * * does not reflect the real buffer sizes (in terms of elements) of the index or selection
		 * buffer.


         */
        uint32_t GetVaoParticleCount();

      protected:
        void Generate();

        /**
		 * This function is called during update after the index and selection buffer and the vao particle
         * count
         * field are
		 * updated.
		 */
        virtual void OnUpdate() = 0;

        /**
		 * This function is called once during generation after the index and selection buffer are generated.


         * * * The function has to create the vao according to its specification and should create any other buffers

         * * needed.
		 */
        virtual void OnGenerate() = 0;

        /**
		 * The default constructor. Should be called by derived classes.
		 * @param particleCollection The

         * * particle collection that the vao represents.
		 */
        explicit ParticleVertexArray3D(FluidSolver::ParticleCollection* particleCollection);

        /**
		 * Returns the buffer object containing the indices for the point primitives that will be drawed.

         * *
         * @return Index buffer
		 */
        Engine::Graphics::Buffer::IndexBuffer<uint32_t>* GetIndexBuffer();

       


        /**
		 * The underlying opengl vao object. This object should be created by the derived class in the

         * * OnGenerate function.
		 * The object is deleted by the base class and must not be deleted by a derived

         * * class.
		 */
        Engine::Graphics::Buffer::VertexArray* vao = nullptr;

      private:
        uint32_t vaoParticleCount = 0;
        FluidSolver::ParticleCollection* particleCollection = nullptr;

     

        Engine::Graphics::Buffer::IndexBuffer<uint32_t>* indexBuffer = nullptr;

    };

    /**
	 * A particle vertex array for the striped particle collection.
	 */
    class ParticleVertexArray3DForCollection : public ParticleVertexArray3D {

      public:
        explicit ParticleVertexArray3DForCollection(FluidSolver::ParticleCollection* collection);


        ~ParticleVertexArray3DForCollection() override;

      protected:
        void OnGenerate() override;

        void OnUpdate() override;

      private:
        FluidSolver::ParticleCollection* stripedParticleCollection = nullptr;

        Engine::Graphics::Buffer::VertexBuffer<FluidSolver::MovementData3D>* movementBuffer = nullptr;
        Engine::Graphics::Buffer::VertexBuffer<FluidSolver::ParticleInfo>* infoBuffer = nullptr;
    };
} // namespace FluidSolver

