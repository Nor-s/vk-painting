#ifndef VKCPP_RENDER_IMAGE_IMAGE_H
#define VKCPP_RENDER_IMAGE_IMAGE_H

#include "stb/stb_image.h"
#include "vulkan_header.h"
#include "render/buffer/base_buffer.h"

#include <string>

namespace vkcpp
{
    class Image : public BaseBuffer
    {
    private:
        std::string filename_;

        VkImage handle_{VK_NULL_HANDLE};

        VkImageView view_{VK_NULL_HANDLE};

        VkSampler sampler_{VK_NULL_HANDLE};

        VkDeviceMemory memory_{VK_NULL_HANDLE};

    public:
        Image(const Device *device, const CommandPool *command_pool);

        const VkImageView &get_image_view() { return view_; }
        const VkSampler &get_sampler() { return sampler_; }

        void init_texture_image();

        void init_texture_image_view();

        void init_texture_sampler();

        void create_image(uint32_t width,
                          uint32_t height,
                          VkFormat format,
                          VkImageTiling tiling,
                          VkImageUsageFlags usage,
                          VkMemoryPropertyFlags properties,
                          VkImage &image,
                          VkDeviceMemory &imageMemory);

        void transition_image_layout(VkImage image,
                                     VkFormat format,
                                     VkImageLayout oldLayout,
                                     VkImageLayout newLayout);

        void copy_buffer_to_image(VkBuffer buffer,
                                  VkImage image,
                                  uint32_t width,
                                  uint32_t height);

        void destroy_image();
    };

} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_IMAGE_IMAGE_H