
#include "base_image.h"
namespace vkcpp
{
    /**
     *  components = VK_COMPONENT_SWIZZLE_IDENTITY;
     *  aspectMask = Vk_IMAGE_ASPECT_COLOR_BIT; //for color target
     *  miplevel,layer = 0
     *  miplevelCount,layerCount = 1
     */
    VkImageView BaseImage::create_image_view(const Device *device, VkImage image, VkFormat format)
    {
        // Specify createinfo.
        VkImageViewCreateInfo view_info{};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = image;
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D; // dimension of texture.
        view_info.format = format;

        // components field allows to swizzle the color channels around.
        view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // Describes what the image's purpose is and which part of the image should be accessed.
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        VkImageView image_view;
        if (vkCreateImageView(*device, &view_info, nullptr, &image_view) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }

        return image_view;
    }
}