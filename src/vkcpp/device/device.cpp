#include "device.h"

#include "instance.h"
#include "surface.h"
#include "physical_device.h"

#include <iostream>
#include <vector>
#include <set>

namespace vkcpp
{
    Device::Device(const PhysicalDevice *gpu, const Surface *surface, const Instance *instance)
        : gpu_(gpu), surface_(surface)
    {
        const QueueFamilyIndices &indices = gpu_->get_ref_queue_family_indices();
        const std::vector<const char *> &device_extensions = gpu->get_ref_extensions();

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), indices.present_family.value()};

        float queue_priority = 1.0f;
        for (uint32_t queue_family : unique_queue_families)
        {
            VkDeviceQueueCreateInfo queue_create_info{};
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queue_family;
            queue_create_info.queueCount = 1;
            queue_create_info.pQueuePriorities = &queue_priority;
            queue_create_infos.push_back(queue_create_info);
        }

        VkPhysicalDeviceFeatures device_features{};
        device_features.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
        create_info.pQueueCreateInfos = queue_create_infos.data();

        create_info.pEnabledFeatures = &device_features;

        create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
        create_info.ppEnabledExtensionNames = device_extensions.data();

        if (instance->get_enable_validation_layers())
        {
            create_info.enabledLayerCount = static_cast<uint32_t>(instance->validation_layers_.size());
            create_info.ppEnabledLayerNames = instance->validation_layers_.data();
        }
        else
        {
            create_info.enabledLayerCount = 0;
        }

        if (vkCreateDevice(*gpu, &create_info, nullptr, &handle_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(handle_, indices.graphics_family.value(), 0, &graphics_queue_);
        vkGetDeviceQueue(handle_, indices.present_family.value(), 0, &present_queue_);
    }
    Device::~Device()
    {
        if (handle_ != VK_NULL_HANDLE)
        {
            vkDestroyDevice(handle_, nullptr);
        }
    }

} // namespace vkcpp