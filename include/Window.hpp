#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include <memory>

class Window
{
public:
    static bool init();
    static void terminate();
    static std::unique_ptr<Window> create(int width, int height, const std::string& name);
    int width();
    int height();
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