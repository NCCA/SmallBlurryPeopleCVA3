#include <SDL.h>
#include <iostream>
#include <ngl/NGLInit.h>

#include "imgui.h"
#include "ImGUIImpl.h"

#include "Scene.hpp"

int main()
{


    SDL_Init(SDL_INIT_VIDEO);

    SDL_Event event;

    bool quit = false;

    SDL_Rect rect;
    SDL_GetDisplayBounds(0, &rect);

    SDL_Window *window=SDL_CreateWindow("window_test",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        rect.w,
                                        rect.h,
                                        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
                                        );
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

    // Setup ImGui binding
    ImGuiImplSdlInit(window);
    ImGuiIO& io = ImGui::GetIO();
		Scene scene;

		glViewport(0, 0, rect.w, rect.h);
		glClearColor(0.5,0.5,0.5,1);


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(window);

    while(!quit)
    {
        while(SDL_PollEvent(&event))
        {
            ImGuiImplSdlProcessEvent(&event);
            ImGuiImplSdlNewFrame(window);

            ImGui::Begin("Main Menu", 0, {256, 128}, 0.0f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);

            bool quitPressed = ImGui::Button("Quit", {256, 128});
            if(quitPressed)
                quit = true;

            ImGui::End();

            if(!io.WantCaptureMouse)
            {
                switch(event.type)
                {
                case SDL_QUIT : quit = true; break;
								case SDL_MOUSEMOTION : scene.mouseMoveEvent(event.motion); break;
								case SDL_MOUSEBUTTONDOWN : scene.mousePressEvent(event.button); break;
								case SDL_MOUSEBUTTONUP : scene.mouseReleaseEvent(event.button); break;
								case SDL_MOUSEWHEEL : scene.wheelEvent(event.wheel); break;
                default : break;
                }
            }
        }

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			scene.draw();
			ImGui::Render();

			SDL_GL_SwapWindow(window);

		}
    ImGuiImplSdlShutdown();

    SDL_Quit();
    return EXIT_SUCCESS;

}
