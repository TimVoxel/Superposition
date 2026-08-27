#pragma once

#include <glad/gl.h>
#include <string>
#include <optional>
#include <Math.hpp>

class Shader
{
public:
    Shader(const GLuint program);
    static std::optional<Shader> tryLoad(const std::string& vertPath, const std::string& fragPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void use() const;
    GLuint id() const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
    void setVec2(const std::string& name, Vec2f v) const;
    void setVec3(const std::string& name, Vec3f v) const;
    void setVec4(const std::string& name, Vec4f v) const;

private:
    GLuint program_;

    static std::string readFile(const std::string& path);
    static GLuint compile(GLenum type, const std::string& src);
    static void checkCompileErrors(GLuint shader, const std::string& type);
    static void checkLinkErrors(GLuint program);
};