//
// Created by corbi on 17.04.2019.
//

#ifndef CPPGUI_DEMO_SHADER_HPP
#define CPPGUI_DEMO_SHADER_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <libraries/glm/glm.hpp>
#include <engine/graphics/buffer/Buffer.hpp>
#include "Texture2D.hpp"
#include "Texture2DArray.hpp"

namespace Engine {
    namespace Graphics {


        class Shader {

        public:

            enum ProgramPartType {
                ProgramPartTypeVertex = GL_VERTEX_SHADER,
                ProgramPartTypeFragment = GL_FRAGMENT_SHADER,
                ProgramPartTypeGeometry = GL_GEOMETRY_SHADER,
            };

            struct ProgramPart {
                std::string code;
                ProgramPartType type;

                ProgramPart();
                ProgramPart(ProgramPartType type, std::string code);

            };

            struct ProgramPartFile {
                std::string filepath;
                ProgramPartType type;
            };

            Shader(std::vector<ProgramPartFile> parts);

            Shader(std::vector<ProgramPartFile> parts, std::vector<std::string> defines);

            Shader(std::vector<ProgramPart> parts, std::vector<std::string> defines);

            Shader(std::vector<ProgramPart> parts);

            ~Shader();

            void Bind();

            void Unbind();

            void SetValue(const std::string &name, bool value);

            void SetValue(const std::string &name, int32_t value);

            void SetValue(const std::string &name, float_t value);

            void SetValue(const std::string &name, glm::vec2 value);

            void SetValue(const std::string &name, glm::vec3 value);

            void SetValue(const std::string &name, glm::vec4 value);

            void SetValue(const std::string &name, glm::mat3 value);

            void SetValue(const std::string &name, glm::mat4 value);

            void SetValue(const std::string &name, Buffer::Buffer* uniformBuffer, int32_t bindingPoint);

            /**
             * Binds the texture to the texture unit and the uniform also.
             * @param name The name of the uniform variable
             * @param texture The texture that should be bound.
             * @param unit The texture unit the texture should be bound to.
             * @note The texture unit should be between GL_TEXTURE0-GL_TEXTURE_MAX
             */
            void SetValue(const std::string &name, Texture2D *texture, uint32_t unit);

            /**
 * Binds the texture to the texture unit and the uniform also.
 * @param name The name of the uniform variable
 * @param texture The texture that should be bound.
 * @param unit The texture unit the texture should be bound to.
 * @note The texture unit should be between GL_TEXTURE0-GL_TEXTURE_MAX
 */
            void SetValue(const std::string &name, Texture2DArray *texture, uint32_t unit);

        private:

            uint32_t ID;

            static uint32_t currentlyBound;

            std::vector<ProgramPart> parts;
            std::vector<std::string> defines;
            std::unordered_map<std::string, GLint> uniformLoactions;
            std::unordered_set<std::string> foundNames;

            GLint GetUniformLoaction(const std::string &name);

            GLint GetUniformBufferLocation(const std::string &name);

            void Generate();

            void Delete();

            uint32_t CompileShader(ProgramPart &part);

            void InsertDefinesIntoCode();

            std::string LoadCodeFromFile(std::string filepath);

            void LoadShaderFilesAsShaderParts(std::vector<ProgramPartFile> fileParts);

        };

    }
}


#endif //CPPGUI_DEMO_SHADER_HPP
