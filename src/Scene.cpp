#include <algorithm>
#include <iostream>
#include <random>
#include <ngl/ShaderLib.h>
#include <ngl/Transformation.h>
#include <ngl/VAOPrimitives.h>
#include <SDL2/SDL.h>

#include "Scene.hpp"
#include "Gui.hpp"
#include "Utility.hpp"

#include <ngl/NGLStream.h>

#include <ngl/AABB.h>

//Can't believe std clamp is c++11 only. Pfffft.
float clamp(float _in, float _lo, float _hi)
{
    if(_in < _lo)
        return _lo;
    else if(_in > _hi)
        return _hi;
    return _in;
}

const int shadowResolution = 4096;

Scene::Scene(ngl::Vec2 _viewport) :
    m_mouse_trans_active(false),
    m_mouse_rot_active(false),
    m_mouse_zoom(10.0f),
    m_mouse_pan(1.0f),
    m_mouse_translation(0.0f,0.0f),
    m_mouse_rotation(0.0f),
    m_mouse_prev_pos(0.0f, 0.0f)
{
    m_viewport = _viewport;

		m_cam.setInitPivot( ngl::Vec3(0.0f, 0.0f, 0.0f));
		m_cam.setInitPos( ngl::Vec3( 0.0f, 1.0f, m_mouse_zoom));
    m_cam.setUp (ngl::Vec3(0.0f,1.0f,0.0f));
    float aspect = _viewport.m_x / _viewport.m_y;
    m_cam.setAspect( aspect );
    m_cam.setFOV( 60.0f );
    m_cam.calculateProjectionMat();
    m_cam.calculateViewMat();

    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    createShader("deferredLight", "vertScreenQuad", "fragBasicLight");
    createShader("diffuse", "vertDeferredData", "fragDeferredDiffuse");
    createShader("colour", "vertDeferredData", "fragBasicColour");
    createShader("charPick", "vertDeferredData", "fragPickChar");
    createShader("terrainPick", "vertDeferredData", "fragPickTerrain");
    createShader("sky", "vertScreenQuad", "fragSky");
    createShader("shadowDepth", "vertMVPUVN", "fragShadowDepth");
    createShader("button", "buttonVert", "buttonFrag", "buttonGeo");

    slib->use("sky");
    slib->setRegisteredUniform("viewport", m_viewport);

    slib->use("deferredLight");
    slib->setRegisteredUniform("pixelstep", ngl::Vec2(0.5f, 0.5f) / m_viewport);

    //reads file with list of names
    readNameFile();
    //creates a character with a random name
    createCharacter();

    //Graphics stuff.
    //Framebuffers
    std::cout << "Initalising data framebuffer to " << m_viewport.m_x << " by " << m_viewport.m_y << '\n';
    m_mainBuffer.initialise(m_viewport.m_x, m_viewport.m_y);

    m_mainBuffer.addTexture( "diffuse", GL_RGBA, GL_RGBA, GL_COLOR_ATTACHMENT0 );
    m_mainBuffer.addTexture( "normal", GL_RGBA, GL_RGBA16F, GL_COLOR_ATTACHMENT1);
    m_mainBuffer.addTexture( "position", GL_RGBA, GL_RGBA32F, GL_COLOR_ATTACHMENT2 );
    m_mainBuffer.addDepthAttachment( "depth" );
    if(!m_mainBuffer.checkComplete())
    {
        std::cerr << "Uh oh! Framebuffer incomplete! Error code " << glGetError() << '\n';
        exit(EXIT_FAILURE);
    }
    m_mainBuffer.unbind();

    std::cout << "Initalising id framebuffer to " << m_viewport.m_x << " by " << m_viewport.m_y << '\n';
    m_pickBuffer.initialise( m_viewport.m_x, m_viewport.m_y );
    m_pickBuffer.addTexture( "terrainpos", GL_RGBA, GL_RGBA, GL_COLOR_ATTACHMENT0 );
    m_pickBuffer.addTexture( "charid", GL_RED_INTEGER, GL_R16I, GL_COLOR_ATTACHMENT1, GL_INT );
    m_pickBuffer.addDepthAttachment( "depth" );
    if(!m_pickBuffer.checkComplete())
    {
        std::cerr << "Uh oh! Framebuffer incomplete! Error code " << glGetError() << '\n';
        exit(EXIT_FAILURE);
    }
    m_pickBuffer.unbind();

    std::cout << "Initalising shadow framebuffer to " << shadowResolution << " by " << shadowResolution << '\n';
    m_shadowBuffer.initialise( shadowResolution, shadowResolution );
    glDrawBuffer( GL_NONE );
    glReadBuffer( GL_NONE );
    for(int i = 0; i < 3; ++i)
    {
        m_shadowBuffer.addTexture("depth[" + std::to_string(i) + "]", GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT );
    }
    if(!m_shadowBuffer.checkComplete())
    {
        std::cerr << "Uh oh! Framebuffer incomplete! Error code " << glGetError() << '\n';
        exit(EXIT_FAILURE);
    }
    m_shadowBuffer.unbind();

    m_shadowMat.assign(1, ngl::Mat4());

    ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();

    prim->createTrianglePlane("plane",14,14,80,80,ngl::Vec3(0,1,0));
    prim->createSphere( "sphere", 0.1, 12 );

    m_store.loadMesh("knight", "knight/knight.obj");
    m_store.loadTexture("knight_d", "knight/knight_d.png");

    m_store.loadMesh("mountain", "mountain/mountain.obj");
    m_store.loadTexture("mountain_d", "mountain/mountain_diff.png");

    //playing with trees and houses and such
    m_store.loadMesh("tree", "tree/tree.obj");
    m_store.loadTexture("tree_d", "tree/tree_d.png");
    m_store.loadMesh("house", "house/house.obj");
    m_store.loadMesh("person", "person/person.obj");
    m_store.loadMesh("storehouse", "storeHouse/storeHouse.obj");
    m_store.loadTexture("storehouse_d", "storeHouse/storehouse_diff.png");

    //Get as vec4s
    std::vector<ngl::Vec4> data;
    std::vector<ngl::Vec3> original = m_grid.getTriangles();
    data.reserve(original.size());
    //Convert format, add some cheeky randomisation
    for(auto &vert : original)
				data.push_back( ngl::Vec4(vert.m_x, vert.m_y, vert.m_z, 1.0f) );

    m_terrainVAO = createVAO( data );
    m_terrainVAOSize = data.size();

    std::vector<ngl::Vec4> verts = {
        ngl::Vec4(-1.0, -1.0, 0.0f, 1.0f),
        ngl::Vec4(1.0, -1.0, 0.0f, 1.0f),
        ngl::Vec4(1.0, 1.0, 0.0f, 1.0f),
        ngl::Vec4(-1.0, 1.0, 0.0f, 1.0f)
    };

    std::vector<ngl::Vec2> uvs = {
        ngl::Vec2(0.0, 0.0),
        ngl::Vec2(1.0, 0.0),
        ngl::Vec2(1.0, 1.0),
        ngl::Vec2(0.0, 1.0)
    };

    m_screenQuad = createVAO(
                verts,
                uvs
                );

    glViewport(0, 0, m_viewport.m_x, m_viewport.m_y);
    glEnable(GL_MULTISAMPLE);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Gui *gui = Gui::instance();
    gui->init(_viewport, "button");
    std::cout << "Scene constructor complete.\n";
}

void Scene::readNameFile()
{
    std::ifstream namesFile;
    namesFile.open("names/game_names.txt");

    if(!namesFile.is_open())
    {
        std::cerr<<"Couldnt open file\n";
        exit(EXIT_FAILURE);
    }

    int lineNo = 0;
    std::string lineBuffer;
    while (!namesFile.eof())
    {
        std::getline(namesFile, lineBuffer, '\n');
        if(lineBuffer.size() != 0)
        {
            m_file_names.push_back(lineBuffer);
            lineNo++;
        }
    }
    namesFile.close();
}

void Scene::createCharacter()
{
  int numberNames = m_file_names.size();
  std::random_device rnd;
  std::mt19937 mt_rand(rnd());
  std::uniform_int_distribution<int> nameNo(0,numberNames - 1);
  int name_chosen = nameNo(mt_rand);
  m_characters.push_back(Character(&m_grid, &m_world_inventory, m_file_names[name_chosen]));
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

    m_cam.movePivot(trans);

    m_cam.calculateViewMat();

    for(Character &character : m_characters)
    {
      if (character.isActive() == true)
        character.update();
    }

    //m_sunAngle.m_x = 150.0f;
    m_sunAngle.m_z = 5.0f;
    m_sunAngle.m_x += 0.1f;
    if(m_sunAngle.m_x > 360.0f)
        m_sunAngle.m_x = 0.0f;
    //std::cout << m_sunAngle.m_x << '\n';

    ngl::Transformation t;
    t.setRotation( m_sunAngle );
    m_sunDir = t.getMatrix().getForwardVector();
    m_sunDir.normalize();

}

//I'm sorry this function is so long :(
void Scene::draw()
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();
    glClearColor(0.0,0.0,0.0,0.0);

    m_transform.reset();
    glEnable(GL_DEPTH_TEST);

    //---------------------------//
    // UTILITY ID DRAW //
    //---------------------------//
    //Draw to pick buffer.
    m_pickBuffer.bind();
    m_pickBuffer.activeColourAttachments({GL_COLOR_ATTACHMENT0});
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    slib->use("terrainPick");

    glBindVertexArray(m_terrainVAO);
    loadMatricesToShader();
    glDrawArraysEXT(GL_TRIANGLES, 0, m_terrainVAOSize);
    glBindVertexArray(0);

    m_pickBuffer.activeColourAttachments({GL_COLOR_ATTACHMENT1});
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    slib->use("charPick");

    //Draw characters...

    m_pickBuffer.unbind();

    //---------------------------//
    //  SHADOW PASS  //
    //---------------------------//
    ngl::Vec3 s = m_sunDir;
    //Flip if the moon is up.
    if(s.dot(ngl::Vec3( 0.0f, 1.0f, 0.0f )) < 0.0f)
        s = -s;

    glViewport(0, 0, shadowResolution, shadowResolution);

    //The intervals at which we will draw into shadow buffers.
    std::vector<float> cascadeDistances = {0.01f, 16.0f, 64.0f, 256.0f};
    //cascadeDistances = {0.01f, 16.0f};

    std::vector< std::pair< ngl::Vec3, ngl::Vec3 > > boxes = generateOrthoShadowMatrices( cascadeDistances );

    //Render a pass from each AABB
    slib->use("shadowDepth");

    size_t index = 0;
    for(auto &box : boxes)
    {
				//std::cout << "Shadowpass for " << index << '\n';
        shadowPass( box, index );
        index++;
    }
		//std::cout << '\n';

    //glCullFace(GL_BACK);
    glViewport(0, 0, m_viewport.m_x, m_viewport.m_y);

    //---------------------------//
    // RAW DATA PASS //
    //---------------------------//
    m_mainBuffer.bind();
    m_mainBuffer.activeColourAttachments();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    slib->use("colour");
    slib->setRegisteredUniform("colour", ngl::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

    m_transform.reset();
    glBindVertexArray(m_terrainVAO);
    loadMatricesToShader();
    glDrawArraysEXT(GL_TRIANGLES, 0, m_terrainVAOSize);
    glBindVertexArray(0);

    slib->use("diffuse");

    for(int i = 0; i < m_grid.getW(); ++i)
        for(int j = 0; j < m_grid.getH(); ++j)
        {
            GridTile t = m_grid.get(i, j);
            m_transform.setPosition(i, 0.0f, j);
            if(t.getType() == TileType::TREES)
            {
                drawAsset( "tree", "tree_d", "diffuse" );
            }
            else if(t.getType() == TileType::MOUNTAINS)
            {
                drawAsset( "mountain", "mountain_d", "diffuse" );
            }
            else if(t.getType() == TileType::STOREHOUSE)
            {
                drawAsset( "storehouse", "storehouse_d", "diffuse" );
            }
            else if(t.getType() == TileType::HOUSE)
            {
                drawAsset( "house", "", "colour");
            }
        }

    for(auto &character : m_characters)
    {
      ngl::Vec2 pos = character.getPos();
      ngl::Vec3 new_pos = {pos[0],0.0f,pos[1]};
      m_transform.setPosition(new_pos);
      drawAsset( "person", "", "colour");
    }

    m_mainBuffer.unbind();
    m_mainBuffer.activeColourAttachments( {GL_COLOR_ATTACHMENT0} );
    //glClearColor(1.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    //---------------------------//
    //          SKY           //
    //---------------------------//
    glBindVertexArray(m_screenQuad);

    slib->use("sky");
    slib->setRegisteredUniform("iMV", m_cam.getV().inverse());
    slib->setRegisteredUniform("iP", m_cam.getP().inverse());
    slib->setRegisteredUniform("camPos", m_cam.getPos());
    slib->setRegisteredUniform( "sunDir", m_sunDir );

    slib->setRegisteredUniform("surfaceHeight", 0.8f + 0.2f * m_sunDir.dot(ngl::Vec3(0.0f, -1.0f, 0.0f)));

    glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);

    //---------------------------//
    //       LIGHTING      //
    //---------------------------//
    slib->use("deferredLight");
    GLuint id = slib->getProgramID("deferredLight");
    slib->setRegisteredUniform("sunDir", m_sunDir );
    slib->setRegisteredUniform( "sunInts", powf( clamp(m_sunDir.dot(ngl::Vec3(0.0f, 1.0f, 0.0f)), 0.0f, 1.0f), 0.1f ) );
    slib->setRegisteredUniform( "moonInts", clamp(m_sunDir.dot(ngl::Vec3(0.0f, -1.0f, 0.0f)), 0.0f, 1.0f) );
    slib->setRegisteredUniform( "shadowMatrix[0]", m_shadowMat[0] );
    slib->setRegisteredUniform( "shadowMatrix[1]", m_shadowMat[1] );
    slib->setRegisteredUniform( "shadowMatrix[2]", m_shadowMat[2] );

    slib->setRegisteredUniform( "camPos", ngl::Vec4(m_cam.getPos()) );
    for( int i = 0; i < cascadeDistances.size(); ++i )
        slib->setRegisteredUniform( "cascades[" + std::to_string(i) + "]", cascadeDistances[i] );

    //m_shadowBuffer.bindTexture(id, "depth", "diffuse", 0);
    m_mainBuffer.bindTexture(id, "diffuse", "diffuse", 0);
    m_mainBuffer.bindTexture(id, "normal", "normal", 1);
    m_mainBuffer.bindTexture(id, "position", "position", 2);
    m_shadowBuffer.bindTexture( id, "depth[0]", "shadowDepths[0]", 3 );
    m_shadowBuffer.bindTexture( id, "depth[1]", "shadowDepths[1]", 4 );
    m_shadowBuffer.bindTexture( id, "depth[2]", "shadowDepths[2]", 5 );

    glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);

    //---------------------------//
    //          BUTTONS          //
    //---------------------------//
    // ?
    glClear(GL_DEPTH_BUFFER_BIT);
    Gui::instance()->drawButtons();

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

std::vector< bounds > Scene::generateOrthoShadowMatrices(const std::vector<float> &_divisions)
{
    ngl::Vec3 s = m_sunDir;
    //Flip if the moon is up.
    if(s.dot(ngl::Vec3( 0.0f, 1.0f, 0.0f )) < 0.0f)
        s = -s;
    ngl::Mat4 V = ngl::lookAt(
                s,
                ngl::Vec3(0.0f, 0.0f, 0.0f),
                ngl::Vec3(0.0f, 1.0f, 0.0f)
                );

    //Get cascades from this.
    std::vector< std::array<ngl::Vec3, 8> > cascades;

    for(int i = 0; i <= _divisions.size() - 2; ++i)
    {
				//std::cout << "Calculating for " << _divisions[i] << " to " << _divisions[ i + 1 ] << '\n';
        cascades.push_back( m_cam.calculateCascade( _divisions[i], _divisions[i + 1] ) );
    }

    //These cascades are in world space. We must convert them into light space.
    for(auto &c : cascades)
    {
        //Loop through each vertex.
        for(auto &vert : c)
        {
            ngl::Vec4 vert4 (vert.m_x, vert.m_y, vert.m_z, 1.0f);
            vert4 = V * vert4;
            vert = ngl::Vec3( vert4.m_x, vert4.m_y, vert4.m_z );
            //std::cout << "Post trans " << vert << '\n';
        }
        //std::cout << '\n';
    }

    //Create AABBs enclosing each cascade section
    std::vector< std::pair<ngl::Vec3, ngl::Vec3> > boxes;
    for(auto &c : cascades)
        boxes.push_back( Utility::enclose(c) );

    return boxes;
}

void Scene::shadowPass(bounds _box, size_t _index)
{
    ngl::Vec3 s = m_sunDir;
    //Flip if the moon is up.
    if(s.dot(ngl::Vec3( 0.0f, 1.0f, 0.0f )) < 0.0f)
        s = -s;

    //Box dimensions
    ngl::Vec3 dim = _box.second - _box.first;
    //Box position
    //ngl::Vec3 mid = (_box.first + _box.second) / 2.0f;

    ngl::Mat4 P = ngl::ortho(
                -dim.m_x, dim.m_x,
                -dim.m_y, dim.m_y,
                -dim.m_z, dim.m_z
                );

    ngl::Mat4 V = ngl::lookAt(
                s + m_cam.getPos(),
                ngl::Vec3(0.0f, 0.0f, 0.0f) + m_cam.getPos(),
                ngl::Vec3(0.0f, 1.0f, 0.0f)
                );

    /*ngl::Mat4 P = ngl::ortho(
                -32, 32,
                -32, 32,
                -32, 32
                );

    ngl::Mat4 V = ngl::lookAt(
                s,
                ngl::Vec3(0.0f, 0.0f, 0.0f),
                ngl::Vec3(0.0f, 1.0f, 0.0f)
                );*/

    m_shadowMat[_index] = V * P;

    m_shadowBuffer.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowBuffer.get( "depth[" + std::to_string(_index) + "]" ), 0);
    glClear( GL_DEPTH_BUFFER_BIT );
    //glCullFace(GL_FRONT);

    glBindVertexArray(m_terrainVAO);
    //loadMatricesToShader();
    loadMatricesToShader(ngl::Mat4(), m_shadowMat[_index]);
    glDrawArraysEXT(GL_TRIANGLES, 0, m_terrainVAOSize);
    glBindVertexArray(0);

    for(int i = 0; i < m_grid.getW(); ++i)
        for(int j = 0; j < m_grid.getH(); ++j)
        {
            GridTile t = m_grid.get(i, j);
            m_transform.setPosition(i, 0.0f, j);
            ngl::Mat4 mvp = m_transform.getMatrix() * m_shadowMat[_index];
            if(t.getType() == TileType::TREES)
            {
                ngl::Obj * k = m_store.getModel( "tree" );
                loadMatricesToShader( m_transform.getMatrix(), mvp );
                k->draw();
            }
            else if(t.getType() == TileType::MOUNTAINS)
            {
              ngl::Obj * k = m_store.getModel( "mountain" );
              loadMatricesToShader( m_transform.getMatrix(), mvp );
              k->draw();
            }
            else if(t.getType() == TileType::HOUSE)
            {
                ngl::Obj * k = m_store.getModel( "house" );
                loadMatricesToShader( m_transform.getMatrix(), mvp );
                k->draw();
            }
            else if(t.getType() == TileType::STOREHOUSE)
            {
                ngl::Obj * k = m_store.getModel( "storehouse" );
                loadMatricesToShader( m_transform.getMatrix(), mvp );
                k->draw();
            }
        }

    for(auto &character : m_characters)
    {
      ngl::Vec2 pos = character.getPos();
      m_transform.setPosition(pos[0],0.0f,pos[1]);
      ngl::Mat4 mvp = m_transform.getMatrix() * m_shadowMat[_index];

      ngl::Obj * k = m_store.getModel( "person" );
      loadMatricesToShader( m_transform.getMatrix(), mvp );
      k->draw();
    }


    //Tweaking the shadow matrix so that it can be used for shading.
    ngl::Mat4 biasMat (
                0.5, 0.0, 0.0, 0.0,
                0.0, 0.5, 0.0, 0.0,
                0.0, 0.0, 0.5, 0.0,
                0.5, 0.5, 0.5, 1.0
                );
    m_shadowMat[_index] = m_shadowMat[_index] * biasMat;

    m_shadowBuffer.unbind();
}

void Scene::mousePressEvent(const SDL_MouseButtonEvent &_event)
{
    //checks if the left button has been pressed down and flags start
    if(_event.button == SDL_BUTTON_LEFT)
    {
        m_mouse_trans_origin[0] = _event.x;
        m_mouse_trans_origin[1] = _event.y;
        //records position of mouse on press
        m_mouse_prev_pos[0] = _event.x;
        m_mouse_prev_pos[1] = _event.y;
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

        //checks if the mouse has moved, a click or a drag
        ngl::Vec2 distance = m_mouse_trans_origin - m_mouse_prev_pos;

        //if its a click then the mouseSelection funciton is called
        if(distance.length() < 1)
            mouseSelection();

        m_mouse_prev_pos.set(0.0f, 0.0f);
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
		if(_event.y > 0 && m_mouse_zoom > 10.5)
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

void Scene::mouseSelection()
{
	std::cout<<"--------CALLED MOUSE SELECTION----------------"<<std::endl;

	int mouse_coords[2] = {0,0};
	SDL_GetMouseState(&mouse_coords[0], &mouse_coords[1]);

	m_pickBuffer.bind();

	//check character_id texture
	GLuint char_texID = getCharPickTexture();
	glBindTexture(GL_TEXTURE_2D, char_texID);
	glReadBuffer(GL_COLOR_ATTACHMENT1);

	long unsigned int red;
	glReadPixels(mouse_coords[0], (m_viewport[1] - mouse_coords[1]), 1, 1, GL_RED, GL_UNSIGNED_BYTE, &red);
	//change depending on number characters
	if(red >= 0 && red < m_characters.size())
	{
		for (Character &character : m_characters)
		{
			if (character.getID() == red)
			{
				m_active_char = &character;
				if(character.isActive() == false)
					character.setActive(true);
			}
		}
	}
	//check grid_id texture
	else
	{
		//bind default texture
		glBindTexture(GL_TEXTURE_2D, 0);

		GLuint grid_texID = getTerrainPickTexture();
		glBindTexture(GL_TEXTURE_2D, grid_texID);
		glReadBuffer(GL_COLOR_ATTACHMENT0);

		std::array<unsigned char, 3> grid_coord;

		// x, window height - y - 1
		glReadPixels(mouse_coords[0], (m_viewport[1] - mouse_coords[1]), 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &grid_coord[0]);

		if(grid_coord[0] != 0 && grid_coord[2] != 0)
		{
			std::cout<<int(grid_coord[0])<<","<<int(grid_coord[1])<<","<<int(grid_coord[2])<<": GRID_COORDS"<<std::endl;

			int grid_coord_x = floor((grid_coord[0]/255.0) * 50);
			int grid_coord_y = floor((grid_coord[2]/255.0) * 50);

			std::cout<<grid_coord_x<<", "<<grid_coord_y<<": GRID_COORDS"<<std::endl;

			int target_id = m_grid.coordToId(ngl::Vec2(grid_coord_x, grid_coord_y));

			if(m_characters[0].isActive() == true)
			{
				std::cout<<"STATE"<<std::endl;
				m_characters[0].setTarget(target_id);
			}

		}
		else
		{
			//if grid_coord == {0,0,0}
			std::cout<<"NO GRID CLICK D:"<<std::endl;
		}
	}

	glReadBuffer(GL_NONE);
	m_pickBuffer.unbind();

}

void Scene::loadMatricesToShader(const ngl::Mat4 _M, const ngl::Mat4 _MVP)
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();
    slib->setRegisteredUniform( "M", _M );
    slib->setRegisteredUniform( "MVP", _MVP );
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
        //std::cerr << "Uh oh! Invalid uniform location in Scene::bindTextureToShader!! " << _uniform << '\n';
        //return;
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

void Scene::createShader(const std::string _name, const std::string _vert, const std::string _frag, const std::string _geo)
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

    // add geometry shader if string given
    if(!_geo.empty())
    {
      slib->attachShader(_geo, ngl::ShaderType::GEOMETRY);
      slib->loadShaderSource(_geo, "shaders/" + _geo + ".glsl");
      slib->compileShader(_geo);
      slib->attachShaderToProgram(_name, _geo);
    }

    slib->linkProgramObject(_name);
}

GLuint Scene::createVAO(std::vector<ngl::Vec4> &_verts)
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
    //Zero the elements
    normals.reserve( _verts.size());

    for(size_t i = 0; i < _verts.size(); i += 3)
    {
        ngl::Vec4 a = _verts[i + 1] - _verts[i];
        ngl::Vec4 b = _verts[i + 2] - _verts[i];
        ngl::Vec3 a3 = ngl::Vec3(a.m_x, a.m_y, a.m_z);
        ngl::Vec3 b3 = ngl::Vec3(b.m_x, b.m_y, b.m_z);
        ngl::Vec3 normal = a3.cross(b3);
        normal.normalize();
        //normal = -normal;
        for(int i = 0; i < 3; ++i)
            normals.push_back(normal);
    }

    GLuint normBuffer = createBuffer3f(normals);
    setBufferLocation( normBuffer, 2, 3 );

    return vao;
}

GLuint Scene::createVAO(std::vector<ngl::Vec4> &_verts, std::vector<ngl::Vec2> &_uvs)
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //Generate a VBO
    GLuint vertBuffer = createBuffer4f( _verts );
    setBufferLocation( vertBuffer, 0, 4 );

    GLuint UVBuffer = createBuffer2f( _uvs );
    setBufferLocation( UVBuffer, 1, 2 );

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
