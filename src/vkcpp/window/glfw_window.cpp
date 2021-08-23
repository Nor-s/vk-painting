#include "glfw_window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

namespace vkcpp
{
    GlfwWindow::GlfwWindow()
    {
        init_window(width_, height_, title_);
    }
    GlfwWindow::~GlfwWindow()
    {
        if (handle_)
        {
            destroy_window();
        }
    }
    VkSurfaceKHR GlfwWindow::create_surface(VkInstance &instance)
    {
        if (instance == VK_NULL_HANDLE || !handle_)
        {
            throw std::runtime_error("failed to access instance or GLFWwindow*!");
        }
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(instance, handle_, nullptr, &surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
        return surface;
    }
    bool GlfwWindow::should_close()
    {
        return glfwWindowShouldClose(handle_);
    }
    void GlfwWindow::close()
    {
        glfwSetWindowShouldClose(handle_, GLFW_TRUE);
    }
    void GlfwWindow::process_events()
    {
        glfwPollEvents();
    }

    void GlfwWindow::init_window(uint32_t width, uint32_t height, std::string &title)
    {
        set_window(width, height, title);
        if (!handle_)
        {
            init_glfw();
        }
        else
        {
        }
    }
    void GlfwWindow::init_glfw()
    {
        if (!glfwInit())
        {
            throw std::runtime_error("GLFW couldn't be initialized.");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        handle_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
        if (!handle_)
        {
            glfwTerminate();
            throw std::runtime_error("GLFW failed to create window");
        }
    }
    void GlfwWindow::destroy_window()
    {
        glfwDestroyWindow(handle_);
        handle_ = nullptr;
        glfwTerminate();
    }

    std::pair<const char **, uint32_t> GlfwWindow::get_required_instance_extensions() const
    {
        uint32_t glfw_extension_count;
        auto glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
        return std::make_pair(glfw_extensions, glfw_extension_count);
    }

    void GlfwWindow::set_window(uint32_t width, uint32_t height, const std::string &title)
    {
        GlfwWindow::set_size(width, height);
        GlfwWindow::set_title(title);
    }
    void GlfwWindow::set_size(uint32_t width, uint32_t height)
    {
        Window::set_size(width, height);
        glfwSetWindowSize(handle_, width_, height_);
    }
    void GlfwWindow::set_title(const std::string &title)
    {
        Window::set_title(title);
        glfwSetWindowTitle(handle_, title_.c_str());
    }

} // namespace vkcpp