#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include <memory>

class Window
{
public:
    static std::unique_ptr<Window> create(int width, int height, const std::string& name);
    static std::unique_ptr<Window> createVirtual(const std::string& name);
    int width();
    int height();
    void hide();
    void update();
    void swapBuffers();
    void destroy();
    int shouldClose();
    void makeContextCurrent();
    ~Window();
    Window(GLFWwindow* window);
private:
    GLFWwindow* window_;
    int width_;
    int height_;
};