#ifndef VKCPP_RENDER_IMAGE_OFFSCREEN_H
#define VKCPP_RENDER_IMAGE_OFFSCREEN_H

#include "image.h"
#include <utility>

namespace vkcpp
{
    class Offscreen : public Image
    {
    private:
    public:
        explicit Offscreen(const Device *device, const VkExtent3D &extent, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

        virtual ~Offscreen() = default;
        void init_offscreen_image();

        void init_offscreen_view();
    };

} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_IMAGE_IMAGE_OFFSCREEN_H