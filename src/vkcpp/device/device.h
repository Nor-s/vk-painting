#ifndef VKCPP_DEVICE_DEVICE_H
#define VKCPP_DEVICE_DEVICE_H

#include "vulkan_header.h"

namespace vkcpp
{
    class Instance;
    class Surface;
    class PhysicalDevice;

    /**
     *  @brief A wrapper class for VkDevice
     */
    class Device
    {
    private:
        VkQueue graphics_queue_;
        VkQueue present_queue_;
        VkDevice handle_;
        const PhysicalDevice *gpu_;
        const Surface *surface_;

        //      std::vector<VkExtensionProperties> device_extensions;
        //        std::vector<const char *> enabled_extensions{};

        //    std::vector<std::vector<Queue>> queues;

    public:
        Device(const PhysicalDevice *gpu, const Surface *surface, const Instance *instance);
        ~Device();
    };
} // namespace vkcpp
#endif // #ifndef VKCPP_DEVICE_DEVICE_H