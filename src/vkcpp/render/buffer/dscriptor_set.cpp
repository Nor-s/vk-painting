#include "descriptor_set.h"

#include "device/device.h"

#include <iostream>

namespace vkcpp
{
    DescriptorSet::DescriptorSet(const Device *device, uint32_t size)
        : device_(device), size_(size)
    {
        init_layout();
        init_pool();
        init_descriptor_sets();
    }
    DescriptorSet::~DescriptorSet()
    {
        destroy_pool();
        destroy_layout();
    }

    void DescriptorSet::init_layout_bindings()
    {
        layout_bindings_.resize(2);
        VkDescriptorSetLayoutBinding &layout_binding = layout_bindings_[0];
        layout_binding.binding = 0;
        layout_binding.descriptorCount = 1;
        layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        layout_binding.pImmutableSamplers = nullptr;
        layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding &sampler_binding = layout_bindings_[1];
        sampler_binding.binding = 1;
        sampler_binding.descriptorCount = 1;
        sampler_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        sampler_binding.pImmutableSamplers = nullptr;
        sampler_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    }

    void DescriptorSet::init_layout()
    {
        init_layout_bindings();

        VkDescriptorSetLayoutCreateInfo layout_info{};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = layout_bindings_.size();
        layout_info.pBindings = layout_bindings_.data();

        if (vkCreateDescriptorSetLayout(*device_, &layout_info, nullptr, &layout_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void DescriptorSet::init_pool()
    {
        std::array<VkDescriptorPoolSize, 2> pool_sizes{};
        pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_sizes[0].descriptorCount = size_;
        pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        pool_sizes[1].descriptorCount = size_;

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
        pool_info.pPoolSizes = pool_sizes.data();
        pool_info.maxSets = size_;

        if (vkCreateDescriptorPool(*device_, &pool_info, nullptr, &pool_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void DescriptorSet::init_descriptor_sets()
    {
        std::vector<VkDescriptorSetLayout> layouts(size_, layout_);
        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = pool_;
        alloc_info.descriptorSetCount = size_;
        alloc_info.pSetLayouts = layouts.data();

        descriptor_sets_.resize(size_);
        if (vkAllocateDescriptorSets(*device_, &alloc_info, descriptor_sets_.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
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
    void DescriptorSet::destroy_pool()
    {
        if (pool_ != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(*device_, pool_, nullptr);
            pool_ = VK_NULL_HANDLE;
        }
    }

}