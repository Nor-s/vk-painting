#ifndef VKCPP_DEVICE_DEVICE_H
#define VKCPP_DEVICE_DEVICE_H

#include "vulkan_header.h"
#include <memory>

namespace vkcpp
{
    class Instance;
    class Surface;
    class PhysicalDevice;
    class Queue;

    /**
     *  @brief A wrapper class for VkDevice
     */
    class Device
    {
    private:
        const PhysicalDevice *gpu_;
        const Surface *surface_;
        std::unique_ptr<Queue> graphics_queue_{nullptr};
        std::unique_ptr<Queue> present_queue_{nullptr};
        std::unique_ptr<Queue> compute_queue_{nullptr};
        std::unique_ptr<Queue> transfer_queue_{nullptr};
        VkDevice handle_{VK_NULL_HANDLE};

        //      std::vector<VkExtensionProperties> device_extensions;
        //        std::vector<const char *> enabled_extensions{};

        //    std::vector<std::vector<Queue>> queues;

    public:
        Device(const PhysicalDevice *gpu, const Surface *surface);
        ~Device();
        operator const VkDevice &() const { return handle_; }
    };
} // namespace vkcpp
#endif // #ifndef VKCPP_DEVICE_DEVICE_H