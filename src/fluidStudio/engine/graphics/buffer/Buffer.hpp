#pragma once

#include <glad/glad.h>

#include <cstddef>
#include <stdint.h>

namespace Engine
{
    namespace Graphics
    {
        namespace Buffer
        {

            enum IndexBufferDataType
            {
                IndexBufferDataTypeByte = GL_UNSIGNED_BYTE,
                IndexBufferDataTypeShort = GL_UNSIGNED_SHORT,
                IndexBufferDataTypeInt = GL_UNSIGNED_INT,
            };

            class Buffer
            {
              public:
                enum BufferType
                {
                    BufferTypeVertex = GL_ARRAY_BUFFER,
                    BufferTypeIndex = GL_ELEMENT_ARRAY_BUFFER,
                    BufferTypeUniform = GL_UNIFORM_BUFFER
                };

                enum DataMode
                {
                    DataModeStatic = GL_STATIC_DRAW,
                    DataModeDynamic = GL_DYNAMIC_DRAW,
                    DataModeStream = GL_STREAM_DRAW
                };

                /**
                 *
                 * @param type
                 * @param elementSize Size of one
                 * element in bytes.
                 * @param dataMode
                 * @param elementCount
                 */
                Buffer(BufferType type, size_t elementSize, DataMode dataMode = DataModeStatic,
                       size_t elementCount = 0);

                virtual ~Buffer();

                void Bind();

                void BindBase(int32_t base);

                void Unbind();

                virtual IndexBufferDataType GetIndexBufferDataType();

              protected:
                void SetSize(size_t elementCount);

                void SetData(void *data);

                void SetData(void *data, size_t offsetInBufferInBytes, size_t sizeInBytes);

              private:
                uint32_t ID = 0;

                BufferType type;

                DataMode dataMode;

              public:
                uint32_t GetId() const;

                BufferType GetType() const;

                DataMode GetDataMode() const;

                size_t GetElementSize() const;

                size_t GetElementCount() const;

                size_t GetSizeInBytes() const;

              private:
                size_t elementSize;

                size_t elementCount = 0;

                size_t sizeInBytes = 0;

                void GenerateBuffer();

                void DeleteBuffer();
            };
        } // namespace Buffer
    }     // namespace Graphics
} // namespace Engine
