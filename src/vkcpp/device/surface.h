#ifndef VKCPP_DEVICE_SURFACE_H
#define VKCPP_DEVICE_SURFACE_H

#include "vulkan_header.h"

namespace vkcpp
{
    class Surface
    {
    private:
        VkSurfaceKHR handle_{VK_NULL_HANDLE};

    public:
        Surface() = default;
    };
} // namespace vkcpp

#endif
