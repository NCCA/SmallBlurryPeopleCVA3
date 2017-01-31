#include <SDL.h>
#include <iostream>
#include <ngl/NGLInit.h>

#include "imgui.h"
#include "ImGUIImpl.h"

#include "Scene.hpp"
#include "Grid.hpp"
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

    while(!quit)
    {
        while(SDL_PollEvent(&event))
        {
            ImGuiImplSdlProcessEvent(&event);
            ImGuiImplSdlNewFrame(window);

            ImGui::Begin("Main Menu", 0, {128, 300}, 0.0f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);

            ImGui::SetCursorPosY(0);

            /*const char* names[scene.character_names.size()];
                        for (size_t i = 0; i< scene.character_names.size(); i++)
                        {
                            names[i]= scene.character_names[i].c_str();
                        }
                        static int current_character_name = -1;
                        //current_character_name stores current element of char *array that has been selected
                        bool character_chosen = ImGui::Combo("character_select", &current_character_name, names, (sizeof(names)/sizeof(char*)));

            ImGui::SetCursorPosY( 64 );
            bool savePressed = ImGui::Button("Save", {128, 64});
            ImGui::SetCursorPosY( 128 );
            bool loadPressed = ImGui::Button("Load", {128, 64});
                        ImGui::SetCursorPossY( 186 );
            bool quitPressed = ImGui::Button("Quit", {128, 64});


            ImGui::Begin("Character stats", 0, {128, 300}, 0.0f, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetCursorPosY(20);
            ImGui::TextColored({1.0f, 1.0f, 1.0f, 1.0f},"hunger");

            ImGui::SetCursorPosY(40);
            float hunger = 0.1;
            ImGui::ProgressBar(hunger);

                        if(character_chosen)
                        {

                        }
                        if(quitPressed)
                                quit = true;
                        */

            ImGui::End();

            if(!io.WantCaptureMouse)
            {
                switch(event.type)
                {
                case SDL_QUIT : quit = true; break;
                case SDL_MOUSEBUTTONDOWN : scene.mousePressEvent(event.button); break;
                case SDL_MOUSEBUTTONUP : scene.mouseReleaseEvent(event.button); break;
                case SDL_MOUSEWHEEL : scene.wheelEvent(event.wheel); break;
                default : break;
                }
            }
        }

        scene.update();
        scene.draw();
        ImGui::Render();

        SDL_GL_SwapWindow(window);

    }
    ImGuiImplSdlShutdown();

    SDL_Quit();
    return EXIT_SUCCESS;

}
