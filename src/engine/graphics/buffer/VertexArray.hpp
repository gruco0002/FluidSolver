//
// Created by corbi on 17.04.2019.
//

#ifndef CPPGUI_DEMO_VERTEXARRAY_HPP
#define CPPGUI_DEMO_VERTEXARRAY_HPP

#include <vector>
#include <set>
#include "../Enums.hpp"
#include "Buffer.hpp"
#include "IndexBuffer.hpp"

namespace Engine {
    namespace Graphics {
        namespace Buffer {
            class VertexArray {

            public:


                /**
                 * Data for a buffer binding. There can be bindings to multiple buffers and / or multiple bindings to one buffer in one vertex array object.
                 * @note For an index buffer there is no need to specify any field besides the Buffer* buffer field.
                 * @warning Each attribute index can be only used once per vertex array.
                 * @note The buffer consists of an array of elements. In an vertex array, you gain the possibility to subdivide
                 *       the whole array at nearly arbitary positions. This partitioning results in attribute elements, which are
                 *       divided further into components. E.g. an attribute element represents a 4D-Vector, so it consists of
                 *       four components.
                 */
                struct BufferBinding {
                    Buffer *buffer = nullptr;
                    uint8_t attributeIndex = 0; // aka attribute location
                    uint8_t numberOfComponentsPerAttributeElement = 1; //aka size
                    size_t byteOffsetToFirstAttributeElement = 0; // aka pointer
                    size_t byteOffsetBetweenAttributeElement = 4; // aka stride
                    ComponentType componentType = ComponentTypeFloat; // aka type
                    bool isInstanced = false;

                    BufferBinding(Buffer *buffer, uint8_t attributeIndex = 0,
                                  uint8_t numberOfComponentsPerAttributeElement = 1,
                                  size_t byteOffsetToFirstAttributeElement = 0,
                                  size_t byteOffsetBetweenAttributeElement = 4,
                                  ComponentType componentType = ComponentTypeFloat, bool isInstanced = false);

                    BufferBinding();
                };


            public:
                VertexArray(std::vector<BufferBinding> bufferBindings);

                void Bind();

                void Unbind();

                virtual ~VertexArray();

                Buffer *GetIndexBuffer();

                /**
                 * Draws the vao using the index buffer.
                 * @param primitiveType
                 * @param count Number of indices that should be used for drawing
                 */
                virtual void Draw(GLenum primitiveType, uint32_t count);

            private:

                uint32_t ID;

                void Generate();

                void Delete();

                BufferBinding GetIndexBufferBinding();

                std::vector<BufferBinding> bufferBindings;

                std::set<uint8_t> boundLocations;

                Buffer *indexBuffer;

            };

        }
    }
}

#endif //CPPGUI_DEMO_VERTEXARRAY_HPP
