#include <iostream>
#include "Application.hpp"

int main()
{
    VL::Application application;

    try {
        application.run();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Hello World!\n";
    return 0;
}
