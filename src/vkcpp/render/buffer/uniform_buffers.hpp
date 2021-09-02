#ifndef VKCPP_RENDER_BUFFER_UNIFORM_BUFFERS_HPP
#define VKCPP_RENDER_BUFFER_UNIFORM_BUFFERS_HPP

#include "buffer.hpp"
#include "descriptor_sets.h"
#include "device/device.h"
#include "descriptor_sets.h"
#include "render/image/image.h"

#include <memory>

namespace vkcpp
{
    template <typename T>
    class UniformBuffers : public DescriptorSets
    {
    private:
        const Image *image_;

        std::vector<Buffer<T>> handle_;

    public:
        UniformBuffers() = delete;

        UniformBuffers(const Device *device_, const Image *image, uint32_t size);

        virtual ~UniformBuffers();

        void init_uniform_buffers();

        void destroy_uniform_buffers();

        void update_uniform_buffer(uint32_t idx, const T &src_data);

        void update_descriptor();
    };

} // namespace vkcpp

#include "uniform_buffers.tpp"

#endif // #ifndef VKCPP_BUFFER_UNIFORM_BUFFER_H