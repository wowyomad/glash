#include "window.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>
#include <memory>

namespace glash
{

    Window::Window(int width, int height, const std::string &title)
    {
        std::cout << "Window constructor called" << std::endl;
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        m_pWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!m_pWindow)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(m_pWindow);

        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
            throw std::runtime_error("Failed to initialize GLEW");
        }

        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
    }

    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(m_pWindow);
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(m_pWindow);
    }

} // namespace glash
