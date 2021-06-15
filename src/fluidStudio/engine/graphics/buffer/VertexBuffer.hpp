#ifndef ENGINE_VERTEXBUFFER_HPP
#define ENGINE_VERTEXBUFFER_HPP

#include "Buffer.hpp"

#include <vector>


namespace Engine
{
    namespace Graphics
    {
        namespace Buffer
        {

            template <typename T> class VertexBuffer : public Buffer {

              public:
                VertexBuffer(std::vector<T>& data, DataMode dataMode = DataModeStatic);

                VertexBuffer(T* data, size_t elementCount, DataMode dataMode = DataModeStatic);

                VertexBuffer(size_t elementCount, DataMode dataMode = DataModeStatic);

                void UpdateData(std::vector<T>& data);

                void UpdateData(T* data);

                void UpdateData(T* data, size_t elementCount);

                void SetData(std::vector<T>& data);

                void SetData(T* data, size_t elementCount);
            };

            template <typename T>
            VertexBuffer<T>::VertexBuffer(std::vector<T>& data, Buffer::DataMode dataMode)
                : Buffer::Buffer(BufferTypeVertex, sizeof(T), dataMode, data.size())
            {
                Buffer::SetData((void*)data.data());
            }

            template <typename T>
            VertexBuffer<T>::VertexBuffer(T* data, size_t elementCount, Buffer::DataMode dataMode)
                : Buffer::Buffer(BufferTypeVertex, sizeof(T), dataMode, elementCount)
            {
                Buffer::SetData((void*)data);
            }

            template <typename T>
            VertexBuffer<T>::VertexBuffer(size_t elementCount, Buffer::DataMode dataMode)
                : Buffer::Buffer(BufferTypeVertex, sizeof(T), dataMode, elementCount)
            {
            }

            template <typename T> void VertexBuffer<T>::UpdateData(std::vector<T>& data)
            {
                if (this->GetElementCount() < data.size())
                    throw EngineException("Buffer Exception: Input size is too large correct");
                Buffer::SetData((void*)data.data(), 0, data.size() * GetElementSize());
            }

            template <typename T> void VertexBuffer<T>::UpdateData(T* data)
            {
                Buffer::SetData((void*)data);
            }

            template <typename T> void VertexBuffer<T>::SetData(std::vector<T>& data)
            {
                if (this->GetElementCount() != data.size())
                    this->SetSize(data.size());
                UpdateData(data);
            }

            template <typename T> void VertexBuffer<T>::SetData(T* data, size_t elementCount)
            {
                if (this->GetElementCount() != elementCount)
                    this->SetSize(elementCount);
                UpdateData(data);
            }

            template <typename T> void VertexBuffer<T>::UpdateData(T* data, size_t elementCount)
            {
                if (this->GetElementCount() < elementCount)
                    throw EngineException("Buffer Exception: Input size is too large correct");
                Buffer::SetData((void*)data, 0, elementCount * GetElementSize());
            }

        } // namespace Buffer
    }     // namespace Graphics
} // namespace Engine

#endif // ENGINE_VERTEXBUFFER_HPP
