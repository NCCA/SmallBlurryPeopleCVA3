#include <ngl/ShaderLib.h>

#include "Renderer.hpp"
#include "Util.hpp"

Renderer::Renderer()
{
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
			Util::errorExit("SDL initialisation failed");

	SDL_DisplayMode best;
	best.w = 0;
	best.h = 0;

	for(int i = 0; i < SDL_GetNumVideoDisplays(); ++i)
	{
			SDL_DisplayMode temp;
			SDL_GetCurrentDisplayMode(i, &temp);
			if(temp.w * temp.h > best.w * best.h)
					best = temp;
	}

	m_w = best.w;
	m_h = best.h;


	m_window = SDL_CreateWindow("All The Small Things",
															0, 0,
															m_w, m_h,
															SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS );

	if(!m_window)
			Util::errorExit("Unable to create window");

	//Setting up the GL attributes before they can be used
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	m_gl_context = SDL_GL_CreateContext( m_window );

	if(!m_gl_context)
			Util::errorExit("Unable to create GL context");

	makeCurrent();
	SDL_GL_SetSwapInterval(0);

	ngl::NGLInit::instance();

	createShaderProgram( "blinn", "MVPUVNVert", "blinnFrag" );
	createShaderProgram( "bufferLight", "screenquadVert", "bufferLightFrag" );

	m_framebuffer.initialise(m_w, m_h);

	m_framebuffer.addTexture( "diffuse", GL_RGBA, GL_RGBA, GL_COLOR_ATTACHMENT0 );
	m_framebuffer.addTexture( "normal", GL_RGBA, GL_RGBA16F, GL_COLOR_ATTACHMENT1 );
	m_framebuffer.addTexture( "position", GL_RGBA, GL_RGBA16F, GL_COLOR_ATTACHMENT2 );

	m_framebuffer.addDepthAttachment("depth");

	m_framebuffer.activeColourAttachments(
	{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3}
							);

	if(!m_framebuffer.checkComplete())
			Util::errorExit( "Error! Framebuffer failed!\n" );

	m_framebuffer.unbind();

	glViewport(0, 0, m_w, m_h);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	clear();
	swap();
	hide();
}

void Renderer::createShaderProgram(const std::string _name, const std::string _vert, const std::string _frag)
{
		ngl::ShaderLib * slib = ngl::ShaderLib::instance();

		slib->createShaderProgram(_name);
		slib->attachShader(_vert, ngl::ShaderType::VERTEX);
		slib->attachShader(_frag, ngl::ShaderType::FRAGMENT);

		slib->loadShaderSource(_vert, "shaders/" + _vert + ".glsl");
		slib->loadShaderSource(_frag, "shaders/" + _frag + ".glsl");

		slib->compileShader(_vert);
		slib->compileShader(_frag);

		slib->attachShaderToProgram(_name, _vert);
		slib->attachShaderToProgram(_name, _frag);

		slib->linkProgramObject(_name);
}
