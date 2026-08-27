#include <shader.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

Shader::Shader(const GLuint program)
    : program_(program) {}

Shader::Shader(Shader&& other) noexcept
    : program_(other.program_)
{
    other.program_ = 0;
}

Shader::~Shader()
{
    glDeleteProgram(program_);
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other)
    {
        glDeleteProgram(program_);

        program_ = other.program_;
        other.program_ = 0;
    }

    return *this;
}

std::optional<Shader> Shader::tryLoad(const std::string& vertPath, const std::string& fragPath) 
{
    const std::string vertSrc = readFile(vertPath);
    const std::string fragSrc = readFile(fragPath);

    const GLuint vertCompiled = compile(GL_VERTEX_SHADER, vertSrc);
    const GLuint fragCompiled = compile(GL_FRAGMENT_SHADER, fragSrc);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertCompiled);
    glAttachShader(program, fragCompiled);
    glLinkProgram(program);
    
    glDeleteShader(vertCompiled);
    glDeleteShader(fragCompiled);

    checkLinkErrors(program);
    return Shader(program);
}

void Shader::use() const
{
    glUseProgram(program_);
}

GLuint Shader::id() const
{
    return program_;
}

std::string Shader::readFile(const std::string& path)
{
    std::ifstream file(path);

    if (!file)
    {
        throw std::runtime_error("Failed to open shader file " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint Shader::compile(GLenum type, const std::string& src)
{
    GLuint shader = glCreateShader(type);
    const char* cstr = src.c_str();
    glShaderSource(shader, 1, &cstr, nullptr);
    glCompileShader(shader);
    checkCompileErrors(shader, type == GL_VERTEX_SHADER
            ? "VERTEX"
            : "FRAGMENT");
    return shader;
}

void Shader::checkCompileErrors(GLuint shader, const std::string& type)
{
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        throw std::runtime_error(type + " shader compilation failed:\n" + std::string(infoLog));
    }
}

void Shader::checkLinkErrors(GLuint program)
{
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        throw std::runtime_error("Shader program linking failed:\n" + std::string(infoLog));
    }
}

void Shader::setInt(const std::string& name, int value) const
{
    const GLint location = glGetUniformLocation(program_, name.c_str());
    glUniform1i(location, value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    const GLint location = glGetUniformLocation(program_, name.c_str());
    glUniform1f(location, value);
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
    const GLint location = glGetUniformLocation(program_, name.c_str());
    glUniform2f(location, x, y);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    const GLint location = glGetUniformLocation(program_, name.c_str());
    glUniform3f(location, x, y, z);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
    const GLint location = glGetUniformLocation(program_, name.c_str());
    glUniform4f(location, x, y, z, w);
}

void Shader::setVec2(const std::string& name, Vec2f v) const
{
    const GLint location = glGetUniformLocation(program_, name.c_str());
    glUniform2f(location, v.x, v.y);
}

void Shader::setVec3(const std::string& name, Vec3f v) const
{
    const GLint location = glGetUniformLocation(program_, name.c_str());
    glUniform3f(location, v.x, v.y, v.z);
}

void Shader::setVec4(const std::string& name, Vec4f v) const
{
    const GLint location = glGetUniformLocation(program_, name.c_str());
    glUniform4f(location, v.x, v.y, v.z, v.w);
}