#include <iostream>
#include <ngl/NGLStream.h>
#include <ngl/ShaderLib.h>
#include <ngl/Transformation.h>
#include <ngl/VAOPrimitives.h>
#include <SDL2/SDL.h>

#include "imgui.h"

#include "Scene.hpp"

Scene::Scene() :
    m_character(&m_grid),
    m_mouse_trans_active(false),
    m_mouse_rot_active(false),
    m_mouse_zoom(10.0f),
    m_mouse_pan(1.0f),
    m_mouse_rotation(0.0f),
    m_mouse_translation(0.0f,0.0f),
    m_temp_debug_counter(0.0f)
{
    SDL_Rect rect;
    SDL_GetDisplayBounds(0, &rect);

    m_cam.setInitPivot( ngl::Vec3(0.0f, 0.0f, 0.0f));
    m_cam.setInitPos( ngl::Vec3( 0.0f, 0.5f, m_mouse_zoom));
    m_cam.setUp (ngl::Vec3(0.0f,1.0f,0.0f));

    float aspect = (float)rect.w / (float)rect.h;
    m_cam.setAspect( aspect );
    m_cam.setFOV( 45.0f );
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

    prim->createTrianglePlane("plane",30,30,80,80,ngl::Vec3(0,1,0));
    prim->createSphere("sphere",0.5f,50);
}

void Scene::draw()
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    slib->use("simplesurface");

    ngl::VAOPrimitives * prim = ngl::VAOPrimitives::instance();

    for(int i = -10; i < 10; ++i)
    {
        for(int j = -10; j < 10; ++j)
        {
            m_transform.setPosition(ngl::Vec3(i * 10, 0.0f, j * 10));
            loadMatricesToShader();
            prim->draw("sphere");
        }
    }

    m_transform.setPosition(m_cam.getPivot());
    loadMatricesToShader();
    prim->draw("sphere");
}

void Scene::update()
{
    if(m_mouse_trans_active)
    {
        int mousePos[2] = {0, 0};
        SDL_GetMouseState( &mousePos[0], &mousePos[1] );
        std::cout << "mousex " << mousePos[0] << ", " << mousePos[1] << '\n';
        ngl::Vec2 diff = m_mouse_trans_origin - ngl::Vec2(mousePos[0], mousePos[1]);

        m_mouse_translation += diff;

        //Make origin equal to cur pos.
        m_mouse_trans_origin = ngl::Vec2(mousePos[0], mousePos[1]);
    }
    else if(m_mouse_rot_active)
    {
        int mouseX = 0;
        SDL_GetMouseState(&mouseX, nullptr);
        float mouse_distance =m_mouse_rot_origin - mouseX;
        m_mouse_rotation -= mouse_distance / 32.0f;
        m_mouse_rot_origin = mouseX;
    }


    std::cout << "updating!\n";
    m_temp_debug_counter += 0.1f;

    m_cam.clearTransforms();

    m_cam.setInitPos(ngl::Vec3(0.0f, 0.0f, m_mouse_zoom));

    ngl::Vec3 right_move = (m_cam.right() * m_mouse_translation.m_x);
    ngl::Vec3 forwards_move (m_cam.back() * m_mouse_translation.m_y);
    ngl::Vec3 combined_move = right_move + forwards_move;
    combined_move.m_y = 0.0f;
    combined_move *= 0.1f;

    m_cam.movePivot( ngl::Vec3(100,0,0) );
    m_cam.rotateCamera( m_mouse_pan, m_mouse_rotation, 0.0f );
    m_cam.calculateViewMat();
}

void Scene::loadMatricesToShader()
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();
    ngl::Mat4 M = m_transform.getMatrix();
    ngl::Mat4 MVP = M * m_cam.getVP();
    slib->setRegisteredUniform( "M", M );
    slib->setRegisteredUniform( "MVP", MVP );
}

void Scene::mousePressEvent(const SDL_MouseButtonEvent &_event)
{
    //checks if the left button has been pressed down and flags start
    if(_event.button == SDL_BUTTON_LEFT)
    {
        m_mouse_trans_origin.m_x = _event.x;
        m_mouse_trans_origin.m_y = _event.y;
        m_mouse_trans_active=true;
    }

    //checks if the right button has been pressed down and flags start
    if(_event.button == SDL_BUTTON_RIGHT)
    {
        m_mouse_rot_origin = _event.x;
        m_mouse_rot_active = true;
    }
}

void Scene::mouseMoveEvent(const SDL_MouseMotionEvent &_event)
{
    return;
    std::cout << "moving!\n";
    //translates
    if(m_mouse_trans_active && _event.state &SDL_BUTTON_LEFT)
    {
        //Get difference between current mouse position and click origin.
        ngl::Vec2 mouse_distance (
                    _event.x - m_mouse_trans_origin.m_x,
                    _event.y - m_mouse_trans_origin.m_y
                    );

        //Add difference to mouse translation.
        m_mouse_translation += mouse_distance;

        //Make origin equal to cur pos.
        m_mouse_trans_origin = ngl::Vec2( (float)_event.x, (float)_event.y );
    }
    //rotates
    if(m_mouse_rot_active && _event.state &SDL_BUTTON_RMASK)
    {
        float mouse_distance;
        mouse_distance =_event.x - m_mouse_rot_origin;
        m_mouse_rotation += mouse_distance;
        m_mouse_rot_origin = _event.x;
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
        if (m_mouse_pan > 0)
        {
            m_mouse_pan -= 0.5;
        }
        m_mouse_zoom -= 0.5;
    }

    else if(_event.y < 0 && m_mouse_zoom < 30)
    {
        if(m_mouse_pan < 10)
        {
            m_mouse_pan += 0.5;
        }
        m_mouse_zoom += 0.5;
    }

}
