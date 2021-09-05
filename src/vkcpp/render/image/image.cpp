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
        init_texture_image();
        init_texture_image_view();
        init_texture_sampler();
    }
    Image::~Image()
    {
        destroy_image();
    }
    void Image::init_texture_image()
    {

        int tex_width, tex_height, tex_channels;
        stbi_uc *pixels = stbi_load(filename_, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
        VkDeviceSize image_size = tex_width * tex_height * 4;

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
            VK_FORMAT_R8G8B8A8_SRGB,
            {static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height), 1U},
            VK_IMAGE_TILING_OPTIMAL,
            VK_SAMPLE_COUNT_1_BIT,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            handle_,
            memory_);

        CommandBuffers cmd_buffer = std::move(CommandBuffers::beginSingleTimeCmd(device_, command_pool_));

        CommandBuffers::cmdImageMemoryBarrier(
            cmd_buffer[0],
            handle_,
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
            handle_,
            static_cast<uint32_t>(tex_width),
            static_cast<uint32_t>(tex_height));

        CommandBuffers::cmdImageMemoryBarrier(
            cmd_buffer[0],
            handle_,
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
        view_ = create::imageView(*device_, VK_IMAGE_VIEW_TYPE_2D, handle_, VK_FORMAT_R8G8B8A8_SRGB, {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
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
        if (view_ != VK_NULL_HANDLE)
        {
            vkDestroyImageView(*device_, view_, nullptr);
            view_ = VK_NULL_HANDLE;
        }

        if (handle_ != VK_NULL_HANDLE)
        {
            vkDestroyImage(*device_, handle_, nullptr);
            handle_ = VK_NULL_HANDLE;
        }
        if (memory_ != VK_NULL_HANDLE)
        {
            vkFreeMemory(*device_, memory_, nullptr);
            memory_ = VK_NULL_HANDLE;
        }
    }

} // namespace vkcpp
