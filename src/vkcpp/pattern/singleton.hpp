#ifndef VKCPP_SINGLETON_HPP
#define VKCPP_SINGLETON_HPP

namespace vkcpp
{
    template <typename T>
    class Singleton
    {
    protected:
        inline static T *instance_ = nullptr;

        static bool init_instance()
        {
            if (instance_)
            {
                return false;
            }
            instance_ = new T();
            return true;
        }

    public:
        ~Singleton()
        {
            destroy_instance();
        }
        static void destroy_instance()
        {
            if (instance_)
            {
                delete instance_;
                instance_ = nullptr;
            }
        }
        static T *get()
        {
            if (instance_ == nullptr)
            {
                init_instance();
            }
            return instance_;
        }
    };
} // namespace vkcpp

#endif