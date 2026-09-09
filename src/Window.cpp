#include <Window.hpp>

std::unique_ptr<Window> Window::create(int width, int height, const std::string& name)
{
    GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (window == nullptr)
    {
        return nullptr;
    }
    return std::make_unique<Window>(window);
}

std::unique_ptr<Window> Window::createVirtual(const std::string& name)
{
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(1, 1, name.c_str(), nullptr, nullptr);
    if (window == nullptr)
    {
        return nullptr;
    }
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
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

void Window::hide()
{
    glfwHideWindow(window_);
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