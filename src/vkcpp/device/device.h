#ifndef VKCPP_DEVICE_DEVICE_H
#define VKCPP_DEVICE_DEVICE_H

#include "vulkan_header.h"
#include "queue.h"
#include <memory>
#include <iostream>

namespace vkcpp
{
    class Instance;

    class PhysicalDevice;

    /**
     *  @brief A wrapper class for VkDevice
     */
    class Device
    {
    public:
        const uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties) const;

    private:
        const PhysicalDevice *gpu_;

        std::unique_ptr<Queue> graphics_queue_{nullptr};

        std::unique_ptr<Queue> present_queue_{nullptr};

        std::unique_ptr<Queue> compute_queue_{nullptr};

        std::unique_ptr<Queue> transfer_queue_{nullptr};

        VkDevice handle_{VK_NULL_HANDLE};

    public:
        Device(const PhysicalDevice *gpu);

        ~Device();

        operator const VkDevice &() const
        {
            if (handle_ == VK_NULL_HANDLE)
            {
                throw std::runtime_error("failed to get device");
            }
            return handle_;
        }

        const PhysicalDevice &get_gpu() const { return *gpu_; };

        const VkQueue &get_graphics_queue() const { return *graphics_queue_; }
        const VkQueue &get_present_queue() const { return *present_queue_; }

        void init_device(const PhysicalDevice *gpu);

        void init_queues(const PhysicalDevice *gpu);
    };
} // namespace vkcpp
#endif // #ifndef VKCPP_DEVICE_DEVICE_H