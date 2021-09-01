#ifndef VKCPP_RENDER_IMAGE_BASE_IMAGE_HPP
#define VKCPP_RENDER_IMAGE_BASE_IMAGE_HPP

#include "vulkan_header.h"
#include "device/device.h"
#include <iostream>

namespace vkcpp
{
    class BaseImage
    {
    public:
        BaseImage() = default;
        virtual ~BaseImage() = default;

    protected:
        VkImageView create_image_view(const Device *device, VkImage image, VkFormat format);
    };

} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_IMAGE_BASE_IMAGE_HPP