#ifndef VKCPP_RENDER_DESCRIPTOR_DESCRIPTOR_SET_H
#define VKCPP_RENDER_DESCRIPTOR_DESCRIPTOR_SET_H

#include "vulkan_header.h"
#include <vector>

namespace vkcpp
{
    class Device;

    class DescriptorSet
    {
    public:
        struct DescriptorSetLayoutBindingAttribute
        {
            uint32_t binding;
            VkDescriptorType descriptor_type;
            uint32_t descriptor_count;
            VkShaderStageFlags stage_flags;
            const VkSampler *immutable_samplers;
        }; // struct

    private:
        const Device *device_{nullptr};

        uint32_t layout_bindings_size_{0};

        std::vector<DescriptorSetLayoutBindingAttribute> layout_binding_attributes;

        std::vector<VkDescriptorSetLayoutBinding> layout_bindings_;

        VkDescriptorSetLayout layout_{VK_NULL_HANDLE};

        VkDescriptorSet handle_{VK_NULL_HANDLE};

    public:
        void init_layout_bindings();

        void init_layout();

        void destroy_layout();
    }; // class

} // namespace vkcpp

#endif // #define VKCPP_RENDER_DESCRIPTOR_DESCRIPTOR_SET_H