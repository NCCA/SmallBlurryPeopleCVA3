#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL2/SDL.h>
#include <ngl/NGLInit.h>

#include "Framebuffer.hpp"

class Renderer
{
public:
		Renderer();
		void clear() {glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);}
		void hide() {SDL_HideWindow( m_window );}
		void makeCurrent() { SDL_GL_MakeCurrent(m_window, m_gl_context); }
		void swap() {SDL_GL_SwapWindow(m_window);}
private:
		void createShaderProgram(const std::string _name, const std::string _vert, const std::string _frag);

		Framebuffer m_framebuffer;
		SDL_GLContext m_gl_context;
		int m_h;
		int m_w;
		SDL_Window * m_window;
};

#endif
