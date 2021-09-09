#include "class/brush.h"

#include <iostream>
namespace painting
{
    Brush::Brush(const vkcpp::Device *device,
                 const vkcpp::RenderStage *render_stage,
                 const vkcpp::CommandPool *command_pool,
                 int idx)
        : vkcpp::Object2D(device, render_stage, command_pool)
    {
        texture_file_ = tex_[idx];
        init_texture({});
        init_object2d();
        init_transform({0.0, 0.0, 1.0}, {0.1, 0.1, 1.0});
        init_color({0.0, 0.0, 0.0, .4});
    }
}

namespace painting
{
    std::vector<Brush *> Palette::brushes_;
    int Palette::brushes_size_ = 0;
    Palette::Palette() {}
    void Palette::init_brushes()
    {
        if (Palette::brushes_size_ == 0)
        {
            for (int i = 0; i < 4; i++)
            {
                //              Palette::brushes_.push_back(new Brush(i));
                //              Palette::brushes_size_++;
            }
        }
    }
    int Palette::get_brushes_size()
    {
        return brushes_size_;
    }
    void Palette::set_translate(float translate[3])
    {
        for (int i = 0; i < 3; i++)
        {
            translate_[i] = translate[i];
        }
    }
    void Palette::set_scale(float scale[3])
    {
        for (int i = 0; i < 3; i++)
        {
            scale_[i] = scale[i];
        }
    }
    void Palette::set_rotate(float rotate)
    {
        rotate_ = rotate;
    }
    void Palette::set_color(float color[4])
    {
        for (int i = 0; i < 4; i++)
        {
            color_[i] = color[i];
        }
    }
    void Palette::set_brushidx(int idx)
    {
        idx_ = idx;
    }
    void Palette::draw()
    {
        /*
        auto p = &brushes_[idx_];
        p->init_model();
        p->translate(translate_);
        p->rotate(rotate_);
        p->scale(scale_);
        p->scale(0.4);
        p->set_color_to_uniform(color_);
        p->set_model_to_uniform();
        p->draw();
        */
    }
}