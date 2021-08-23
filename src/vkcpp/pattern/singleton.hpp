#ifndef VKCPP_PATTERN_SINGLETON_HPP
#define VKCPP_PATTERN_SINGLETON_HPP

namespace vkcpp
{
    template <typename T>
    class Singleton
    {
    protected:
        inline static T *instance_ = nullptr;

        static bool initInstance()
        {
            if (instance_ != nullptr)
            {
                return false;
            }
            instance_ = new T();
            return true;
        }

    public:
        ~Singleton()
        {
            destroyInstance();
        }
        static void destroyInstance()
        {
            if (instance_ != nullptr)
            {
                delete[] instance_;
                instance_ = nullptr;
            }
        }
        static T *getInstance()
        {
            if (instance_ == nullptr)
            {
                initInstance();
            }
            return instance_;
        }
    };
} // namespace vkcpp

#endif