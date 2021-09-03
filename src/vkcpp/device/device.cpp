#include "device.h"

#include "instance.h"
#include "surface.h"
#include "physical_device.h"
#include "queue.h"

#include <iostream>
#include <vector>
#include <set>

namespace vkcpp
{
    Device::Device(const PhysicalDevice *gpu)
        : gpu_(gpu)
    {
        init_device(gpu_);
        init_queues(gpu_);
    }
    Device::~Device()
    {
        if (handle_ != VK_NULL_HANDLE)
        {
            vkDestroyDevice(handle_, nullptr);
        }
    }

    void Device::init_device(const PhysicalDevice *gpu)
    {
        const QueueFamilyIndices &indices = gpu->get_queue_family_indices();
        const Instance &instance = gpu->get_instance();

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), indices.present_family.value()};
        if (indices.compute_family.has_value())
        {
            unique_queue_families.insert(indices.compute_family.value());
        }
        if (indices.transfer_family.has_value())
        {
            unique_queue_families.insert(indices.transfer_family.value());
        }

        // Specify queues to be created
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

        // Specify device features
        const VkPhysicalDeviceFeatures &device_features = gpu->get_features();

        // Specify device extensions
        const std::vector<const char *> &device_extensions = gpu->get_extensions();

        // Specify Device create info
        VkDeviceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
        create_info.pQueueCreateInfos = queue_create_infos.data();
        create_info.pEnabledFeatures = &device_features;
        create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
        create_info.ppEnabledExtensionNames = device_extensions.data();

        if (instance.get_enable_validation_layers())
        {
            create_info.enabledLayerCount = static_cast<uint32_t>(instance.validation_layers_.size());
            create_info.ppEnabledLayerNames = instance.validation_layers_.data();
        }
        else
        {
            create_info.enabledLayerCount = 0;
        }

        // Creat the logicald device
        if (vkCreateDevice(*gpu, &create_info, nullptr, &handle_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }
    }
    void Device::init_queues(const PhysicalDevice *gpu)
    {
        const QueueFamilyIndices &indices = gpu->get_queue_family_indices();

        graphics_queue_ = std::make_unique<Queue>(this, indices.graphics_family.value(), 0, false, gpu->get_queue_family_properties()[indices.graphics_family.value()]);
        present_queue_ = std::make_unique<Queue>(this, indices.present_family.value(), 0, true, gpu->get_queue_family_properties()[indices.present_family.value()]);
        if (indices.compute_family.has_value())
        {
            graphics_queue_ = std::make_unique<Queue>(this, indices.compute_family.value(), 0, false, gpu->get_queue_family_properties()[indices.compute_family.value()]);
        }
        if (indices.transfer_family.has_value())
        {
            graphics_queue_ = std::make_unique<Queue>(this, indices.transfer_family.value(), 0, false, gpu->get_queue_family_properties()[indices.transfer_family.value()]);
        }
    }

} // namespace vkcpp