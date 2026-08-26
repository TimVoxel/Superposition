#include <Window.hpp>

bool Window::init()
{
     if (!glfwInit())
    {
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    return true;
}

void Window::terminate()
{
    glfwTerminate();
}

std::unique_ptr<Window> Window::create(int width, int height, const std::string& name)
{
    GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (window == nullptr)
    {
        return nullptr;
    }
    return std::make_unique<Window>(window);
}

int Window::width()
{
    return width_;
}

int Window::height()
{
    return height_;
}

void Window::update() 
{
    glfwPollEvents();
    glfwGetFramebufferSize(window_, &width_, &height_);
}

void Window::swapBuffers()
{
    glfwSwapBuffers(window_);
}

void Window::destroy()
{
    glfwDestroyWindow(window_);
}

int Window::shouldClose()
{
    return glfwWindowShouldClose(window_);
}

void Window::makeContextCurrent()
{
    glfwMakeContextCurrent(window_);
}

Window::~Window()
{
    glfwDestroyWindow(window_);
}

Window::Window(GLFWwindow* window) : window_(window)
{
    glfwGetFramebufferSize(window_, &width_, &height_);
}