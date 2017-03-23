#include <SDL.h>
#include <iostream>
#include <memory>
#include <ngl/NGLInit.h>
#include <set>

#include "imgui.h"
#include "ImGUIImpl.h"

#include "Scene.hpp"
#include "Grid.hpp"
#include "Utility.hpp"
#include "Prefs.hpp"
#include "PrefsParser.hpp"

int main()
{

		SDL_Init(SDL_INIT_VIDEO);

    SDL_Event event;

    SDL_Rect rect = {100, 100, 1280, 720};
    //SDL_GetDisplayBounds(0, &rect);

    SDL_Window *window=SDL_CreateWindow("window_test",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        rect.w,
                                        rect.h,
                                        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
                                        );
    SDL_GetWindowSize(window, &rect.w, &rect.h);

    if(window == NULL)
    {
        std::cerr<<"Could not create SDL window \n";
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if(glContext == NULL)
    {
        std::cerr<<"Could not create OpenGL context \n";
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_GL_MakeCurrent(window, glContext);
    SDL_GL_SetSwapInterval(1);

    ngl::NGLInit::instance();


    Prefs* prefs = Prefs::instance();
    prefs->init();
    prefs->printPrefs();




    Scene scene (ngl::Vec2(rect.w, rect.h));

    while(scene.isActive())
    {
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT : scene.quit(); break;
            case SDL_MOUSEBUTTONDOWN : scene.mousePressEvent(event.button); break;
            case SDL_MOUSEBUTTONUP : scene.mouseReleaseEvent(event.button); break;
            case SDL_MOUSEWHEEL : scene.wheelEvent(event.wheel); break;
            case SDL_WINDOWEVENT : scene.windowEvent(event.window); break;
            case SDL_KEYDOWN : scene.keyDownEvent(event.key); break;
            case SDL_KEYUP : scene.keyUpEvent(event.key); break;

            default : break;
            }
        }
        scene.updateMousePos();

        scene.update();
        scene.draw();

        SDL_GL_SwapWindow(window);

    }
    //for when we want to start saving preferences
    //p->save();
    SDL_Quit();
    return EXIT_SUCCESS;

}
