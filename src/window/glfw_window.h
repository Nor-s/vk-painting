#ifndef VKCPP_GLFW_WINDOW_H
#define VKCPP_GLFW_WINDOW_H

#include "window.hpp"
#include "pattern/singleton.hpp"

struct GLFWwindow;
namespace vkcpp
{
    class GlfwWindow : public Window, public Singleton<GlfwWindow>
    {
        inline static const bool is_instanced = init_instance();

    private:
        GLFWwindow *handle_ = nullptr;

    public:
        GlfwWindow();
        virtual ~GlfwWindow();
        virtual VkSurfaceKHR create_surface(VkInstance &instance) override;
        virtual bool should_close() override;
        virtual void close() override;
        virtual void process_events() override;
        void init_window(uint32_t width, uint32_t height, std::string &title);
        void init_glfw();
        void destroy_window();

        std::pair<const char **, uint32_t> get_required_instance_extensions() const;

        virtual void set_window(uint32_t width, uint32_t height, const std::string &title) override;
        virtual void set_size(uint32_t width, uint32_t height) override;
        virtual void set_title(const std::string &title) override;
    };
} // namespace vkcpp
#endif // #ifndef VKCPP_GLFW_WINDOW_H