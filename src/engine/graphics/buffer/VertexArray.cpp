//
// Created by corbi on 17.04.2019.
//

#include "../../EngineException.hpp"
#include "VertexArray.hpp"

Engine::Graphics::Buffer::VertexArray::~VertexArray() {
    Delete();
}

Engine::Graphics::Buffer::VertexArray::VertexArray(
        std::vector<Engine::Graphics::Buffer::VertexArray::BufferBinding> bufferBindings) : bufferBindings(
        bufferBindings) {
    Generate();
}

void Engine::Graphics::Buffer::VertexArray::Bind() {
    glBindVertexArray(ID);
}

void Engine::Graphics::Buffer::VertexArray::Unbind() {
    glBindVertexArray(0);
}

void Engine::Graphics::Buffer::VertexArray::Generate() {
    glGenVertexArrays(1, &ID);
    Bind();

    // add the index buffer and bind it
    auto indexBufferBinding = GetIndexBufferBinding();
    indexBufferBinding.buffer->Bind();
    indexBuffer = indexBufferBinding.buffer;

    // add all other bindings.
    for (BufferBinding &binding : bufferBindings) {
        if (binding.buffer->GetType() != Buffer::BufferTypeVertex)
            continue;

        // check if attribute index is free
        if (boundLocations.find(binding.attributeIndex) != boundLocations.end())
            throw EngineException("Vertex Array Error: The attribute index is already in use by another binding!");

        // bind to attribute index
        boundLocations.insert(binding.attributeIndex);
        binding.buffer->Bind();
        glEnableVertexAttribArray(binding.attributeIndex);

        // check component type and bind data
        auto cp = binding.componentType;
        if (cp == ComponentTypeFloat || cp == ComponentTypeHalfFloat) {
            glVertexAttribPointer(binding.attributeIndex, binding.numberOfComponentsPerAttributeElement,
                                  binding.componentType, GL_FALSE, binding.byteOffsetBetweenAttributeElement,
                                  (void *) binding.byteOffsetToFirstAttributeElement);
        } else if (cp == ComponentTypeUInt || cp == ComponentTypeByte || cp == ComponentTypeShort ||
                   cp == ComponentTypeUnsignedByte) {
            glVertexAttribIPointer(binding.attributeIndex, binding.numberOfComponentsPerAttributeElement,
                                   binding.componentType, binding.byteOffsetBetweenAttributeElement,
                                   (void *) binding.byteOffsetToFirstAttributeElement);
        } else {
            throw EngineException("Vertex Array Error: unsupported component type!");
        }

        if (binding.isInstanced) {
            // enable instancing
            // TODO: check if this is working as expected
            glVertexAttribDivisor(binding.attributeIndex, 1);
        }

    }

    Unbind();

}

void Engine::Graphics::Buffer::VertexArray::Delete() {
    Unbind();
    glDeleteVertexArrays(1, &ID);
}

Engine::Graphics::Buffer::VertexArray::BufferBinding Engine::Graphics::Buffer::VertexArray::GetIndexBufferBinding() {

    bool found = false;
    BufferBinding ret;
    for (BufferBinding &tmp: bufferBindings) {
        if (tmp.buffer->GetType() == Buffer::BufferTypeIndex) {
            if (found) {
                throw EngineException("Vertex Array Error: There is more than one index buffer!");
            }
            ret = tmp;
            found = true;
        }
    }
    if (!found)
        throw EngineException("Vertex Array Error: There is no index buffer!");

    return ret;
}

Engine::Graphics::Buffer::Buffer *Engine::Graphics::Buffer::VertexArray::GetIndexBuffer() {
    return indexBuffer;
}

void Engine::Graphics::Buffer::VertexArray::Draw(GLenum primitiveType, uint32_t count) {
    Bind();
    glDrawElements(primitiveType, count, indexBuffer->GetIndexBufferDataType(), nullptr);
}

Engine::Graphics::Buffer::VertexArray::BufferBinding::BufferBinding(Engine::Graphics::Buffer::Buffer *buffer,
                                                                    uint8_t attributeIndex,
                                                                    uint8_t numberOfComponentsPerAttributeElement,
                                                                    size_t byteOffsetToFirstAttributeElement,
                                                                    size_t byteOffsetBetweenAttributeElement,
                                                                    Engine::ComponentType componentType,
                                                                    bool isInstanced) {
    this->buffer = buffer;
    this->attributeIndex = attributeIndex;
    this->numberOfComponentsPerAttributeElement = numberOfComponentsPerAttributeElement;
    this->byteOffsetBetweenAttributeElement = byteOffsetBetweenAttributeElement;
    this->byteOffsetToFirstAttributeElement = byteOffsetToFirstAttributeElement;
    this->componentType = componentType;
    this->isInstanced = isInstanced;

}

Engine::Graphics::Buffer::VertexArray::BufferBinding::BufferBinding() {

}
