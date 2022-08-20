#pragma once

#include "Buffer.hpp"

#include <limits>
#include <vector>
#include <stdexcept>


namespace Engine {
    namespace Graphics {
        namespace Buffer {


            /**
             * An index buffer for usage in an vertex array object.
             * @tparam T Must be
             * uint8_t or uint16_t or uint32_t.
             */
            template<typename T>
            class IndexBuffer : public Buffer {
              public:
                IndexBuffer(std::vector<T>& data, DataMode dataMode = DataModeStatic);

                IndexBuffer(T* data, size_t elementCount, DataMode dataMode = DataModeStatic);

                IndexBuffer(size_t elementCount, DataMode dataMode = DataModeStatic);

                void UpdateData(std::vector<T>& data);

                void UpdateData(T* data);

                void SetData(std::vector<T>& data);

                void SetData(T* data, size_t elementCount);

                IndexBufferDataType GetIndexBufferDataType() override;

              private:
                IndexBufferDataType indexBufferDataType;

                void CheckAndSetType();
            };

            template<typename T>
            IndexBuffer<T>::IndexBuffer(std::vector<T>& data, Buffer::DataMode dataMode)
                : Buffer::Buffer(BufferTypeIndex, sizeof(T), dataMode, data.size()) {
                CheckAndSetType();
                Buffer::SetData((void*)data.data());
            }

            template<typename T>
            IndexBuffer<T>::IndexBuffer(T* data, size_t elementCount, Buffer::DataMode dataMode)
                : Buffer::Buffer(BufferTypeIndex, sizeof(T), dataMode, elementCount) {
                CheckAndSetType();
                Buffer::SetData((void*)data);
            }

            template<typename T>
            IndexBuffer<T>::IndexBuffer(size_t elementCount, Buffer::DataMode dataMode)
                : Buffer::Buffer(BufferTypeIndex, sizeof(T), dataMode, elementCount) {
                CheckAndSetType();
            }


            template<typename T>
            void IndexBuffer<T>::UpdateData(std::vector<T>& data) {
                if (this->GetElementCount() != data.size())
                    throw std::invalid_argument("Buffer Exception: Input size is not correct");
                Buffer::SetData((void*)data.data());
            }

            template<typename T>
            void IndexBuffer<T>::UpdateData(T* data) {
                Buffer::SetData((void*)data);
            }

            template<typename T>
            void IndexBuffer<T>::SetData(std::vector<T>& data) {
                if (this->GetElementCount() != data.size())
                    this->SetSize(data.size());
                UpdateData(data);
            }

            template<typename T>
            void IndexBuffer<T>::SetData(T* data, size_t elementCount) {
                if (this->GetElementCount() != elementCount)
                    this->SetSize(elementCount);
                UpdateData(data);
            }

            template<typename T>
            IndexBufferDataType IndexBuffer<T>::GetIndexBufferDataType() {
                return indexBufferDataType;
            }

            template<typename T>
            void IndexBuffer<T>::CheckAndSetType() {
                if (!std::numeric_limits<T>::is_integer)
                    throw std::invalid_argument("Index Buffer: Unsupported data type!");

                if (std::numeric_limits<T>::is_signed)
                    throw std::invalid_argument("Index Buffer: Unsupported data type!");


                if (sizeof(T) == sizeof(uint32_t)) {
                    indexBufferDataType = IndexBufferDataTypeInt;
                } else if (sizeof(T) == sizeof(uint16_t)) {
                    indexBufferDataType = IndexBufferDataTypeShort;
                } else if (sizeof(T) == sizeof(uint8_t)) {
                    indexBufferDataType = IndexBufferDataTypeByte;
                } else {
                    throw std::invalid_argument("Index Buffer: Unsupported data type!");
                }
            }

        } // namespace Buffer
    }     // namespace Graphics
} // namespace Engine
