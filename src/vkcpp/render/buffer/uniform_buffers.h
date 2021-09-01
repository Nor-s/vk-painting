#ifndef VKCPP_RENDER_BUFFER_UNIFORM_BUFFERS_H
#define VKCPP_RENDER_BUFFER_UNIFORM_BUFFERS_H

#include <memory>
#include "buffer.h"
#include "descriptor_sets.h"

namespace vkcpp
{
    class Device;

    class Image;

    template <typename T>
    class UniformBuffers : public DescriptorSets
    {
    private:
        const Image *image_;

        std::vector<Buffer> handle_;

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

#endif // #ifndef VKCPP_BUFFER_UNIFORM_BUFFER_H