#include "descriptor_set.h"

#include "device/device.h"

#include <iostream>

namespace vkcpp
{

    void DescriptorSet::init_layout_bindings()
    {
        for (int i = 0; i < layout_bindings_size_; i++)
        {
            VkDescriptorSetLayoutBinding layout_binding{};
            layout_binding.binding = 0;
            layout_binding.descriptorCount = 1;
            layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            layout_binding.pImmutableSamplers = nullptr;
            layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        }
    }

    void DescriptorSet::init_layout()
    {
        VkDescriptorSetLayoutCreateInfo layout_info{};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = layout_bindings_.size();
        layout_info.pBindings = layout_bindings_.data();

        if (vkCreateDescriptorSetLayout(*device_, &layout_info, nullptr, &layout_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }
    void DescriptorSet::destroy_layout()
    {
        if (layout_ != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(*device_, layout_, nullptr);
            layout_ = VK_NULL_HANDLE;
        }
    }

}