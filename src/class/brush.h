#ifndef CLASS_BRUSH_H
#define CLASS_BRUSH_H

#include <vector>
#include "object/object2d.h"

namespace painting
{
    class Brush : public vkcpp::Object2D
    {
    private:
        const char *tex_[4] = {
            "../textures/brushes/1.png",
            "../textures/brushes/4.png",
            "../textures/brushes/6.png",
            "../textures/brushes/9.png"};

    public:
        Brush(const vkcpp::Device *device,
              const vkcpp::RenderStage *render_stage,
              const vkcpp::CommandPool *command_pool,
              int idx);
    };
    struct Palette
    {
        static std::vector<Brush *> brushes_;
        static int brushes_size_;
        static void init_brushes();
        static int get_brushes_size();

        float translate_[3];
        float rotate_;
        float scale_[3];
        float color_[4];
        int idx_;

        Palette();
        void set_translate(float translate[3]);
        void set_scale(float scale[3]);
        void set_rotate(float rotate);
        void set_color(float color[4]);
        void set_brushidx(int idx);
        void draw();
    };
}
#endif
