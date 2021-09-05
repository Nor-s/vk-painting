#ifndef VKCPP_RENDER_IMAGE_IMAGE_H
#define VKCPP_RENDER_IMAGE_IMAGE_H

#include "stb/stb_image.h"
#include "vulkan_header.h"

namespace vkcpp
{
    class Device;
    class CommandPool;
    class Image
    {
    private:
        const Device *device_;

        const CommandPool *command_pool_;

        const char *filename_;

        VkImage handle_{VK_NULL_HANDLE};

        VkImageView view_{VK_NULL_HANDLE};

        VkSampler sampler_{VK_NULL_HANDLE};

        VkDeviceMemory memory_{VK_NULL_HANDLE};

    public:
        Image(const Device *device, const CommandPool *command_pool, const char *filename);

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