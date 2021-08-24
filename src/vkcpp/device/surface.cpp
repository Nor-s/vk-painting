#include "surface.h"

#include "instance.h"

#include <iostream>

namespace vkcpp
{
    Surface::Surface(const Instance *instance, VkSurfaceKHR surface)
        : instance_(instance), handle_(surface)
    {
    }
    Surface::~Surface()
    {
        destroy_surface();
    }
    void Surface::destroy_surface()
    {
        if (handle_ != VK_NULL_HANDLE)
        {
            if (instance_ == nullptr || (VkInstance)(*instance_) == VK_NULL_HANDLE)
            {
                throw std::runtime_error("faild to destroy surface : instance_ is nullptr");
            }
            vkDestroySurfaceKHR(*instance_, handle_, nullptr);
            handle_ = VK_NULL_HANDLE;
        }
    }

} // namespace vkcpp