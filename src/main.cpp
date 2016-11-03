#include <iostream>
#include <SDL2/SDL.h>
#include "ngl/Vec3.h"
#include "ngl/NGLStream.h"

int main()
{
    SDL_Window *w = SDL_CreateWindow("test", 100, 100, 100, 100, SDL_WINDOW_SHOWN);
    SDL_Delay(1000);
    ngl::Vec3 a(1, 2, 3);
    std::cout << "hello\n" << a << "\n";
    return 0;
}
