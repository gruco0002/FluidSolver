#ifndef ENGINE_UNIFORMBUFFER_HPP
#define ENGINE_UNIFORMBUFFER_HPP

#include <vector>
#include "Buffer.hpp"

namespace Engine {
    namespace Graphics {
        namespace Buffer {

            template<typename T>
            class UniformBuffer : public Buffer {

            public:
                UniformBuffer(std::vector<T> &data, DataMode dataMode = DataModeStream);

                UniformBuffer(T *data, size_t elementCount, DataMode dataMode = DataModeStream);

                UniformBuffer(size_t elementCount, DataMode dataMode = DataModeStream);

                void UpdateData(std::vector<T> &data);

                void UpdateData(T *data);

                void SetData(std::vector<T> &data);

                void SetData(T *data, size_t elementCount);

            };

            template<typename T>
            UniformBuffer<T>::UniformBuffer(std::vector<T> &data, Buffer::DataMode dataMode) : Buffer::Buffer(
                    BufferTypeUniform, sizeof(T), dataMode, data.size()) {
                Buffer::SetData((void *) data.data());
            }

            template<typename T>
            UniformBuffer<T>::UniformBuffer(T *data, size_t elementCount, Buffer::DataMode dataMode) : Buffer::Buffer(
                    BufferTypeUniform, sizeof(T), dataMode, elementCount) {
                Buffer::SetData((void *) data);
            }

            template<typename T>
            UniformBuffer<T>::UniformBuffer(size_t elementCount, Buffer::DataMode dataMode) : Buffer::Buffer(
                    BufferTypeUniform,
                    sizeof(T), dataMode, elementCount) {

            }

            template<typename T>
            void UniformBuffer<T>::UpdateData(std::vector<T> &data) {
                if (this->GetElementCount() < data.size())
                    throw EngineException("Buffer Exception: Input size is too large correct");
                Buffer::SetData((void *) data.data(), 0, data.size() * GetElementSize());
            }

            template<typename T>
            void UniformBuffer<T>::UpdateData(T *data) {
                Buffer::SetData((void *) data);
            }

            template<typename T>
            void UniformBuffer<T>::SetData(std::vector<T> &data) {
                if (this->GetElementCount() != data.size())
                    this->SetSize(data.size());
                UpdateData(data);
            }

            template<typename T>
            void UniformBuffer<T>::SetData(T *data, size_t elementCount) {
                if (this->GetElementCount() != elementCount)
                    this->SetSize(elementCount);
                UpdateData(data);
            }
        }
    }
}


#endif //ENGINE_UNIFORMBUFFER_HPP
