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

Scene::Scene(ngl::Vec2 _viewport) :
    m_active(true),
    m_mouse_trans_active(false),
    m_mouse_rot_active(false),
    m_mouse_zoom_cur(10.0f),
    m_mouse_zoom_targ(10.0f),
    m_mouse_pan(1.0f),
    m_mouse_translation(0.0f,0.0f),
    m_mouse_rotation(0.0f),
    m_mouse_prev_pos(0.0f, 0.0f),
    m_sunAngle(90.0f, 0.0f, 5.0f),
    m_day(80)
{
    m_prefs = Preferences::instance();
    m_viewport = _viewport;

    m_cam.setInitPivot( ngl::Vec3(0.0f, 0.0f, 0.0f));
    m_cam.setInitPos( ngl::Vec3( 0.0f, 1.0f, m_mouse_zoom_cur));
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
    createShader("terrain", "vertDeferredData", "fragTerrain");
    createShader("terrainPick", "vertDeferredData", "fragPickTerrain");
    createShader("sky", "vertScreenQuad", "fragSky");
    createShader("shadowDepth", "vertMVPUVN", "fragShadowDepth");
    createShader("button", "buttonVert", "buttonFrag", "buttonGeo");
    createShader("water", "vertWater", "fragWater", "", "tescWater", "teseWater");
    createShader("waterDisplacement", "vertScreenQuad", "fragWaterDisplacement");

    slib->use("sky");
    slib->setRegisteredUniform("viewport", m_viewport);

    slib->use("deferredLight");
    slib->setRegisteredUniform("pixelstep", ngl::Vec2(0.5f, 0.5f) / m_viewport);
    slib->setRegisteredUniform("waterLevel", m_grid.getWaterLevel() / m_terrainHeightDivider);

    slib->use("water");
    GLint tlvl = 0;
    glGetIntegerv( GL_MAX_TESS_GEN_LEVEL, &tlvl );
    slib->setRegisteredUniform("maxTessLevel", tlvl);
    std::cout << "Max water tesselation level set to " << tlvl << '\n';
    slib->setRegisteredUniform("pixelstep", ngl::Vec2(1.0f, 1.0f) / m_prefs->getWaterMapRes());
    slib->setRegisteredUniform("viewport", m_viewport);

    //reads file with list of names
    readNameFile();
    //creates a character with a random name
    createCharacter();

    initialiseFramebuffers();

    m_shadowMat.assign(3, ngl::Mat4());

    ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();

    prim->createTrianglePlane("plane",14,14,80,80,ngl::Vec3(0,1,0));
    prim->createSphere( "sphere", 0.1, 12 );

    std::cout << "Loading assets...\n";
    //m_store.loadMesh("knight", "knight/knight.obj");
    //m_store.loadTexture("knight_d", "knight/knight_d.png");

    m_store.loadMesh("mountain", "mountain/mountain.obj");
    m_store.loadTexture("mountain_d", "mountain/mountain_diff.png");

    //playing with trees and houses and such
    m_store.loadMesh("tree", "tree/tree.obj");
    m_store.loadTexture("tree_d", "tree/tree_d.png");
    m_store.loadMesh("house", "house/house.obj");
    m_store.loadMesh("person", "person/person.obj");
    m_store.loadMesh("storehouse", "storeHouse/storeHouse.obj");
    m_store.loadTexture("storehouse_d", "storeHouse/storehouse_diff.png");

    m_store.loadTexture("grass", "terrain/grass.png");
    m_store.loadTexture("rock", "terrain/rock.png");
    m_store.loadTexture("snow", "terrain/snow.png");

    std::cout << "Constructing terrain...\n";
    m_terrainVAO = constructTerrain();

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

    verts = {ngl::Vec4(0.0f, 0.0f, 0.0f, 1.0f), ngl::Vec4(0.0f, 1.0f, 0.0f, 1.0f), ngl::Vec4(1.0f, 1.0f, 0.0f, 1.0f), ngl::Vec4(1.0f, 0.0f, 0.0f, 1.0f)};
    std::vector<ngl::Vec3> normals = {ngl::Vec3(0.0f, 0.0f, 1.0f), ngl::Vec3(0.0f, 0.0f, 1.0f), ngl::Vec3(0.0f, 0.0f, 1.0f), ngl::Vec3(0.0f, 0.0f, 1.0f)};
    uvs = {ngl::Vec2(0.0f, 0.0f), ngl::Vec2(1.0f, 0.0f), ngl::Vec2(0.0f, 1.0f), ngl::Vec2(1.0f, 1.0f)};

    m_unitSquareVAO = createVAO(
                verts,
                normals,
                uvs
                );

    GLint MaxPatchVertices = 0;
    glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
    std::cout << "Max supported patch vertices " << MaxPatchVertices << '\n';
    glPatchParameteri(GL_PATCH_VERTICES, 4);

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
    gui->init(this, _viewport, "button");
    std::cout << "Scene constructor complete.\n";
}

void Scene::initialiseFramebuffers()
{
    //Graphics stuff.
    //Framebuffers
    std::cout << "Initalising data framebuffer to " << m_viewport.m_x << " by " << m_viewport.m_y << '\n';
    m_mainBuffer.initialise(m_viewport.m_x, m_viewport.m_y);
    m_mainBuffer.addTexture( "diffuse", GL_RGBA, GL_RGBA, GL_COLOR_ATTACHMENT0 );
    m_mainBuffer.addTexture( "normal", GL_RGBA, GL_RGBA16F, GL_COLOR_ATTACHMENT1);
    m_mainBuffer.addTexture( "position", GL_RGBA, GL_RGBA32F, GL_COLOR_ATTACHMENT2 );
    m_mainBuffer.addTexture( "linearDepth", GL_RED, GL_R16F, GL_COLOR_ATTACHMENT3 );
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

    std::cout << "Initalising shadow framebuffer to " << m_prefs->getShadowMapRes() << " by " << m_prefs->getShadowMapRes() << '\n';
    m_shadowBuffer.initialise(m_prefs->getShadowMapRes(), m_prefs->getShadowMapRes());
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

    std::cout << "Initalising displacement framebuffer to " << m_prefs->getWaterMapRes() << " by " << m_prefs->getWaterMapRes() << '\n';
    m_displacementBuffer.initialise(m_prefs->getWaterMapRes(), m_prefs->getWaterMapRes());
    m_displacementBuffer.addTexture("waterDisplacement", GL_RED, GL_R16F, GL_COLOR_ATTACHMENT0);
    if(!m_displacementBuffer.checkComplete())
    {
        std::cerr << "Uh oh! Framebuffer incomplete! Error code " << glGetError() << m_prefs->getWaterMapRes() <<  '\n';
        exit(EXIT_FAILURE);
    }
    m_displacementBuffer.unbind();
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
    m_cam.setInitPos(ngl::Vec3(0.0f, 0.0f, m_mouse_zoom_cur));

    //Clear all transformations from previous update.
    m_cam.clearTransforms();

    //Construct translation *change* using right and forward vectors.
    ngl::Vec3 trans = m_cam.right() * m_mouse_translation.m_x * 0.05f;
    trans += m_cam.forwards() * -m_mouse_translation.m_y * 0.05f;
    trans -= m_cam.getPivot();

    ngl::Vec3 cxy = m_cam.getPos();
    int x = clamp(std::round(cxy.m_x),0,m_grid.getW());
    int y = clamp(std::round(cxy.m_z),0,m_grid.getH());
    cxy.m_y = m_grid.get(x, y).getHeight() / m_terrainHeightDivider;

    trans.m_y = -cxy.m_y;

    m_camTargPos = trans;
    m_camCurPos += (m_camTargPos - m_camCurPos) / 8.0f;
    m_mouse_zoom_cur -= (m_mouse_zoom_cur - m_mouse_zoom_targ) / 8.0f;

    m_cam.rotateCamera(m_mouse_pan, m_mouse_rotation, 0.0f);

    m_cam.movePivot(m_camCurPos);

    m_cam.calculateViewMat();

    for(Character &character : m_characters)
    {
        if (character.isActive() == true)
            character.update();
    }

    //m_sunAngle.m_x = 150.0f;
    m_sunAngle.m_z = 30.0f - 25.0f * sinf(m_season * M_PI - M_PI / 2.0f);
    m_sunAngle.m_x += m_prefs->getTimeScale();
    if(m_sunAngle.m_x > 360.0f)
    {
        m_day++;
        m_sunAngle.m_x = 0.0f;
        //std::cout << "Day " << m_day << " Season " << m_season << '\n';
    }
    //std::cout << m_sunAngle.m_x << '\n';

    m_season = (m_day % 365) / 365.0f;

    ngl::Transformation t;
    t.setRotation( m_sunAngle );
    m_sunDir = t.getMatrix().getForwardVector();
    m_sunDir.normalize();

    //1 = Midday
    //0 = Sunrise/sunset
    //-1 = Midnight
    float a = m_sunDir.dot( ngl::Vec3(0.0f, 1.0f, 0.0f) );
    //Map to range 0.5PI to -0.5PI
    a *= M_PI / 2.0;

    float t_midday = 0.5f * sin(a) + 0.5f;
    float t_midnight = 0.5f * sin(a + M_PI) + 0.5f;
    float t_sundown = 0.5f * sin(a * 2.0f + M_PI / 2.0f) + 0.5f;

    m_directionalLightCol = t_midday * ngl::Vec3(0.95f, 0.95f, 1.0f) +
            t_midnight * ngl::Vec3(0.3f, 0.6f, 0.8f) +
            t_sundown * ngl::Vec3(1.0f, 0.8f, 0.1f);

    m_directionalLightCol /= t_midday + t_midnight + t_sundown;
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

		ngl::Vec2 grid_size {m_grid.getW(), m_grid.getH()};
		slib->setRegisteredUniform("dimensions", grid_size);

    glBindVertexArray(m_terrainVAO);
    loadMatricesToShader();
    glDrawArraysEXT(GL_TRIANGLES, 0, m_terrainVAOSize);
    glBindVertexArray(0);
    m_pickBuffer.activeColourAttachments({GL_COLOR_ATTACHMENT1});
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    slib->use("charPick");

    //Draw characters...
    for(auto &ch : m_characters)
    {
				ngl::Vec3 pos = ch.getPos();
				m_transform.setPosition(pos);
        slib->setRegisteredUniform("id", ch.getID());
        drawAsset( "person", "", "");
    }

    m_pickBuffer.unbind();
    //---------------------------//
    //  SHADOW PASS  //
    //---------------------------//
    ngl::Vec3 s = m_sunDir;
    //Flip if the moon is up.
    if(s.dot(ngl::Vec3( 0.0f, 1.0f, 0.0f )) < 0.0f)
        s = -s;

    glViewport(0, 0, m_prefs->getShadowMapRes(), m_prefs->getShadowMapRes());

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

    //glCullFace(GL_BACK);
    glViewport(0, 0, m_viewport.m_x, m_viewport.m_y);

    //---------------------------//
    // RAW DATA PASS //
    //---------------------------//
    m_mainBuffer.bind();
    m_mainBuffer.activeColourAttachments();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    slib->use("terrain");
    bindTextureToShader("terrain", m_store.getTexture("grass"), "grass", 0);
    bindTextureToShader("terrain", m_store.getTexture("rock"), "rock", 1);
    bindTextureToShader("terrain", m_store.getTexture("snow"), "snow", 2);

    float waterLevel = m_grid.getWaterLevel() / m_terrainHeightDivider;
    slib->setRegisteredUniform( "waterlevel", waterLevel);
    float snowLevel = m_grid.getMountainHeight() / m_terrainHeightDivider;
    float difference = snowLevel - waterLevel;
    float snow = 0.5f * difference * sinf(m_season * 2.0f * M_PI - M_PI / 2.0f) + 0.5f * difference + waterLevel;
    slib->setRegisteredUniform( "snowline", snow);

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
            m_transform.setPosition(i, m_grid.get(i,j).getHeight() / m_terrainHeightDivider, j);
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
				ngl::Vec3 pos = character.getPos();
				pos.m_y /= m_terrainHeightDivider;
				m_transform.setPosition(pos);
				slib->use("colour");
				slib->setRegisteredUniform("colour", ngl::Vec4(1.0f,1.0f,1.0f,1.0f));
        drawAsset( "person", "", "colour");
    }

    m_mainBuffer.unbind();

    //This gives opengl error 1282. I guess you can't specify draw buffers for the back buffer?
    //m_mainBuffer.activeColourAttachments( {GL_COLOR_ATTACHMENT0} );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    //---------------------------//
    //            SKY            //
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
    //          LIGHTING         //
    //---------------------------//
    slib->use("deferredLight");
    GLuint id = slib->getProgramID("deferredLight");
    slib->setRegisteredUniform("sunDir", m_sunDir );
    slib->setRegisteredUniform( "sunInts", clamp(m_sunDir.dot(ngl::Vec3(0.0f, 1.0f, 0.0f)), 0.0f, 1.0f) * 1.8f );
    slib->setRegisteredUniform( "moonInts", clamp(m_sunDir.dot(ngl::Vec3(0.0f, -1.0f, 0.0f)), 0.0f, 1.0f) );
    slib->setRegisteredUniform("iGlobalTime", m_sunAngle.m_x * 8.0f);
    slib->setRegisteredUniform( "directionalLightCol", m_directionalLightCol );
    slib->setRegisteredUniform( "shadowMatrix[0]", m_shadowMat[0] );
    slib->setRegisteredUniform( "shadowMatrix[1]", m_shadowMat[1] );
    slib->setRegisteredUniform( "shadowMatrix[2]", m_shadowMat[2] );

    slib->setRegisteredUniform( "camPos", ngl::Vec4(m_cam.getPos()) );
    for( int i = 0; i < cascadeDistances.size(); ++i )
        slib->setRegisteredUniform( "cascades[" + std::to_string(i) + "]", cascadeDistances[i] );

    m_shadowBuffer.bindTexture(id, "depth", "diffuse", 0);
    m_mainBuffer.bindTexture(id, "diffuse", "diffuse", 0);
    m_mainBuffer.bindTexture(id, "normal", "normal", 1);
    m_mainBuffer.bindTexture(id, "position", "position", 2);
    m_mainBuffer.bindTexture( id, "linearDepth", "linearDepth", 3 );
    m_shadowBuffer.bindTexture( id, "depth[0]", "shadowDepths[0]", 4 );
    m_shadowBuffer.bindTexture( id, "depth[1]", "shadowDepths[1]", 5 );
    m_shadowBuffer.bindTexture( id, "depth[2]", "shadowDepths[2]", 6 );

    glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);

    //---------------------------//
    //       DISPLACEMENT       //
    //---------------------------//
    m_displacementBuffer.bind();
    m_displacementBuffer.activeColourAttachments();
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, m_prefs->getWaterMapRes(), m_prefs->getWaterMapRes());

    glBindVertexArray(m_screenQuad);

    slib->use("waterDisplacement");
    slib->setRegisteredUniform("iGlobalTime", m_sunAngle.m_x * 8.0f);

    glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);

    m_displacementBuffer.unbind();

    glViewport(0, 0, m_viewport.m_x, m_viewport.m_y);

    //---------------------------//
    //      FORWARD-SHADING      //
    //---------------------------//

    //Copy depth buffer from main buffer to back buffer.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_mainBuffer.getID());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, m_viewport.m_x, m_viewport.m_y, 0, 0, m_viewport.m_x, m_viewport.m_y,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glEnable(GL_DEPTH_TEST);

    slib->use("water");
    slib->setRegisteredUniform("gEyeWorldPos", m_cam.getPos());
    slib->setRegisteredUniform("iGlobalTime", m_sunAngle.m_x * 8.0f);
    slib->setRegisteredUniform("lightDir", m_sunDir);
    slib->setRegisteredUniform("camPos", m_cam.getPos());
    slib->setRegisteredUniform( "directionalLightCol", m_directionalLightCol );

    id = slib->getProgramID("water");

    m_displacementBuffer.bindTexture( id, "waterDisplacement", "displacement", 0 );
    m_mainBuffer.bindTexture( id, "position", "terrainPos", 1 );

    glBindVertexArray(m_unitSquareVAO);
    m_transform.reset();

    //Unit size of each water tile.
    int scale = 10;
    for(int i = 0; i < m_grid.getW(); i += scale)
    {
        for(int j = 0; j < m_grid.getH(); j += scale)
        {
            m_transform.setScale(scale, scale, 1.0f);
            m_transform.setRotation(90.0f, 0.0f, 0.0f);
            m_transform.setPosition(i, m_grid.getWaterLevel() / m_terrainHeightDivider, j);
            loadMatricesToShader();
            glDrawArraysEXT(GL_PATCHES, 0, 4);
        }
    }

    glBindVertexArray(0);

    //---------------------------//
    //          BUTTONS          //
    //---------------------------//
    // ?
    glClear(GL_DEPTH_BUFFER_BIT);
    Gui::instance()->drawButtons();

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void Scene::quit()
{
    m_active = false;
}

bool Scene::isActive()
{
    return m_active;
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
            m_transform.setPosition(i, m_grid.get(i,j).getHeight()/m_terrainHeightDivider, j);
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
				ngl::Vec3 pos = character.getPos();
				pos.m_y /= m_terrainHeightDivider;
				//std::cout<<"HEIGHT: "<<pos.m_y<<std::endl;
				m_transform.setPosition(pos);
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
        Gui::instance()->mouseDown();
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
        Gui::instance()->mouseUp();
    }

    //checks if the right button has been released and turns off flag
    else if (_event.button == SDL_BUTTON_RIGHT)
    {
        m_mouse_rot_active = false;
    }
}

void Scene::wheelEvent(const SDL_MouseWheelEvent &_event)
{
    //pans camera up and down
    if(_event.y > 0 && m_mouse_zoom_targ > 10.5)
    {
        if (m_mouse_pan > -5)
        {
            m_mouse_pan -= 0.5;
        }
        m_mouse_zoom_targ -= 0.5;
    }

    else if(_event.y < 0 && m_mouse_zoom_targ < 20)
    {
        if(m_mouse_pan < 10)
        {
            m_mouse_pan += 0.5;
        }
        m_mouse_zoom_targ += 0.5;
    }
}

void Scene::updateMousePos()
{
    int mouse_coords[2] = {0,0};
    SDL_GetMouseState(&mouse_coords[0], &mouse_coords[1]);
    Gui::instance()->mousePos(ngl::Vec2(mouse_coords[0], mouse_coords[1]));
}

void Scene::windowEvent(const SDL_WindowEvent &_event)
{
    ngl::Vec2 res;
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();
    switch (_event.event) {
    case SDL_WINDOWEVENT_SIZE_CHANGED:
        res = ngl::Vec2(_event.data1, _event.data2);
        resize(res);
        break;
    default:
        break;
    }
}

void Scene::resize(const ngl::Vec2 &_dim)
{
    std::cout << "Resizing viewport to " << _dim.m_x << ", " << _dim.m_y << '\n';
    m_viewport = _dim;
    m_cam.setAspect( m_viewport.m_x / m_viewport.m_y );
    m_cam.calculateProjectionMat();
    m_cam.calculateViewMat();

    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    slib->use("sky");
    slib->setRegisteredUniform("viewport", m_viewport);

    slib->use("deferredLight");
    slib->setRegisteredUniform("pixelstep", ngl::Vec2(0.5f, 0.5f) / m_viewport);
    slib->setRegisteredUniform("waterLevel", m_grid.getWaterLevel() / m_terrainHeightDivider);

    slib->use("water");
    GLint tlvl = 0;
    glGetIntegerv( GL_MAX_TESS_GEN_LEVEL, &tlvl );
    slib->setRegisteredUniform("maxTessLevel", tlvl);
    std::cout << "Max water tesselation level set to " << tlvl << '\n';
    slib->setRegisteredUniform("pixelstep", ngl::Vec2(1.0f, 1.0f) / m_prefs->getWaterMapRes());
    slib->setRegisteredUniform("viewport", m_viewport);

    m_mainBuffer = Framebuffer();
    m_pickBuffer = Framebuffer();
    m_shadowBuffer = Framebuffer();

    initialiseFramebuffers();
    Gui::instance()->setResolution(_dim);
    std::cout << "Viewport resized.\n";
}

void Scene::mouseSelection()
{
    std::cout<<"CALLED MOUSE SELECTION\n"<<std::endl;
    Gui *gui = Gui::instance();
    int mouse_coords[2] = {0,0};
    SDL_GetMouseState(&mouse_coords[0], &mouse_coords[1]);

    if(gui->mousePos(ngl::Vec2(mouse_coords[0], mouse_coords[1])))
    {
        gui->click();
    }
    else
    {
        m_pickBuffer.bind();

        //check character_id texture
        GLuint char_texID = getCharPickTexture();
        glBindTexture(GL_TEXTURE_2D, char_texID);
        glReadBuffer(GL_COLOR_ATTACHMENT1);

        long unsigned int red;
        glReadPixels(mouse_coords[0], (m_viewport[1] - mouse_coords[1]), 1, 1, GL_RED, GL_UNSIGNED_BYTE, &red);
				std::cout<<"RED"<<red<<std::endl;
        //change depending on number characters
        if(red >= 0 && red < m_characters.size())
        {
            for (Character &character : m_characters)
            {
                if (character.getID() == red)
                {
                    // needs changing, maybe probably because vector addresses can change?
                    Gui::instance()->setActiveCharacter(&character);
                    //
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

								int grid_coord_x = floor((grid_coord[0]/255.0) * m_grid.getW());
								int grid_coord_y = floor((grid_coord[2]/255.0) * m_grid.getH());

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

void Scene::createShader(const std::string _name, const std::string _vert, const std::string _frag, const std::string _geo, const std::string _tessctrl, const std::string _tesseval)
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

    // add tesselation source, if present
    if(!_tessctrl.empty())
    {
        slib->attachShader(_tessctrl, ngl::ShaderType::TESSCONTROL);
        slib->loadShaderSource(_tessctrl, "shaders/" + _tessctrl + ".glsl");
        slib->compileShader(_tessctrl);
        slib->attachShaderToProgram(_name, _tessctrl);
    }

    if(!_tesseval.empty())
    {
        slib->attachShader(_tesseval, ngl::ShaderType::TESSEVAL);
        slib->loadShaderSource(_tesseval, "shaders/" + _tesseval + ".glsl");
        slib->compileShader(_tesseval);
        slib->attachShaderToProgram(_name, _tesseval);
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

GLuint Scene::createVAO(std::vector<ngl::Vec4> &_verts, std::vector<ngl::Vec3> &_normals, std::vector<ngl::Vec2> &_uvs)
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //Generate a VBO
    GLuint vertBuffer = createBuffer4f( _verts );
    setBufferLocation( vertBuffer, 0, 4 );

    GLuint normBuffer = createBuffer3f( _normals );
    setBufferLocation( normBuffer, 2, 3 );

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

GLuint Scene::constructTerrain()
{
    std::cout << "constructTerrain start\n";

    std::vector<std::vector<ngl::Vec3>> faces;
    std::vector<std::vector<ngl::Vec3>> facenorms;

    //Face centers to start.
    for(int i = 0; i < m_grid.getW(); ++i)
    {
        faces.push_back( std::vector<ngl::Vec3>() );
        for(int j = 0; j < m_grid.getH(); ++j)
        {
            float height = m_grid.get(i, j).getHeight() / m_terrainHeightDivider;
            ngl::Vec3 face (i, height, j);
            faces[i].push_back(face);
        }
    }

    //Smooth
    /*const int iterations = 128;
    const float hardness = 0.02f;
    for(int it = 0; it < iterations; ++it)
    {
        for(int i = 1; i < m_grid.getW() - 1; ++i)
        {
            for(int j = 1; j < m_grid.getH() - 1; ++j)
            {
                float ay = 0.0f;
                for(int y = -1; y <= 1; ++y)
                    for(int x = -1; x <= 1; ++x)
                        ay += faces[i + x][j + y].m_y;

                ay /= 9.0f;

                float dy = faces[i][j].m_y - ay;

                faces[i][j].m_y -= hardness * dy;
            }
        }
    }*/

    //Calculate face normals
    for(int i = 0; i < faces.size(); ++i)
    {
        facenorms.push_back( std::vector<ngl::Vec3>() );
        for(int j = 0; j < faces[i].size(); ++j)
        {
            ngl::Vec3 normal;

            bool left = i > 0;
            bool bottom = j > 0;
            bool right = i < faces.size() - 1;
            bool top = j < faces[i].size() - 1;

            int count = 0;

            //->
            if(top and right)
            {
                ngl::Vec3 toTop = faces[i][j+1] - faces[i][j];
                ngl::Vec3 toRight = faces[i+1][j] - faces[i][j];
                ngl::Vec3 n = toTop.cross(toRight);
                n.normalize();
                normal += n;
                count++;
            }
            //V
            if(right and bottom)
            {
                ngl::Vec3 toRight = faces[i+1][j] - faces[i][j];
                ngl::Vec3 toBottom = faces[i][j-1] - faces[i][j];
                ngl::Vec3 n = toRight.cross(toBottom);
                n.normalize();
                normal += n;
                count++;
            }
            //<-
            if(bottom and left)
            {
                ngl::Vec3 toBottom = faces[i][j-1] - faces[i][j];
                ngl::Vec3 toLeft = faces[i-1][j] - faces[i][j];
                ngl::Vec3 n = toBottom.cross(toLeft);
                n.normalize();
                normal += n;
                count++;
            }
            //^
            if(left and top)
            {
                ngl::Vec3 toLeft = faces[i-1][j] - faces[i][j];
                ngl::Vec3 toTop = faces[i][j+1] - faces[i][j];
                ngl::Vec3 n = toLeft.cross(toTop);
                n.normalize();
                normal += n;
                count++;
            }

            normal /= static_cast<float>(count);
            normal.normalize();
            facenorms[i].push_back( normal );
        }
    }

    std::vector<ngl::Vec4> trimesh;
    std::vector<ngl::Vec3> normesh;
    std::vector<ngl::Vec2> uvmesh;

    for(int i = 0; i < faces.size(); ++i)
    {
        for(int j = 0; j < faces[i].size(); ++j)
        {
            terrainFace f = terrainVerticesToFace(
                        i,
                        j,
                        faces,
                        facenorms
                        );

            //t1
            trimesh.push_back( f.m_verts[2].m_pos );
            trimesh.push_back( f.m_verts[1].m_pos );
            trimesh.push_back( f.m_verts[0].m_pos );

            normesh.push_back( f.m_verts[2].m_norm );
            normesh.push_back( f.m_verts[1].m_norm );
            normesh.push_back( f.m_verts[0].m_norm );

            //t2
            trimesh.push_back( f.m_verts[3].m_pos );
            trimesh.push_back( f.m_verts[2].m_pos );
            trimesh.push_back( f.m_verts[1].m_pos );

            normesh.push_back( f.m_verts[3].m_norm );
            normesh.push_back( f.m_verts[2].m_norm );
            normesh.push_back( f.m_verts[1].m_norm );
        }
    }

    for(auto &vec : trimesh)
        uvmesh.push_back( ngl::Vec2(vec.m_x, vec.m_z) );

    std::cout << "constructTerrain end with\n";

    std::cout << trimesh.size() << " vertices\n" << normesh.size() << " normals\n" << uvmesh.size() << " uvs\n";

    /*for(auto &i : trimesh)
        std::cout << i << '\n';*/

    //exit(EXIT_SUCCESS);
    m_terrainVAOSize = trimesh.size();
    return createVAO(
                trimesh,
                normesh,
                uvmesh
                );
}

Scene::terrainFace Scene::terrainVerticesToFace( const int _x,
                                                 const int _y,
                                                 const std::vector<std::vector<ngl::Vec3> > &_facePositions,
                                                 const std::vector<std::vector<ngl::Vec3> > &_faceNormals
                                                 )
{
    //  2---3
    //  |   |
    //  0---1
    terrainFace face;

    face.m_verts[0].m_pos = ngl::Vec4(_x - 0.5f, 0.0f, _y - 0.5f, 1.0f);
    face.m_verts[1].m_pos = ngl::Vec4(_x + 0.5f, 0.0f, _y - 0.5f, 1.0f);
    face.m_verts[2].m_pos = ngl::Vec4(_x - 0.5f, 0.0f, _y + 0.5f, 1.0f);
    face.m_verts[3].m_pos = ngl::Vec4(_x + 0.5f, 0.0f, _y + 0.5f, 1.0f);

    std::pair<float, ngl::Vec3> v0 = generateTerrainFaceData( _x, _y, -1, -1, _facePositions, _faceNormals );
    std::pair<float, ngl::Vec3> v1 = generateTerrainFaceData( _x, _y, 1, -1, _facePositions, _faceNormals );
    std::pair<float, ngl::Vec3> v2 = generateTerrainFaceData( _x, _y, -1, 1, _facePositions, _faceNormals );
    std::pair<float, ngl::Vec3> v3 = generateTerrainFaceData( _x, _y, 1, 1, _facePositions, _faceNormals );

    face.m_verts[0].m_pos.m_y = v0.first;
    face.m_verts[1].m_pos.m_y = v1.first;
    face.m_verts[2].m_pos.m_y = v2.first;
    face.m_verts[3].m_pos.m_y = v3.first;

    face.m_verts[0].m_norm = v0.second;
    face.m_verts[1].m_norm = v1.second;
    face.m_verts[2].m_norm = v2.second;
    face.m_verts[3].m_norm = v3.second;

    return face;
}

std::pair<float, ngl::Vec3> Scene::generateTerrainFaceData(const int _x,
                                                           const int _y,
                                                           const int _dirX,
                                                           const int _dirY,
                                                           const std::vector<std::vector<ngl::Vec3>> &_facePositions,
                                                           const std::vector<std::vector<ngl::Vec3>> &_faceNormals
                                                           )
{
    float yCoord = _facePositions[_x][_y].m_y;
    ngl::Vec3 normal = _faceNormals[_x][_y];
    size_t count = 1;

    //Can we move in the horizontal direction?
    bool horizontal = (_x + _dirX) > 0 and (_x + _dirX) < _facePositions[0].size() - 1;
    //Can we move in the vertical direction?
    bool vertical = (_y + _dirY) > 0 and (_y + _dirY) < _facePositions.size() - 1;

    if(horizontal)
    {
        yCoord += _facePositions[_x + _dirX][_y].m_y;
        normal += _faceNormals[_x + _dirX][_y];
        count++;
    }
    if(vertical)
    {
        yCoord += _facePositions[_x][_y + _dirY].m_y;
        normal += _faceNormals[_x][_y + _dirY];
        count++;
    }
    if(vertical and horizontal)
    {
        yCoord += _facePositions[_x + _dirX][_y + _dirY].m_y;
        normal += _faceNormals[_x + _dirX][_y + _dirY];
        count++;
    }

    normal /= static_cast<float>(count);
    normal.normalize();

    return std::make_pair(
                yCoord / static_cast<float>(count),
                normal
                );
}
