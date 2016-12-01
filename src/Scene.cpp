#include <iostream>
#include <ngl/ShaderLib.h>
#include <ngl/Transformation.h>
#include <ngl/VAOPrimitives.h>
#include <SDL2/SDL.h>

#include "imgui.h"

#include "Scene.hpp"

Scene::Scene() :
		m_character(&m_grid),
		m_mouse_trans_active(false),
		m_mouse_zoom(10.0f),
		m_mouse_pan(1.0f),
		m_mouse_rotation(0.0f),
		m_mouse_translation(0.0f,0.0f)
{
		SDL_Rect rect;
		SDL_GetDisplayBounds(0, &rect);

		m_cam.setInitPivot( ngl::Vec3(0.0f, 0.0f, 0.0f));
		m_cam.setInitPos( ngl::Vec3( 0.0f, 1.0f, m_mouse_zoom));
		m_cam.setUp (ngl::Vec3(0.0f,1.0f,0.0f));
		float aspect = (float)rect.w / (float)rect.h;
		m_cam.setAspect( aspect );
		m_cam.setFOV( 60.0f );
		m_cam.calculateProjectionMat();
		m_cam.calculateViewMat();

		ngl::ShaderLib * slib = ngl::ShaderLib::instance();

		const std::string name = "simplesurface";
		const std::string vert = "vertMVPUVN";
		const std::string frag = "fragBasicLight";

		slib->createShaderProgram( name );
		slib->attachShader(vert, ngl::ShaderType::VERTEX);
		slib->attachShader(frag, ngl::ShaderType::FRAGMENT);

		slib->loadShaderSource(vert, "shaders/" + vert + ".glsl");
		slib->loadShaderSource(frag, "shaders/" + frag + ".glsl");

		slib->compileShader(vert);
		slib->compileShader(frag);

		slib->attachShaderToProgram(name, vert);
		slib->attachShaderToProgram(name, frag);

		slib->linkProgramObject(name);

		ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();

		prim->createTrianglePlane("plane",14,14,80,80,ngl::Vec3(0,1,0));
}

void Scene::draw()
{
		m_cam.setInitPos(ngl::Vec3(0.0f, 1.0f, m_mouse_zoom));

		m_cam.clearTransforms();

		ngl::Vec3 trans (m_mouse_translation[0] * 0.1, 0.0f, m_mouse_translation[1] *  0.1);

		m_cam.rotatePivot(m_mouse_pan, m_mouse_rotation, 0.0f);

		m_cam.moveCamera(trans);

		m_cam.calculateViewMat();

		ngl::Transformation m_transform;
		m_transform.setPosition(0.0f, 0.0f, 0.0f);

		ngl::ShaderLib * slib = ngl::ShaderLib::instance();

		slib->use("simplesurface");

		ngl::VAOPrimitives * prim = ngl::VAOPrimitives::instance();

		 m_transform.reset();
		 {
			 m_transform.setPosition(0.0,-0.5,0.0);
			 ngl::Mat4 M = m_transform.getMatrix();
			 ngl::Mat4 MVP = M * m_cam.getVP();
			 slib->setRegisteredUniform( "M", M );
			 slib->setRegisteredUniform( "MVP", MVP );
			 prim->draw("plane");
		 }
}

void Scene::mousePressEvent(const SDL_MouseButtonEvent &_event)
{
	//checks if the left button has been pressed down and flags start
	if(_event.button == SDL_BUTTON_LEFT)
	{
		m_mouse_trans_origin[0] = _event.x;
		m_mouse_trans_origin[1] = _event.y;
		m_mouse_trans_active=true;
	}

	//checks if the right button has been pressed down and flags start
	else if(_event.button == SDL_BUTTON_RIGHT)
	{
		m_mouse_rot_origin = _event.x;
		m_mouse_rot_active = true;
	}
}

void Scene::mouseMoveEvent(const SDL_MouseMotionEvent &_event)
{
	//translates
	if(m_mouse_trans_active && _event.state &SDL_BUTTON_LEFT)
	{
		ngl::Vec2 mouse_distance;
		mouse_distance[0] =_event.x-m_mouse_trans_origin[0];
		mouse_distance[1] =_event.y-m_mouse_trans_origin[1];
		m_mouse_translation[0] += (float) 0.1f * mouse_distance[0];
		m_mouse_translation[1] += (float) 0.1f * mouse_distance[1];
		m_mouse_trans_origin[0] = _event.x;
		m_mouse_trans_origin[1] = _event.y;
		this->draw();
	}
	//rotates
	else if(m_mouse_rot_active && _event.state &SDL_BUTTON_RMASK)
	{
		float mouse_distance;
		mouse_distance =_event.x-m_mouse_rot_origin;
		m_mouse_rotation += (float) 0.075f * mouse_distance;
		m_mouse_rot_origin = _event.x;
		this->draw();
	}

}

void Scene::mouseReleaseEvent (const SDL_MouseButtonEvent &_event)
{
	//checks if the left button has been released and turns off flag
	if (_event.button == SDL_BUTTON_LEFT)
	{
		m_mouse_trans_active = false;
	}

	//checks if the right button has been released and turns off flag
	if (_event.button == SDL_BUTTON_RIGHT)
	{
		m_mouse_rot_active = false;
	}

}

void Scene::wheelEvent(const SDL_MouseWheelEvent &_event)
{
	//pans camera up and down
	if(_event.y > 0 && m_mouse_zoom > 10)
	{
		if (m_mouse_pan > -5)
		{
			m_mouse_pan -= 0.5;
		}
		m_mouse_zoom -= 0.5;
		this->draw();
	}

	else if(_event.y < 0 && m_mouse_zoom < 20)
	{
		if(m_mouse_pan < 10)
		{
			m_mouse_pan += 0.5;
		}
		m_mouse_zoom += 0.5;
		this->draw();
	}

}
