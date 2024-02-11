#include "Shader.hpp"

#include "glm/gtc/type_ptr.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>

uint32_t Engine::Graphics::Shader::currentlyBound = 0;

Engine::Graphics::Shader::Shader(std::vector<Engine::Graphics::Shader::ProgramPart> parts,
                                 std::vector<std::string> defines)
    : parts(parts), defines(defines)
{
    Generate();
}

Engine::Graphics::Shader::Shader(std::vector<Engine::Graphics::Shader::ProgramPart> parts) : parts(parts)
{
    Generate();
}

Engine::Graphics::Shader::~Shader()
{
    Delete();
}

void Engine::Graphics::Shader::Bind()
{
    if (currentlyBound == ID)
        return;
    glUseProgram(ID);
    currentlyBound = ID;
}

void Engine::Graphics::Shader::Unbind()
{
    glUseProgram(0);
    currentlyBound = 0;
}

void Engine::Graphics::Shader::SetValue(const std::string &name, bool value)
{
    Bind();
    glUniform1i(GetUniformLoaction(name), (int32_t)value);
}

void Engine::Graphics::Shader::SetValue(const std::string &name, int32_t value)
{
    Bind();
    glUniform1i(GetUniformLoaction(name), value);
}

void Engine::Graphics::Shader::SetValue(const std::string &name, float_t value)
{
    Bind();
    glUniform1f(GetUniformLoaction(name), value);
}

void Engine::Graphics::Shader::SetValue(const std::string &name, glm::vec2 value)
{
    Bind();
    glUniform2f(GetUniformLoaction(name), value.x, value.y);
}

void Engine::Graphics::Shader::SetValue(const std::string &name, glm::vec3 value)
{
    Bind();
    glUniform3f(GetUniformLoaction(name), value.x, value.y, value.z);
}

void Engine::Graphics::Shader::SetValue(const std::string &name, glm::vec4 value)
{
    Bind();
    glUniform4f(GetUniformLoaction(name), value.x, value.y, value.z, value.w);
}

void Engine::Graphics::Shader::SetValue(const std::string &name, glm::mat3 value)
{
    Bind();
    glUniformMatrix3fv(GetUniformLoaction(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Engine::Graphics::Shader::SetValue(const std::string &name, glm::mat4 value)
{
    Bind();
    glUniformMatrix4fv(GetUniformLoaction(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Engine::Graphics::Shader::SetValue(const std::string &name, Engine::Graphics::Texture2D *texture, uint32_t unit)
{
    Bind();
    texture->Bind(unit);
    glUniform1i(GetUniformLoaction(name), unit - GL_TEXTURE0);
}

GLint Engine::Graphics::Shader::GetUniformLoaction(const std::string &name)
{
    if (foundNames.find(name) != foundNames.end())
        return uniformLoactions[name];
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1)
        throw std::invalid_argument("Shader Error: Cannot retrieve uniform variable with this name!");

    foundNames.insert(name);
    uniformLoactions[name] = location;
    return location;
}

void Engine::Graphics::Shader::Generate()
{
    // set defines
    InsertDefinesIntoCode();

    // compile shader parts
    std::vector<uint32_t> compiledShaders;
    for (ProgramPart &part : parts)
    {
        compiledShaders.push_back(CompileShader(part));
    }

    // link shader parts
    ID = glCreateProgram();
    for (uint32_t compiledShader : compiledShaders)
    {
        glAttachShader(ID, compiledShader);
    }
    glLinkProgram(ID);

    // check for linking errors
    GLint isCompiled = 0;
    glGetProgramiv(ID, GL_LINK_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> errorLog(maxLength);
        glGetProgramInfoLog(ID, maxLength, &maxLength, &errorLog[0]);
        throw std::invalid_argument(
            ("Shader Error: Error linking the shader program parts together! " + std::string(errorLog.data())).c_str());
    }

    // delete program parts
    for (uint32_t compiledShader : compiledShaders)
    {
        glDetachShader(ID, compiledShader);
        glDeleteShader(compiledShader);
    }
}

void Engine::Graphics::Shader::Delete()
{
    Unbind();
    glDeleteProgram(ID);
}

uint32_t Engine::Graphics::Shader::CompileShader(Engine::Graphics::Shader::ProgramPart &part)
{
    uint32_t shaderID = glCreateShader(part.type);
    auto source = part.code.c_str();
    glShaderSource(shaderID, 1, &source, nullptr);
    glCompileShader(shaderID);

    // check for compiling errors
    GLint isCompiled = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shaderID, maxLength, &maxLength, &errorLog[0]);

        std::string program_type("Undefined");

        if (part.type == ProgramPartType::ProgramPartTypeVertex)
        {
            program_type = "Vertex Shader";
        }
        else if (part.type == ProgramPartType::ProgramPartTypeGeometry)
        {
            program_type = "Geometry Shader";
        }
        else if (part.type == ProgramPartType::ProgramPartTypeFragment)
        {
            program_type = "Fragment Shader";
        }

        throw std::invalid_argument(
            ("Shader Error: Error compiling the " + program_type + " program part! " + std::string(errorLog.data()))
                .c_str());
    }

    return shaderID;
}

void Engine::Graphics::Shader::InsertDefinesIntoCode()
{
    std::reverse(defines.begin(),
                 defines.end()); // reverse the defines, since they are added in reverse order => End result is adding
                                 // in correct order

    for (ProgramPart &part : parts)
    {
        // find first newline
        auto firstNewLine = part.code.find('\n');

        // insert defines after that line
        for (std::string &def : defines)
        {
            part.code.insert(firstNewLine + 1, "#define " + def + "\n");
        }
    }
}

Engine::Graphics::Shader::Shader(std::vector<Engine::Graphics::Shader::ProgramPartFile> parts)
{
    LoadShaderFilesAsShaderParts(parts);
    Generate();
}

Engine::Graphics::Shader::Shader(std::vector<Engine::Graphics::Shader::ProgramPartFile> parts,
                                 std::vector<std::string> defines)
    : defines(defines)
{
    LoadShaderFilesAsShaderParts(parts);
    Generate();
}

std::string Engine::Graphics::Shader::LoadCodeFromFile(std::string filepath)
{
    std::string code;
    std::ifstream codeFile;

    // ensure ifstream objects can throw exceptions:
    codeFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        codeFile.open(filepath);

        std::stringstream shaderStream;
        // read file's buffer contents into streams
        shaderStream << codeFile.rdbuf();

        // close file handlers
        codeFile.close();
        // convert stream into string
        code = shaderStream.str();
    }
    catch (std::ifstream::failure &e)
    {
        throw std::invalid_argument(("Shader Error: Error reading files! " + std::string(e.what())).c_str());
    }
    return code;
}

void Engine::Graphics::Shader::LoadShaderFilesAsShaderParts(
    std::vector<Engine::Graphics::Shader::ProgramPartFile> fileParts)
{
    for (ProgramPartFile &part : fileParts)
    {
        auto code = LoadCodeFromFile(part.filepath);
        ProgramPart p;
        p.code = code;
        p.type = part.type;
        parts.push_back(p);
    }
}

void Engine::Graphics::Shader::SetValue(const std::string &name, Engine::Graphics::Buffer::Buffer *uniformBuffer,
                                        int32_t bindingPoint)
{
    if (uniformBuffer->GetType() != Buffer::Buffer::BufferTypeUniform)
        throw std::invalid_argument("Shader Error: Only a uniform buffer can be bound!");
    Bind();
    uniformBuffer->Bind();
    glUniformBlockBinding(ID, GetUniformBufferLocation(name), bindingPoint);
    uniformBuffer->BindBase(bindingPoint);
}

GLint Engine::Graphics::Shader::GetUniformBufferLocation(const std::string &name)
{
    if (foundNames.find(name) != foundNames.end())
        return uniformLoactions[name];
    GLint location = glGetUniformBlockIndex(ID, name.c_str());
    if (location == -1)
        throw std::invalid_argument("Shader Error: Cannot retrieve uniform buffer object with this name!");

    foundNames.insert(name);
    uniformLoactions[name] = location;
    return location;
}

void Engine::Graphics::Shader::SetValue(const std::string &name, Engine::Graphics::Texture2DArray *texture,
                                        uint32_t unit)
{
    Bind();
    texture->Bind(unit);
    glUniform1i(GetUniformLoaction(name), unit - GL_TEXTURE0);
}

Engine::Graphics::Shader::ProgramPart::ProgramPart()
{
}

Engine::Graphics::Shader::ProgramPart::ProgramPart(Engine::Graphics::Shader::ProgramPartType type, std::string code)
{
    this->type = type;
    this->code = code;
}
