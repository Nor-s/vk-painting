#include "class/application.h"

int main()
{
    painting::PaintingApplication app;

    try
    {
        app.run(1024, 512);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}