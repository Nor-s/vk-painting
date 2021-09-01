#define STB_IMAGE_IMPLEMENTATION
#include "image.h"
#include "device/device.h"
#include "device/physical_device.h"
#include "render/command/command_buffers.h"

#include <iostream>

namespace vkcpp
{
    Image::Image(const Device *device, const CommandPool *command_pool, const std::string &filename)
        : BaseBuffer(device, command_pool), filename_(filename)
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
        stbi_uc *pixels = stbi_load(filename_.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
        VkDeviceSize image_size = tex_width * tex_height * 4;

        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }

        VkBuffer staging_buffer;
        VkDeviceMemory staging_memory;
        create_buffer(
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

        create_image(
            tex_width,
            tex_height,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            handle_,
            memory_);

        transition_image_layout(
            handle_,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copy_buffer_to_image(staging_buffer, handle_, static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height));
        transition_image_layout(
            handle_,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(*device_, staging_buffer, nullptr);
        vkFreeMemory(*device_, staging_memory, nullptr);
    }
    void Image::create_image(uint32_t width,
                             uint32_t height,
                             VkFormat format,
                             VkImageTiling tiling,
                             VkImageUsageFlags usage,
                             VkMemoryPropertyFlags properties,
                             VkImage &image,
                             VkDeviceMemory &imageMemory)
    {
        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D; // for 2D
        image_info.extent.width = width;
        image_info.extent.height = height;
        image_info.extent.depth = 1; // for 2D
        image_info.mipLevels = 1;
        image_info.arrayLayers = 1;
        image_info.format = format;
        image_info.tiling = tiling;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage = usage;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.flags = 0; // Optional
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(*device_, &image_info, nullptr, &image) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(*device_, image, &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties);

        if (vkAllocateMemory(*device_, &alloc_info, nullptr, &imageMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(*device_, image, imageMemory, 0);
    }

    void Image::transition_image_layout(VkImage image,
                                        VkFormat format,
                                        VkImageLayout oldLayout,
                                        VkImageLayout newLayout)
    {
        CommandBuffers cmd_buffer = std::move(begin_single_time_cmd());

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // For queue family ownership
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // For queue family ownership
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            cmd_buffer[0],
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        end_single_time_cmd(cmd_buffer);
    }

    void Image::copy_buffer_to_image(VkBuffer buffer,
                                     VkImage image,
                                     uint32_t width,
                                     uint32_t height)
    {
        CommandBuffers cmd_buffer = std::move(begin_single_time_cmd());

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1};

        vkCmdCopyBufferToImage(
            cmd_buffer[0],
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region);

        end_single_time_cmd(cmd_buffer);
    }

    void Image::init_texture_image_view()
    {
        view_ = create_image_view(device_, handle_, VK_FORMAT_R8G8B8A8_SRGB);
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
