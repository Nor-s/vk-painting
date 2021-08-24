#ifndef VKCPP_DEVICE_DEVICE_H
#define VKCPP_DEVICE_DEVICE_H

#include "device/physical_device.h"

namespace vkcpp
{
    class Device
    {
        // Device(PhysicalDevice &gpu, VkSurfaceKHR surface, std::unordered_map<const char *, bool> requested_extensions = {});
    };
} // namespace vkcpp
#endif
/*
#endif //#ifndef VKCPP_DEVICE_DEVICE_H
#include "surface.h"

#include "instance.h"
#include "window/main_window.h"

#include <iostream>

namespace vkcpp
{
    Surface::Surface(const Instance *instance)
    {
        instance_ = instance;
    }
    Surface::~Surface()
    {
        destroy_surface();
    }
    void Surface::init_surface(const Instance *instance)
    {
        instance_ = instance;
        if (handle_ == VK_NULL_HANDLE)
        {
            handle_ = MainWindow::getInstance()->create_surface(*instance_);
        }
    }
    void Surface::destroy_surface()
    {
        if (handle_ != VK_NULL_HANDLE)
        {
            if (instance_ == nullptr)
            {
                throw std::runtime_error("faild to destroy surface : instance_ is nullptr");
            }
            vkDestroySurfaceKHR(*instance_, handle_, nullptr);
            handle_ = VK_NULL_HANDLE;
        }
    }

} // namespace vkcp
*/