#ifndef VKCPP_UTILITY_UTILITY_HPP
#define VKCPP_UTILITY_UTILITY_HPP

#include "vulkan_header.h"
#include <fstream>
#include <vector>
#include <string>

namespace vkcpp
{
    void VK_CHECK_RESULT(VkBool32 a)
    {
        if (a != VK_SUCCESS)
        {
            return;
        }
    }
    std::vector<char> readFile(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }

        size_t file_size = (size_t)file.tellg();
        std::vector<char> buffer(file_size);

        file.seekg(0);
        file.read(buffer.data(), file_size);

        file.close();

        return buffer;
    }
    /*
    // Take a screenshot from the current swapchain image
    // This is done using a blit from the swapchain image to a linear image whose memory content is then saved as a ppm image
    // Getting the image date directly from a swapchain image wouldn't work as they're usually stored in an implementation dependent optimal tiling format
    // Note: This requires the swapchain images to be created with the VK_IMAGE_USAGE_TRANSFER_SRC_BIT flag (see VulkanSwapChain::create)
    void saveScreenshot(const char *filename, VkDevice device, VkPhysicalDevice gpu, VkFormat swapchain_color_format, VkImage src_image, VkExtent3D extent)
    {
        static bool screenshotSaved = false;
        bool supportsBlit = true;

        // Check blit support for source and destination
        VkFormatProperties formatProps;

        // Check if the device supports blitting from optimal images (the swapchain images are in optimal format)
        vkGetPhysicalDeviceFormatProperties(gpu, swapchain_color_format, &formatProps);
        if (!(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT))
        {
            std::cerr << "Device does not support blitting from optimal tiled images, using copy instead of blit!" << std::endl;
            supportsBlit = false;
        }

        // Check if the device supports blitting to linear images
        vkGetPhysicalDeviceFormatProperties(gpu, VK_FORMAT_R8G8B8A8_UNORM, &formatProps);
        if (!(formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT))
        {
            std::cerr << "Device does not support blitting to linear tiled images, using copy instead of blit!" << std::endl;
            supportsBlit = false;
        }

        // Source for the copy is the last rendered swapchain image
        // VkImage srcImage = swapChain.images[currentBuffer];

        // Create the linear tiled destination image to copy to and to read the memory from
        VkImageCreateInfo image_create_info{};
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.imageType = VK_IMAGE_TYPE_2D;
        // Note that vkCmdBlitImage (if supported) will also do format conversions if the swapchain color format would differ
        image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
        image_create_info.extent = extent;
        image_create_info.arrayLayers = 1;
        image_create_info.mipLevels = 1;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_create_info.tiling = VK_IMAGE_TILING_LINEAR;
        image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        // Create the image
        VkImage dstImage;
        VK_CHECK_RESULT(vkCreateImage(device, &image_create_info, nullptr, &dstImage));

        // Create memory to back up the image
        VkMemoryRequirements mem_requirements{};
        VkMemoryAllocateInfo mem_alloc_info{};
        VkDeviceMemory dstImageMemory{nullptr};
        vkGetImageMemoryRequirements(device, dstImage, &mem_requirements);
        mem_alloc_info.allocationSize = mem_requirements.size;
        // Memory must be host visible to copy from
        mem_alloc_info.memoryTypeIndex = buffer->find_memory_type(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        VK_CHECK_RESULT(vkAllocateMemory(device, &mem_alloc_info, nullptr, &dstImageMemory));
        VK_CHECK_RESULT(vkBindImageMemory(device, dstImage, dstImageMemory, 0));

        // Do the actual blit from the swapchain image to our host visible destination image
        VkCommandBuffer copyCmd = vulkanDevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
        // begin
        // Transition destination image to transfer destination layout
        vks::tools::insertImageMemoryBarrier(
            copyCmd,
            dstImage,
            0,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        // Transition swapchain image from present to transfer source layout
        vks::tools::insertImageMemoryBarrier(
            copyCmd,
            srcImage,
            VK_ACCESS_MEMORY_READ_BIT,
            VK_ACCESS_TRANSFER_READ_BIT,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        // If source and destination support blit we'll blit as this also does automatic format conversion (e.g. from BGR to RGB)
        if (supportsBlit)
        {
            // Define the region to blit (we will blit the whole swapchain image)
            VkOffset3D blitSize;
            blitSize.x = width;
            blitSize.y = height;
            blitSize.z = 1;
            VkImageBlit imageBlitRegion{};
            imageBlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageBlitRegion.srcSubresource.layerCount = 1;
            imageBlitRegion.srcOffsets[1] = blitSize;
            imageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageBlitRegion.dstSubresource.layerCount = 1;
            imageBlitRegion.dstOffsets[1] = blitSize;

            // Issue the blit command
            vkCmdBlitImage(
                copyCmd,
                srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &imageBlitRegion,
                VK_FILTER_NEAREST);
        }
        else
        {
            // Otherwise use image copy (requires us to manually flip components)
            VkImageCopy imageCopyRegion{};
            imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageCopyRegion.srcSubresource.layerCount = 1;
            imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageCopyRegion.dstSubresource.layerCount = 1;
            imageCopyRegion.extent.width = width;
            imageCopyRegion.extent.height = height;
            imageCopyRegion.extent.depth = 1;

            // Issue the copy command
            vkCmdCopyImage(
                copyCmd,
                srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &imageCopyRegion);
        }

        // Transition destination image to general layout, which is the required layout for mapping the image memory later on
        vks::tools::insertImageMemoryBarrier(
            copyCmd,
            dstImage,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_MEMORY_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_GENERAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        // Transition back the swap chain image after the blit is done
        vks::tools::insertImageMemoryBarrier(
            copyCmd,
            srcImage,
            VK_ACCESS_TRANSFER_READ_BIT,
            VK_ACCESS_MEMORY_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        vulkanDevice->flushCommandBuffer(copyCmd, queue);

        // Get layout of the image (including row pitch)
        VkImageSubresource subResource{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0};
        VkSubresourceLayout subResourceLayout;
        vkGetImageSubresourceLayout(device, dstImage, &subResource, &subResourceLayout);

        // Map image memory so we can start copying from it
        const char *data;
        vkMapMemory(device, dstImageMemory, 0, VK_WHOLE_SIZE, 0, (void **)&data);
        data += subResourceLayout.offset;

        std::ofstream file(filename, std::ios::out | std::ios::binary);

        // ppm header
        file << "P6\n"
             << width << "\n"
             << height << "\n"
             << 255 << "\n";

        // If source is BGR (destination is always RGB) and we can't use blit (which does automatic conversion), we'll have to manually swizzle color components
        bool colorSwizzle = false;
        // Check if source is BGR
        // Note: Not complete, only contains most common and basic BGR surface formats for demonstration purposes
        if (!supportsBlit)
        {
            std::vector<VkFormat> formatsBGR = {VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_SNORM};
            colorSwizzle = (std::find(formatsBGR.begin(), formatsBGR.end(), swapChain.colorFormat) != formatsBGR.end());
        }

        // ppm binary pixel data
        for (uint32_t y = 0; y < height; y++)
        {
            unsigned int *row = (unsigned int *)data;
            for (uint32_t x = 0; x < width; x++)
            {
                if (colorSwizzle)
                {
                    file.write((char *)row + 2, 1);
                    file.write((char *)row + 1, 1);
                    file.write((char *)row, 1);
                }
                else
                {
                    file.write((char *)row, 3);
                }
                row++;
            }
            data += subResourceLayout.rowPitch;
        }
        file.close();

        std::cout << "Screenshot saved to disk" << std::endl;

        // Clean up resources
        vkUnmapMemory(device, dstImageMemory);
        vkFreeMemory(device, dstImageMemory, nullptr);
        vkDestroyImage(device, dstImage, nullptr);

        screenshotSaved = true;
    }
*/
    /**
	* Finish command buffer recording and submit it to a queue
	*
	* @param commandBuffer Command buffer to flush
	* @param queue Queue to submit the command buffer to
	* @param pool Command pool on which the command buffer has been created
	* @param free (Optional) Free the command buffer once it has been submitted (Defaults to true)
	*
	* @note The queue that the command buffer is submitted to must be from the same family index as the pool it was allocated from
	* @note Uses a fence to ensure command buffer has finished executing
	*/
    /*
    void VulkanDevice::flushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, VkCommandPool pool, bool free)
    {
        if (commandBuffer == VK_NULL_HANDLE)
        {
            return;
        }

        VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));

        VkSubmitInfo submitInfo = vks::initializers::submitInfo();
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        // Create fence to ensure that the command buffer has finished executing
        VkFenceCreateInfo fenceInfo = vks::initializers::fenceCreateInfo(VK_FLAGS_NONE);
        VkFence fence;
        VK_CHECK_RESULT(vkCreateFence(logicalDevice, &fenceInfo, nullptr, &fence));
        // Submit to the queue
        VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, fence));
        // Wait for the fence to signal that command buffer has finished executing
        VK_CHECK_RESULT(vkWaitForFences(logicalDevice, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT));
        vkDestroyFence(logicalDevice, fence, nullptr);
        if (free)
        {
            vkFreeCommandBuffers(logicalDevice, pool, 1, &commandBuffer);
        }
    }
*/
} // namespace vkcpp

#endif // #ifndef VKCPP_UTILITY_UTILITY_HPP