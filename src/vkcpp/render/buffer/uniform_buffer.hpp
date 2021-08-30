#ifndef VKCPP_RENDER_BUFFER_UNIFORM_BUFFERS_H
#define VKCPP_RENDER_BUFFER_UNIFORM_BUFFERS_H

#include "device/device.h"
#include "buffer.h"
#include "vertex.hpp"
#include <memory>
#include <chrono>
#include "descriptor_set.h"

namespace vkcpp
{
    template <typename T>
    class UniformBuffers : public DescriptorSet
    {
    private:
        std::vector<std::uniq_ptr<Buffer<T>>> handle_;

    public:
        UniformBuffers() = delete ();

        UniformBuffers(const Device *device_, uint32_t size)
            : DescriptorSet(device_, size)
        {
            init_uniform_buffers();
        }
        ~UniformBuffers()
        {
            destroy_uniform_buffers();
        }

        void init_uniform_buffers()
        {
            if (handle_.size() != 0)
            {
                destory_uniform_buffers();
            }
            for (int i = 0; i < size_; i++)
            {
                handle_.push_back(
                    std::make_unique(
                        device_,
                        nullptr,
                        nullptr,
                        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        false));
            }
        }
        void destroy_uniform_buffers()
        {
            for (auto &ubo : handle_)
            {
                ubo.reset();
            }
            handle_.resize(0);
            size_ = 0;
        }

        void update_uniform_buffer(uint32_t idx, const T &src_data)
        {
            if (idx >= size_)
            {
                throw std::runtime_error("failed to update ubo! out of bounds");
            }
            void *dst_data;
            vkMapMemory(*device_, handle_[idx], 0, sizeof(T), 0, &dst_data);
            memcpy(*dst_data, &src_data, sizeof(src_data));
            vkUnmapMemory(*device_, handle_[idx]);
        }
        void update_descriptor()
        {
            for (size_t i = 0; i < size_; i++)
            {
                VkDescriptorBufferInfo buffer_info{};
                buffer_info.buffer = *handle_[i];
                buffer_info.offset = 0;
                buffer_info.range = sizeof(T); // or VK_WHOLE_SIZE

                VkDescriptorImageInfo image_info{};
                image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                image_info.imageView = textureImageView;
                image_info.sampler = textureSampler;

                std::array<VkWriteDescriptorSet, 2> descriptor_writes{};

                descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptor_writes[0].dstSet = descriptor_sets_[i];
                descriptor_writes[0].dstBinding = 0;
                descriptor_writes[0].dstArrayElement = 0;
                descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptor_writes[0].descriptorCount = 1;
                descriptor_writes[0].pBufferInfo = &buffer_info;
                descriptor_write[0].pImageInfo = nullptr;       // Optional
                descriptor_write[0].pTexelBufferView = nullptr; // Optional

                descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptor_writes[1].dstSet = descriptor_sets_[i];
                descriptor_writes[1].dstBinding = 1;
                descriptor_writes[1].dstArrayElement = 0;
                descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptor_writes[1].descriptorCount = 1;
                descriptor_writes[0].pBufferInfo = nullptr; // Optional
                descriptor_writes[1].pImageInfo = &image_info;
                descriptor_write[0].pTexelBufferView = nullptr; // Optional

                vkUpdateDescriptorSets(*device_, static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);
            }
        }
    };

} // namespace vkcpp

#endif // #ifndef VKCPP_BUFFER_UNIFORM_BUFFER_H