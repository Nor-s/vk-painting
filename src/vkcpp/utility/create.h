#ifndef VKCPP_UTILITY_CREATE_H
#define VKCPP_UTILITY_CREATE_H

#include "vulkan_header.h"

namespace vkcpp
{
    class Device;
    namespace create
    {
        VkImageView imageView(VkDevice device,
                              VkImageViewType view_type,
                              VkImage image,
                              VkFormat format,
                              VkImageSubresourceRange sub_resource_range);

        void image(const Device *device,
                   uint32_t width,
                   uint32_t height,
                   VkFormat format,
                   VkImageTiling tiling,
                   VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties,
                   VkImage &image,
                   VkDeviceMemory &imageMemory);

        void buffer(const Device *device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &memory);
    } // namespace create
} // namespace vkcpp

#endif