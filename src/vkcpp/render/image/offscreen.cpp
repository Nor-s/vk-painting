#include "offscreen.h"
#include "device/device.h"
#include "device/physical_device.h"
#include "render/command/command_buffers.h"

#include <iostream>

#include "utility/create.h"
namespace vkcpp
{
    Offscreen::Offscreen(const Device *device,
                         const VkExtent3D &extent,
                         VkSampleCountFlagBits samples) : Image(device,
                                                                nullptr,
                                                                VK_FILTER_LINEAR,
                                                                VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                                                                samples,
                                                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                                                VK_FORMAT_B8G8R8A8_SRGB,
                                                                1, 1, extent)
    {
        create::image(
            device_,
            VK_IMAGE_TYPE_2D,
            format_,
            extent_,
            VK_IMAGE_TILING_OPTIMAL,
            samples_,
            usage_,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            image_,
            memory_);

        view_ = create::imageView(
            *device_,
            VK_IMAGE_VIEW_TYPE_2D,
            image_,
            format_,
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        init_sampler(false, 1);
    }
} // namespace vkcpp