#include "../../EngineException.hpp"
#include "Buffer.hpp"

using namespace Engine::Graphics::Buffer;

Buffer::Buffer(Buffer::BufferType type, size_t elementSize, DataMode dataMode, size_t elementCount) : type(type),
                                                                                                      elementSize(
                                                                                                              elementSize),
                                                                                                      dataMode(
                                                                                                              dataMode),
                                                                                                      elementCount(
                                                                                                              elementCount) {
    GenerateBuffer();
}

void Buffer::Bind() {
    glBindBuffer(type, ID);
}

void Buffer::Unbind() {
    glBindBuffer(type, 0);
}

Buffer::~Buffer() {
    DeleteBuffer();
}

void Buffer::DeleteBuffer() {
    Unbind();
    glDeleteBuffers(1, &ID);
    ID = 0;
}

void Buffer::GenerateBuffer() {
    glGenBuffers(1, &ID);
    Bind();

    sizeInBytes = elementCount * elementSize;

    glBufferData(type, sizeInBytes, nullptr, dataMode);
}

void Buffer::SetSize(size_t elementCount) {
    this->elementCount = elementCount;
    this->sizeInBytes = elementSize * elementCount;
    Bind();
    glBufferData(type, sizeInBytes, nullptr, dataMode);
}

void Buffer::SetData(void *data) {
    Bind();
    glBufferSubData(type, 0, sizeInBytes, data);
}

void Buffer::SetData(void *data, size_t offsetInBufferInBytes, size_t sizeInBytes) {

    if (offsetInBufferInBytes + sizeInBytes > this->sizeInBytes)
        throw EngineException("Buffer Exception: Trying to write outside the buffers memory!");

    Bind();
    glBufferSubData(type, offsetInBufferInBytes, sizeInBytes, data);
}

uint32_t Buffer::GetId() const {
    return ID;
}

Buffer::BufferType Buffer::GetType() const {
    return type;
}

Buffer::DataMode Buffer::GetDataMode() const {
    return dataMode;
}

size_t Buffer::GetElementSize() const {
    return elementSize;
}

size_t Buffer::GetElementCount() const {
    return elementCount;
}

size_t Buffer::GetSizeInBytes() const {
    return sizeInBytes;
}

IndexBufferDataType Buffer::GetIndexBufferDataType() {
    throw EngineException("Buffer Exception: Operation not valid for this buffer!");
}

void Buffer::BindBase(int32_t base) {
    glBindBufferBase(type, base, ID);
}


