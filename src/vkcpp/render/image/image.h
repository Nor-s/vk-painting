#ifndef VKCPP_RENDER_IMAGE_IMAGE_H
#define VKCPP_RENDER_IMAGE_IMAGE_H

#include "stb/stb_image.h"
#include "vulkan_header.h"
#include <string>

namespace vkcpp
{
    class Device;
    class CommandPool;
    class Image
    {
    public:
        static void cmd_image_memory_barrier(VkCommandBuffer cmdbuffer,
                                             VkImage image,
                                             VkAccessFlags srcAccessMask,
                                             VkAccessFlags dstAccessMask,
                                             VkImageLayout oldImageLayout,
                                             VkImageLayout newImageLayout,
                                             VkPipelineStageFlags srcStageMask,
                                             VkPipelineStageFlags dstStageMask,
                                             VkImageSubresourceRange subresourceRange);

        static void cmd_copy_buffer_to_image(VkCommandBuffer cmd_buffer,
                                             VkBuffer buffer,
                                             VkImage image,
                                             uint32_t width,
                                             uint32_t height);

    private:
        const Device *device_;

        const CommandPool *command_pool_;

        std::string filename_;

        VkImage handle_{VK_NULL_HANDLE};

        VkImageView view_{VK_NULL_HANDLE};

        VkSampler sampler_{VK_NULL_HANDLE};

        VkDeviceMemory memory_{VK_NULL_HANDLE};

    public:
        Image(const Device *device, const CommandPool *command_pool, const std::string &filename);

        virtual ~Image();

        const VkImageView &get_image_view() const { return view_; }

        const VkSampler &get_sampler() const { return sampler_; }

        void init_texture_image();

        void init_texture_image_view();

        void init_texture_sampler();

        void destroy_image();
    };

} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_IMAGE_IMAGE_H