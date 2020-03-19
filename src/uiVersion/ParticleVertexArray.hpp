#ifndef FLUIDSOLVER_PARTICLEVERTEXARRAY_HPP
#define FLUIDSOLVER_PARTICLEVERTEXARRAY_HPP


#include <core/fluidSolver/particleCollection/StripedParticleCollection.hpp>
#include <engine/graphics/buffer/VertexBuffer.hpp>
#include <engine/graphics/buffer/IndexBuffer.hpp>
#include <engine/graphics/buffer/VertexArray.hpp>
#include <core/selection/IParticleSelection.hpp>

/**
 * The particle vertex array creates a vertex array object (vao) of point sprites for rendering particles with opengl.
 *
 * This abstract class delivers a base construct for deriving special classes that can deal with different data layouts
 * of the particle collection for more performance.
 *
 * ParticleVertexArray::OnGenerate is called once to allow the derived class to generate its own buffers and to generate
 * the vao with its buffer bindings. The index and selection buffer is created beforehand and can be used and bound to
 * the vao.
 * ParticleVertexArray::OnUpdate is called every time the particle collection was updated. After updating the index and
 * selection buffer as well as the vao particle count field, OnUpdate is called such that the derived classes can update
 * their buffers. It is recommended to check the new vao particle count against the current buffers size to ensure safe
 * operation.
 *
 * The vao particle count can change depending on the particle collection with every update statement. However it must
 * not represent the buffer size (in terms of element count) it can be very usual, that for a short period of time the
 * amount of particles becomes smaller but returns to its original size after that. Therefore reducing buffer sizes
 * immediately is not recommended since an enlargement later on can cause huge memory traffic and performance loss.
 * The abstract class ensures that this does not happen by not reducing the size of the index buffer and selection
 * buffer. Hence the count variable is used for drawing only the amount of particles that are currently existing and
 * not the whole buffer and for checks regarding the update of the buffer.
 *
 * The abstract class deletes its own resources like the vao, the index buffer and the selection buffer. All other
 * resources created by derived classes must be taken care of by the derived class.
 *
 * For easier creation of the correct particle vertex array for a given particle collection a static factory function
 * is provided.
 *
 * @note The attribute layout for the vao has to be in the following order:
 * @verbatim
 * Layout-Location  type    name
 * 0                vec2    position
 * 1                vec2    velocity
 * 2                vec2    acceleration
 * 3                float   mass
 * 4                float   pressure
 * 5                float   density
 * 6                uint8   type
 * @endverbatim
 *
 *
 */
class ParticleVertexArray {

public:

    /**
     * Creates the correct instance of a particle vertex array for the given type of particle collection.
     * @param particleCollection Particle Collection
     * @return Correct type of particle vertex array based on the type of the particle collection.
     */
    static ParticleVertexArray *CreateFromParticleCollection(FluidSolver::IParticleCollection *particleCollection);

    /**
     * Should be called if the particle collection, its contained particle data or the selection data changed.
     * The function updates all buffers (and if needed their sizes) to represent the new state.
     * @param particleSelection The current particle selection.
     */
    void Update(FluidSolver::IParticleSelection *particleSelection);

    /**
     * Draws the vao as point sprites.
     */
    void Draw();

    virtual ~ParticleVertexArray();

protected:
    void Generate();

    virtual void OnUpdate() = 0;

    virtual void OnGenerate() = 0;

    explicit ParticleVertexArray(FluidSolver::IParticleCollection *particleCollection);

    Engine::Graphics::Buffer::IndexBuffer<uint32_t> *GetIndexBuffer();

    Engine::Graphics::Buffer::VertexBuffer<int8_t> *GetSelectionBuffer();

    uint32_t GetVaoParticleCount();

    Engine::Graphics::Buffer::VertexArray *vao = nullptr;

private:
    uint32_t vaoParticleCount = 0;
    FluidSolver::IParticleCollection *simpleParticleCollection = nullptr;

    std::vector<int8_t> selectionData;

    void UpdateSelectionData(FluidSolver::IParticleSelection *particleSelection);

    Engine::Graphics::Buffer::IndexBuffer<uint32_t> *indexBuffer = nullptr;
    Engine::Graphics::Buffer::VertexBuffer<int8_t> *selectionBuffer = nullptr;

};


class ParticleVertexArrayForCompactParticleCollection {


};

class ParticleVertexArrayForStripedParticleCollection : public ParticleVertexArray {

public:
    explicit ParticleVertexArrayForStripedParticleCollection(
            FluidSolver::StripedParticleCollection *stripedParticleCollection);

    ~ParticleVertexArrayForStripedParticleCollection() override;

protected:

    void OnGenerate() override;

    void OnUpdate() override;

private:

    FluidSolver::StripedParticleCollection *stripedParticleCollection = nullptr;

    Engine::Graphics::Buffer::VertexBuffer<glm::vec2> *positionBuffer = nullptr;
    Engine::Graphics::Buffer::VertexBuffer<glm::vec2> *velocityBuffer = nullptr;
    Engine::Graphics::Buffer::VertexBuffer<glm::vec2> *accelerationBuffer = nullptr;
    Engine::Graphics::Buffer::VertexBuffer<float> *massBuffer = nullptr;
    Engine::Graphics::Buffer::VertexBuffer<float> *pressureBuffer = nullptr;
    Engine::Graphics::Buffer::VertexBuffer<float> *densityBuffer = nullptr;
    Engine::Graphics::Buffer::VertexBuffer<uint8_t> *typeBuffer = nullptr;
};


#endif //FLUIDSOLVER_PARTICLEVERTEXARRAY_HPP
