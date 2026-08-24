#include <shader.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

Shader::Shader(const std::string& vertPath, const std::string& fragPath) 
{
    const std::string vertSrc = readFile(vertPath);
    const std::string fragSrc = readFile(fragPath);

    const GLuint vertCompiled = compile(GL_VERTEX_SHADER, vertSrc);
    const GLuint fragCompiled = compile(GL_FRAGMENT_SHADER, fragSrc);

    program_ = glCreateProgram();
    glAttachShader(program_, vertCompiled);
    glAttachShader(program_, fragCompiled);
    glLinkProgram(program_);

    checkLinkErrors(program_);

    glDeleteShader(vertCompiled);
    glDeleteShader(fragCompiled);
}

Shader::~Shader()
{
    glDeleteProgram(program_);
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

void Shader::setFloat(const std::string& name, float value)
{
    const GLint location = glGetUniformLocation(program_, name.c_str());
    glUniform1f(location, value);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
    const GLint location = glGetUniformLocation(program_, name.c_str());
    glUniform4f(location, x, y, z, w);
}