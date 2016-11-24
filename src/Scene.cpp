#include <iostream>
#include <ngl/ShaderLib.h>
#include <ngl/Transformation.h>
#include <ngl/VAOPrimitives.h>
#include <SDL2/SDL.h>
#include "Scene.hpp"

Scene::Scene() :
    m_character(&m_grid)
{
    SDL_Rect rect;
    SDL_GetDisplayBounds(0, &rect);

    m_cam.setInitPivot( ngl::Vec3(0.0f, 0.0f, 0.0f) );
    m_cam.setInitPos( ngl::Vec3( 0.0f, 0.0f, 10.0f));
    float aspect = (float)rect.w / (float)rect.h;
    m_cam.setAspect( aspect );
    m_cam.setFOV( 120.0f );
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

    ngl::VAOPrimitives * prim = ngl::VAOPrimitives::instance();
    prim->createSphere( "sphere", 1.0f, 32.0f );
}

void Scene::update()
{
    std::cout<<"update\n";
}

void Scene::draw()
{

    m_cam.clearTransforms();
    m_cam.rotateCamera(0.0f, m_temp_counter_delete, 0.0f);

    m_cam.calculateViewMat();

    m_temp_counter_delete += 1.f;

    ngl::Transformation trans;
    trans.setPosition(0.0f, 0.0f, 0.0f);

    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    slib->use("simplesurface");
    //ngl::Mat3 normalMat = MV;
    //normalMat.inverse();

    ngl::Mat4 M = trans.getMatrix();
    ngl::Mat4 MVP = M * m_cam.getVP();

    slib->setRegisteredUniform( "M", M );
    slib->setRegisteredUniform( "MVP", MVP );

    ngl::VAOPrimitives * prim = ngl::VAOPrimitives::instance();
    prim->draw("sphere");
}

bool Scene::isActive()
{
    return m_active;
}
