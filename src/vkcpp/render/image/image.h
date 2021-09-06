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
    public:
        struct Attachment
        {
            VkImage image{VK_NULL_HANDLE};
            VkDeviceMemory memory{VK_NULL_HANDLE};
            VkImageView view{VK_NULL_HANDLE};
            VkFormat format{};
        };

    private:
        const Device *device_{nullptr};

        const CommandPool *command_pool_{nullptr};

        const char *filename_{VK_NULL_HANDLE};

        Attachment color_attachment_{};

        Attachment depth_attachment_{};

        VkSampler sampler_{VK_NULL_HANDLE};

        VkExtent3D extent_{};

        int tex_width_;

        int tex_height_;

        int tex_channels_;

    public:
        Image(const Device *device, const CommandPool *command_pool, const char *filename);

        Image(const Device *device, const CommandPool *command_pool, VkExtent3D extent);

        virtual ~Image();

        const VkImageView &get_image_view() const { return color_attachment_.view; }

        const VkSampler &get_sampler() const { return sampler_; }

        const VkFormat &get_color_format() const { return color_attachment_.format; }

        const VkFormat &get_depth_format() const { return depth_attachment_.format; }

        const VkImageView &get_color_view() const { return color_attachment_.view; }

        const VkImageView &get_depth_view() const { return depth_attachment_.view; }

        const VkExtent3D &get_extent() const { return extent_; }

        const std::pair<int, int> get_size() const { return {tex_width_, tex_height_}; };

        void init_texture_image();

        void init_texture_image_view();

        void init_texture_sampler();

        void destroy_image();

        /**
         * Must same width and height
         * Maybe problem : read and write at the same time
         */
        void sub_texture_image(const char *filename);
        /**
         * Must same width and height
         * Maybe problem : read and write at the same time
         */
        void sub_texture_image(VkImage host_src_image, VkExtent3D src_extent, const VkFormat &swapchain_color_format);

        void init_image();

        void init_sampler();
    };

} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_IMAGE_IMAGE_H