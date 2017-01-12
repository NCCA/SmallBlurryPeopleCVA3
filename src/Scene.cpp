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

    createShader("simplesurface", "vertMVPUVN", "fragBasicLight");
    createShader("blinn", "vertMVPUVN", "fragBlinn" );

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

    m_store.loadMesh("knight", "knight/knight.obj");
    m_store.loadTexture("knight_d", "knight/knight_d.png");

    //Get as vec4s
    std::vector<ngl::Vec4> data;
    std::vector<ngl::Vec3> original = m_grid.getTriangles();
    data.reserve(original.size());
    //Convert format, add some cheeky randomisation
    for(auto &vert : original)
        data.push_back( ngl::Vec4(vert.m_x, vert.m_y + 0.5f * (sin(vert.m_x) + cos(vert.m_z)), vert.m_z, 1.0f) );
    m_terrainVAO = createVAO( data );
    m_terrainVAOSize = data.size();
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

    /*m_transform.reset();
    m_transform.setPosition(0.0,-0.5,0.0);
    loadMatricesToShader();
    prim->draw("plane");*/
    m_transform.reset();
    glBindVertexArray(m_terrainVAO);
    loadMatricesToShader();
    std::cout << "Drawing " << m_terrainVAOSize << '\n';
    glDrawArraysEXT(GL_TRIANGLES, 0, m_terrainVAOSize);
    glBindVertexArray(0);

    for(int i = 0; i < 20; ++i)
        for(int j = 0; j < 20; ++j)
        {
            m_transform.setPosition(i * 1.5 - 15, 0, j - 10);
            //slib->use("blinn");
            loadMatricesToShader();
            drawAsset( "knight", "knight_d", "blinn" );
            //m_store.getModel("knight")->draw();
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

void Scene::bindTextureToShader(const std::string &_shaderID, const GLuint _tex, const char *_uniform, int _target)
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();
    GLint spid = slib->getProgramID( _shaderID );
    GLint loc = glGetUniformLocation(spid, _uniform);

    if(loc == -1)
    {
        std::cerr << "Uh oh! Invalid uniform location in Scene::bindTextureToShader!! " << _uniform << '\n';
        return;
    }
    glUniform1i(loc, _target);

    glActiveTexture(GL_TEXTURE0+_target);
    glBindTexture(GL_TEXTURE_2D, _tex);
}

void Scene::drawAsset(const std::string &_model, const std::string &_texture, const std::string &_shader)
{
    if(_shader != "")
    {
        ngl::ShaderLib * slib = ngl::ShaderLib::instance();
        slib->use( _shader );
    }

    ngl::Obj * m = m_store.getModel(_model);
    if(m == nullptr)
    {
        std::cerr << "Error! Mesh " << _model << " doesn't exist!\n";
        return;
    }

    if(_texture != "")
    {
        GLuint t = m_store.getTexture( _texture );
        if(t == 0)
        {
            std::cerr << "Error! Texture " << _texture << " doesn't exist!\n";
            return;
        }
        bindTextureToShader(_shader, t, "diffuse", 0);
    }

    loadMatricesToShader();
    m->draw();
}

void Scene::createShader(const std::string _name, const std::string _vert, const std::string _frag)
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    slib->createShaderProgram( _name );
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

GLuint Scene::createVAO(std::vector<ngl::Vec4> _verts)
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //Generate a VBO
    GLuint vertBuffer = createBuffer4f( _verts );
    setBufferLocation( vertBuffer, 0, 4 );

    std::vector<ngl::Vec2> uvs;
    uvs.assign( _verts.size(), ngl::Vec2(0.0f, 0.0f) );
    GLuint UVBuffer = createBuffer2f( uvs );
    setBufferLocation( UVBuffer, 1, 2 );

    //Gen normals.
    std::vector<ngl::Vec3> normals;
    normals.reserve( _verts.size() );
    for(size_t i = 0; i < _verts.size(); i += 3)
    {
        ngl::Vec4 a = _verts[i + 1] - _verts[i];
        ngl::Vec4 b = _verts[i + 2] - _verts[i];
        ngl::Vec3 a3 = ngl::Vec3(a.m_x, a.m_y, a.m_z);
        ngl::Vec3 b3 = ngl::Vec3(b.m_x, b.m_y, b.m_z);
        ngl::Vec3 normal;
        normal.cross(a3, b3);
        normal.normalize();
        normal.m_y = -normal.m_y;
        for(int i = 0; i < 3; ++i)
            normals.push_back(normal);
    }
    GLuint normBuffer = createBuffer3f(normals);
    setBufferLocation( normBuffer, 2, 3 );

    return vao;
}

GLuint Scene::createBuffer4f(std::vector<ngl::Vec4> _vec)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(ngl::Vec4) * _vec.size(),
                 &_vec[0].m_x,
            GL_STATIC_DRAW
            );
    return buffer;
}

GLuint Scene::createBuffer3f(std::vector<ngl::Vec3> _vec)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(ngl::Vec3) * _vec.size(),
                 &_vec[0].m_x,
            GL_STATIC_DRAW
            );
    return buffer;
}

GLuint Scene::createBuffer2f(std::vector<ngl::Vec2> _vec)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(ngl::Vec2) * _vec.size(),
                 &_vec[0].m_x,
            GL_STATIC_DRAW
            );
    return buffer;
}

void Scene::setBufferLocation(GLuint _buffer, int _index, int _size)
{
    glEnableVertexAttribArray(_index);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glVertexAttribPointer( _index, _size, GL_FLOAT, GL_FALSE, 0, 0 );
}
