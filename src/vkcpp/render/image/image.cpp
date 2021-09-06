#define STB_IMAGE_IMPLEMENTATION
#include "image.h"
#include "device/device.h"
#include "device/physical_device.h"
#include "render/command/command_buffers.h"

#include <iostream>

#include "utility/create.h"

namespace vkcpp
{
    Image::Image(const Device *device, const CommandPool *command_pool, const char *filename)
        : device_(device), command_pool_(command_pool), filename_(filename)
    {
        color_attachment_.format = VK_FORMAT_R8G8B8A8_SRGB;
        init_texture_image();
        init_texture_image_view();
        init_texture_sampler();
    }
    Image::Image(const Device *device, const CommandPool *command_pool, VkExtent3D extent)
        : device_(device), command_pool_(command_pool), filename_(nullptr), extent_(extent)
    {
        color_attachment_.format = VK_FORMAT_R8G8B8A8_UNORM;
        tex_width_ = extent_.width;
        tex_height_ = extent_.height;
        tex_channels_ = 1;

        init_image();
        init_sampler();
    }
    Image::~Image()
    {
        destroy_image();
    }
    void Image::init_texture_image()
    {

        stbi_uc *pixels = stbi_load(filename_, &tex_width_, &tex_height_, &tex_channels_, STBI_rgb_alpha);
        VkDeviceSize image_size = tex_width_ * tex_height_ * 4;

        extent_ = {static_cast<uint32_t>(tex_width_), static_cast<uint32_t>(tex_height_), 1U};

        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }

        VkBuffer staging_buffer;
        VkDeviceMemory staging_memory;
        create::buffer(
            device_,
            image_size,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            staging_buffer,
            staging_memory);

        void *data;
        vkMapMemory(*device_, staging_memory, 0, image_size, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(image_size));
        vkUnmapMemory(*device_, staging_memory);

        stbi_image_free(pixels);

        create::image(
            device_,
            VK_IMAGE_TYPE_2D,
            color_attachment_.format,
            {static_cast<uint32_t>(tex_width_), static_cast<uint32_t>(tex_height_), 1U},
            VK_IMAGE_TILING_OPTIMAL,
            VK_SAMPLE_COUNT_1_BIT,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            color_attachment_.image,
            color_attachment_.memory);

        CommandBuffers cmd_buffer = std::move(CommandBuffers::beginSingleTimeCmd(device_, command_pool_));

        CommandBuffers::cmdImageMemoryBarrier(
            cmd_buffer[0],
            color_attachment_.image,
            0,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        CommandBuffers::cmdCopyBufferToImage(
            cmd_buffer[0],
            staging_buffer,
            color_attachment_.image,
            static_cast<uint32_t>(tex_width_),
            static_cast<uint32_t>(tex_height_));

        CommandBuffers::cmdImageMemoryBarrier(
            cmd_buffer[0],
            color_attachment_.image,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        CommandBuffers::endSingleTimeCmd(cmd_buffer);

        vkDestroyBuffer(*device_, staging_buffer, nullptr);
        vkFreeMemory(*device_, staging_memory, nullptr);
    }

    void Image::init_texture_image_view()
    {
        color_attachment_.view = create::imageView(*device_, VK_IMAGE_VIEW_TYPE_2D, color_attachment_.image, VK_FORMAT_R8G8B8A8_SRGB, {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
    }

    void Image::init_texture_sampler()
    {

        VkSamplerCreateInfo sampler_info{};

        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VK_FILTER_LINEAR; // or VK_FILTER_NEAREST
        sampler_info.minFilter = VK_FILTER_LINEAR; // or VK_FILTER_NEAREST

        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        const VkPhysicalDeviceProperties &properties = device_->get_gpu().get_properties();
        sampler_info.anisotropyEnable = VK_TRUE; // or VK_FALSE ... maxAnistropy = 1.0
        sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

        sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;

        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;

        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.mipLodBias = 0.0f;
        sampler_info.minLod = 0.0f;
        sampler_info.maxLod = 0.0f;

        if (vkCreateSampler(*device_, &sampler_info, nullptr, &sampler_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void Image::destroy_image()
    {
        if (sampler_ != VK_NULL_HANDLE)
        {
            vkDestroySampler(*device_, sampler_, nullptr);
            sampler_ = VK_NULL_HANDLE;
        }

        // destroy color acttachment
        if (color_attachment_.view != VK_NULL_HANDLE)
        {
            vkDestroyImageView(*device_, color_attachment_.view, nullptr);
            color_attachment_.view = VK_NULL_HANDLE;
        }
        if (color_attachment_.image != VK_NULL_HANDLE)
        {
            vkDestroyImage(*device_, color_attachment_.image, nullptr);
            color_attachment_.image = VK_NULL_HANDLE;
        }
        if (color_attachment_.memory != VK_NULL_HANDLE)
        {
            vkFreeMemory(*device_, color_attachment_.memory, nullptr);
            color_attachment_.memory = VK_NULL_HANDLE;
        }

        // destroy depth acttachment
        if (depth_attachment_.view != VK_NULL_HANDLE)
        {
            vkDestroyImageView(*device_, depth_attachment_.view, nullptr);
            depth_attachment_.view = VK_NULL_HANDLE;
        }
        if (depth_attachment_.image != VK_NULL_HANDLE)
        {
            vkDestroyImage(*device_, depth_attachment_.image, nullptr);
            depth_attachment_.image = VK_NULL_HANDLE;
        }
        if (depth_attachment_.memory != VK_NULL_HANDLE)
        {
            vkFreeMemory(*device_, depth_attachment_.memory, nullptr);
            depth_attachment_.memory = VK_NULL_HANDLE;
        }
    }

    void Image::sub_texture_image(const char *filename)
    {
        int tex_width, tex_height, tex_channels;
        stbi_uc *pixels = stbi_load(filename, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
        VkDeviceSize image_size = tex_width * tex_height * 4;
        VkDeviceSize extent_size = tex_width_ * tex_height_ * 4;

        if (image_size != extent_size)
        {
            stbi_image_free(pixels);
            return;
        }

        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }

        VkBuffer staging_buffer;
        VkDeviceMemory staging_memory;
        create::buffer(
            device_,
            image_size,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            staging_buffer,
            staging_memory);

        void *data;
        vkMapMemory(*device_, staging_memory, 0, image_size, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(image_size));
        vkUnmapMemory(*device_, staging_memory);

        stbi_image_free(pixels);

        CommandBuffers cmd_buffer = std::move(CommandBuffers::beginSingleTimeCmd(device_, command_pool_));

        CommandBuffers::cmdImageMemoryBarrier(
            cmd_buffer[0],
            color_attachment_.image,
            0,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        CommandBuffers::cmdCopyBufferToImage(
            cmd_buffer[0],
            staging_buffer,
            color_attachment_.image,
            static_cast<uint32_t>(tex_width_),
            static_cast<uint32_t>(tex_height_));

        CommandBuffers::cmdImageMemoryBarrier(
            cmd_buffer[0],
            color_attachment_.image,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        CommandBuffers::endSingleTimeCmd(cmd_buffer);
        vkDestroyBuffer(*device_, staging_buffer, nullptr);
        vkFreeMemory(*device_, staging_memory, nullptr);
    }

    void Image::sub_texture_image(VkImage host_src_image, VkExtent3D src_extent, const VkFormat &color_format)
    {

        VkExtent3D extent;
        extent.depth = 1;
        extent.height = tex_height_;
        extent.width = tex_width_;
        if (!(extent.width == src_extent.width && extent.height == src_extent.height))
        {
            return;
        }
        bool supportsBlit = device_->check_support_blit(color_format);

        // Do the actual blit from the swapchain image to our host visible destination image
        vkcpp::CommandBuffers copy_cmd = std::move(vkcpp::CommandBuffers::beginSingleTimeCmd(device_, command_pool_));
        // Transition destination image to transfer destination layout
        vkcpp::CommandBuffers::cmdImageMemoryBarrier(
            copy_cmd[0],
            color_attachment_.image,
            0,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        // Transition swapchain image from present to transfer source layout
        vkcpp::CommandBuffers::cmdImageMemoryBarrier(
            copy_cmd[0],
            host_src_image,
            VK_ACCESS_MEMORY_READ_BIT,
            VK_ACCESS_TRANSFER_READ_BIT,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        vkcpp::CommandBuffers::cmdCopyImage(
            copy_cmd[0],
            supportsBlit,
            extent,
            {VK_IMAGE_ASPECT_COLOR_BIT, 1}, {VK_IMAGE_ASPECT_COLOR_BIT, 1},
            host_src_image, color_attachment_.image);

        // Transition destination image to general layout, which is the required layout for mapping the image memory later on
        CommandBuffers::cmdImageMemoryBarrier(
            copy_cmd[0],
            color_attachment_.image,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        // Transition back the swap chain image after the blit is done
        vkcpp::CommandBuffers::cmdImageMemoryBarrier(
            copy_cmd[0],
            host_src_image,
            VK_ACCESS_TRANSFER_READ_BIT,
            VK_ACCESS_MEMORY_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        copy_cmd.flush_command_buffer(0);
    }

} // namespace vkcpp

namespace vkcpp
{
    VkBool32 getSupportedDepthFormat(const VkPhysicalDevice physicalDevice, VkFormat *depthFormat)
    {
        // Since all depth formats may be optional, we need to find a suitable depth format to use
        // Start with the highest precision packed format
        std::vector<VkFormat> depthFormats = {
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM};

        for (auto &format : depthFormats)
        {
            VkFormatProperties formatProps;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);
            // Format must support depth stencil attachment for optimal tiling
            if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
            {
                *depthFormat = format;
                return true;
            }
        }

        return false;
    }
    void Image::init_image()
    {
        uint32_t width = extent_.width;
        uint32_t height = extent_.height;
        // Find a suitable depth format
        VkBool32 validDepthFormat = getSupportedDepthFormat(device_->get_gpu(), &depth_attachment_.format);
        assert(validDepthFormat);

        // Color attachment
        create::image(
            device_,
            VK_IMAGE_TYPE_2D,

            color_attachment_.format,

            {width, height, 1},
            VK_IMAGE_TILING_OPTIMAL,
            VK_SAMPLE_COUNT_1_BIT,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            color_attachment_.image,
            color_attachment_.memory);

        color_attachment_.view = create::imageView(
            *device_,
            VK_IMAGE_VIEW_TYPE_2D,
            color_attachment_.image,

            color_attachment_.format,

            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        // Depth stencil attachment
        create::image(
            device_,
            VK_IMAGE_TYPE_2D,

            depth_attachment_.format,

            {width, height, 1},
            VK_IMAGE_TILING_OPTIMAL,
            VK_SAMPLE_COUNT_1_BIT,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            depth_attachment_.image,
            depth_attachment_.memory);

        depth_attachment_.view = create::imageView(
            *device_,
            VK_IMAGE_VIEW_TYPE_2D,
            depth_attachment_.image,
            depth_attachment_.format,
            {VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1});
    }

    void Image::init_sampler()
    {
        // Create sampler to sample from the attachment in the fragment shader
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = samplerInfo.addressModeU;
        samplerInfo.addressModeW = samplerInfo.addressModeU;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.maxAnisotropy = 1.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 1.0f;
        samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        if (vkCreateSampler(*device_, &samplerInfo, nullptr, &sampler_))
        {
            std::cout << "failed to create sampler for offscrreen\n";
            return;
        }
    }

} // namespace vkcpp