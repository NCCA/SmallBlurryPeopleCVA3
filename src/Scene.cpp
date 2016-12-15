#include <iostream>
#include <ngl/ShaderLib.h>
#include <ngl/Transformation.h>
#include <ngl/VAOPrimitives.h>
#include <SDL2/SDL.h>

#include "imgui.h"

#include "Scene.hpp"

Scene::Scene() :
    m_mouse_trans_active(false),
    m_mouse_rot_active(false),
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

		/////////////////////////////////////////////////////////

		m_characters.push_back(Character(&m_grid));
		m_characters.push_back(Character(&m_grid));

		character_names = {"Paul", "Susan"};

		std::unordered_map<std::string, int> m_char_map;

		for (size_t i = 0; i<m_characters.size(); i++)
		{
			m_char_map[character_names[i]] =  m_characters[i].getID();
			std::cout<<character_names[i]<<" :name, "<< m_char_map[character_names[i]]<<": ID\n";
		}




	////////////////////////////////////////////////////////////////

    ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();

    prim->createTrianglePlane("plane",14,14,80,80,ngl::Vec3(0,1,0));
    prim->createSphere( "sphere", 0.1, 12 );
}

void Scene::update()
{
    //If this is not zeroed first, the camera will keep sliding about after we release the lmb.
    m_mouse_translation = ngl::Vec2(0,0);

    //translates
    if(m_mouse_trans_active)
    {
        //Get mouse pos.
        int mousePos[2] = {0, 0};
        SDL_GetMouseState( &mousePos[0], &mousePos[1] );

        //Compute distance to mouse origin
        ngl::Vec2 mouse_distance (mousePos[0], mousePos[1]);
        mouse_distance -= m_mouse_trans_origin;

        m_mouse_translation = mouse_distance;
        m_mouse_trans_origin = ngl::Vec2( mousePos[0], mousePos[1] );
    }
    //rotates
    else if(m_mouse_rot_active)
    {
        int mouse_origin = 0;
        int mouse_distance = 0;
        SDL_GetMouseState(&mouse_origin, nullptr);
        mouse_distance = mouse_origin - m_mouse_rot_origin;
        m_mouse_rotation += (float) 0.075f * mouse_distance;
        m_mouse_rot_origin = mouse_origin;
    }

    //Set initial position (should really make this function more intuitive).
    m_cam.setInitPos(ngl::Vec3(0.0f, 0.0f, m_mouse_zoom));

    //Clear all transformations from previous update.
    m_cam.clearTransforms();

    //Construct translation *change* using right and forward vectors.
    ngl::Vec3 trans = m_cam.right() * m_mouse_translation.m_x * 0.05f;
    trans += m_cam.forwards() * -m_mouse_translation.m_y * 0.05f;
    trans -= m_cam.getPivot();
    trans.m_y = 0.0f;

    m_cam.rotateCamera(m_mouse_pan, m_mouse_rotation, 0.0f);

    m_cam.movePivot( trans );

    m_cam.calculateViewMat();
}

void Scene::draw()
{
    m_transform.setPosition(0.0f, 0.0f, 0.0f);

    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    slib->use("simplesurface");

    ngl::VAOPrimitives * prim = ngl::VAOPrimitives::instance();

    m_transform.reset();
    {
        m_transform.setPosition(0.0,-0.5,0.0);
        loadMatricesToShader();
        prim->draw("plane");
    }

    for(int i = -10; i < 10; ++i)
    {
        for(int j = -10; j < 10; ++j)
        {
            for(int k = -10; k < 10; ++k)
            {
                m_transform.setPosition(i, j, k);
                loadMatricesToShader();
                prim->draw("sphere");
            }
        }
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
    }

    else if(_event.y < 0 && m_mouse_zoom < 20)
    {
        if(m_mouse_pan < 10)
        {
            m_mouse_pan += 0.5;
        }
        m_mouse_zoom += 0.5;
    }

}

void Scene::loadMatricesToShader()
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();
    ngl::Mat4 M = m_transform.getMatrix();
    ngl::Mat4 MVP = M * m_cam.getVP();
    slib->setRegisteredUniform( "M", M );
    slib->setRegisteredUniform( "MVP", MVP );
}
