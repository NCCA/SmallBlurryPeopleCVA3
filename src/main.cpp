#include <SDL.h>
#include <iostream>
#include <ngl/NGLInit.h>

#include "Scene.hpp"

int main()
{
		SDL_Init(SDL_INIT_VIDEO);

		SDL_Event event;
		Scene scene;

		bool quit = false;

		SDL_Rect rect;
		SDL_GetDisplayBounds(0,&rect);

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
		glClearColor(0.5,0.5,0.5,1);
		glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(window);

		while(!quit)
		{
			while(SDL_PollEvent(&event))
			{
				switch(event.type)
				{
					case SDL_QUIT : quit = true; break;
					//case SDL_MOUSEBUTTONDOWN : scene.mousePressEvent(); break;
					default : break;
				}
			}
			SDL_GL_SwapWindow(window);
		}

		SDL_Quit();
		return EXIT_SUCCESS;

		/*
		while(scene.isActive())
		{
			scene.update();
			scene.draw();
		}
		*/
		//return 0;
}
