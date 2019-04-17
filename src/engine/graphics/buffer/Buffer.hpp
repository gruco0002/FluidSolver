//
// Created by corbi on 17.04.2019.
//

#ifndef FLUIDSOLVER_BUFFER_HPP
#define FLUIDSOLVER_BUFFER_HPP


#include <glad/glad.h>
#include <cstdint>

namespace Engine {
    namespace Graphics {
        namespace Buffer {


            class Buffer {

            public:
                enum BufferType {
                    BufferTypeVertex = GL_ARRAY_BUFFER,
                    BufferTypeIndex = GL_ELEMENT_ARRAY_BUFFER,
                };

                enum DataMode {
                    DataModeStatic = GL_STATIC_DRAW,
                    DataModeDynamic = GL_DYNAMIC_DRAW,
                };

                /**
                 *
                 * @param type
                 * @param elementSize Size of one element in bytes.
                 * @param dataMode
                 * @param elementCount
                 */
                Buffer(BufferType type, size_t elementSize, DataMode dataMode = DataModeStatic,
                       size_t elementCount = 0);

                virtual ~Buffer();

                void Bind();

                void Unbind();

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
        }
    }
}

#endif //FLUIDSOLVER_BUFFER_HPP
