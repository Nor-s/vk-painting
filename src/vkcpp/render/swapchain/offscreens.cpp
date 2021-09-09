#include "offscreens.h"
#include "render/image/image_depth.h"
#include "device/device.h"

namespace vkcpp
{

    Offscreens::Offscreens(const Device *device, const VkExtent3D &extent, uint32_t size)
        : device_(device), extent_(extent), size_(size)
    {
        init_offscreens();
        init_depth();
    }

    Offscreens::~Offscreens()
    {
        destroy_offscreens();
    }

    void Offscreens::init_offscreens()
    {
        for (int i = 0; i < size_; i++)
        {
            offscreens_.push_back(std::make_unique<Offscreen>(device_, extent_));
        }
    }
    void Offscreens::init_depth()
    {
        for (int i = 0; i < size_; i++)
        {
            depth_.push_back(std::make_unique<ImageDepth>(device_, nullptr, extent_));
        }
    }
    void Offscreens::destroy_offscreens()
    {
        for (int i = 0; i < size_; i++)
        {
            depth_[i].reset();
        }
        depth_.resize(0);
        for (auto &offscreen : offscreens_)
        {
            offscreen.reset();
        }
        size_ = 0;
        offscreens_.resize(0);
    }

} // namespace vkcpp
