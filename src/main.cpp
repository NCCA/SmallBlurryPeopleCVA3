#include <iostream>
#include <SDL2/SDL.h>
#include "ngl/Vec3.h"

int main()
{
    SDL_Window *w = SDL_CreateWindow("test", 100, 100, 100, 100, SDL_WINDOW_SHOWN);
    SDL_Delay(1000);
    std::cout << "hello\n";
    return 0;
}
