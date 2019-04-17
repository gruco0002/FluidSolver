//
// Created by corbi on 17.04.2019.
//

#ifndef FLUIDSOLVER_INDEXBUFFER_HPP
#define FLUIDSOLVER_INDEXBUFFER_HPP

#include "Buffer.hpp"
#include <vector>

namespace Engine {
    namespace Graphics {
        namespace Buffer {


            template<typename T>
            class IndexBuffer : public Buffer {

            public:
                IndexBuffer(std::vector<T> &data, DataMode dataMode = DataModeStatic);

                IndexBuffer(T *data, size_t elementCount, DataMode dataMode = DataModeStatic);

                IndexBuffer(size_t elementCount, DataMode dataMode = DataModeStatic);

                void UpdateData(std::vector<T> &data);

                void UpdateData(T *data);

                void SetData(std::vector<T> &data);

                void SetData(T *data, size_t elementCount);

            };

            template<typename T>
            IndexBuffer<T>::IndexBuffer(std::vector<T> &data, Buffer::DataMode dataMode) : Buffer::Buffer(
                    BufferTypeIndex, sizeof(T), dataMode, data.size()) {
                Buffer::SetData((void *) data.data());
            }

            template<typename T>
            IndexBuffer<T>::IndexBuffer(T *data, size_t elementCount, Buffer::DataMode dataMode) : Buffer::Buffer(
                    BufferTypeIndex, sizeof(T), dataMode, elementCount) {
                Buffer::SetData((void *) data);
            }

            template<typename T>
            IndexBuffer<T>::IndexBuffer(size_t elementCount, Buffer::DataMode dataMode) : Buffer::Buffer(
                    BufferTypeIndex,
                    sizeof(T), dataMode, elementCount) {

            }

            template<typename T>
            void IndexBuffer<T>::UpdateData(std::vector<T> &data) {
                if (this->GetElementCount() != data.size())
                    throw EngineException("Buffer Exception: Input size is not correct");
                Buffer::SetData((void *) data.data());
            }

            template<typename T>
            void IndexBuffer<T>::UpdateData(T *data) {
                Buffer::SetData((void *) data);
            }

            template<typename T>
            void IndexBuffer<T>::SetData(std::vector<T> &data) {
                if (this->GetElementCount() != data.size())
                    this->SetSize(data.size());
                UpdateData(data);
            }

            template<typename T>
            void IndexBuffer<T>::SetData(T *data, size_t elementCount) {
                if (this->GetElementCount() != elementCount)
                    this->SetSize(elementCount);
                UpdateData(data);
            }

        }
    }
}

#endif //FLUIDSOLVER_INDEXBUFFER_HPP
