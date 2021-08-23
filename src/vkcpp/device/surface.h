#ifndef VKCPP_SURFACE_H
#define VKCPP_SURFACE_H

#include "vkcpp/vulkan_header.h"

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
