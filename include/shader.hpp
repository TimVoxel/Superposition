#pragma once

#include <glad/gl.h>
#include <string>

class Shader
{
public:
    Shader(const std::string& vertPath, const std::string& fragPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use() const;
    GLuint id() const;
    void setFloat(const std::string& name, float value);
    void setVec4(const std::string& name, float x, float y, float z, float w);
private:
    GLuint program_;

    static std::string readFile(const std::string& path);
    static GLuint compile(GLenum type, const std::string& src);
    static void checkCompileErrors(GLuint shader, const std::string& type);
    static void checkLinkErrors(GLuint program);
};