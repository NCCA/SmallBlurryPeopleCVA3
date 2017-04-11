#include <algorithm>
#include <iostream>
#include <random>
#include <ngl/ShaderLib.h>
#include <ngl/Transformation.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Random.h>
#include <SDL2/SDL.h>

#include "Scene.hpp"
#include "Gui.hpp"
#include "Utility.hpp"
#include "Commands.hpp"

#include <ngl/NGLStream.h>

#include <ngl/AABB.h>

//This should be the same as the size of the map, for now.
const ngl::Vec2 waterDimensions (50.0f, 50.0f);

const int shadowResolution = 4096;
const int waterResolution = 1024;

Scene::Scene(ngl::Vec2 _viewport) :
    m_active(true),
    m_active_char_id(-1),
    m_mouse_trans_active(false),
    m_mouse_rot_active(false),
    m_centre_camera(false),
    m_mouse_prev_pos(0.0f, 0.0f),
    m_sunAngle(90.0f, 0.0f, 5.0f),
    m_day(80),
    m_state(GameState::START_MENU),
    m_game_started(false),
    m_movement_held{false},
    m_mouseSelectionBoxPosition( ngl::Vec3(), ngl::Vec3(), 0.75f),
    m_mouseSelectionBoxScale( ngl::Vec3(1.0f, 1.0f, 1.0f), ngl::Vec3(1.0f, 1.0f, 1.0f), 0.75f)
{
    m_prefs = Prefs::instance();
    AssetStore *store = AssetStore::instance();
    m_viewport = _viewport;

    float aspect = _viewport.m_x / _viewport.m_y;
    m_cam.setAspect( aspect );
    m_cam.calculateProjectionMat();
    m_cam.clearTransforms();
    m_cam.calculateViewMat();

    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    createShader("deferredLight", "vertScreenQuad", "fragBasicLight");
    createShader("diffuse", "vertDeferredDataInstanced", "fragDeferredDiffuse");
    createShader("colour", "vertDeferredData", "fragBasicColour");
    createShader("charPick", "vertDeferredDataChar", "fragPickChar");
    createShader("terrain", "vertDeferredData", "fragTerrain");
    createShader("terrainPick", "vertDeferredData", "fragPickTerrain");
    createShader("selection", "vertDeferredData", "fragBasicColour");
    createShader("sky", "vertScreenQuad", "fragSky");
    createShader("shadowDepth", "vertDeferredData", "fragShadowDepth");
    createShader("shadowDepthInstanced", "vertDeferredDataInstanced", "fragShadowDepth");
    createShader("button", "buttonVert", "buttonFrag", "buttonGeo");
    createShader("water", "vertWater", "fragWater", "", "tescWater", "teseWater");
    createShader("waterDisplacement", "vertScreenQuad", "fragWaterDisplacement");
    createShader("waterDisplacementNormal", "vertScreenQuad", "fragWaterDisplacementNormal");
    createShader("bokeh", "vertScreenQuad", "fragBokehBlur");
    createShader("debugTexture", "vertScreenQuadTransform", "fragDebugTexture");
    createShader("mousebox", "vertDeferredData", "fragMousebox");

    slib->use("sky");
    slib->setRegisteredUniform("viewport", m_viewport);

    slib->use("deferredLight");
    slib->setRegisteredUniform("waterLevel", m_grid.getGlobalWaterLevel() / m_terrainHeightDivider);

    slib->use("water");
    GLint tlvl = 0;
    glGetIntegerv( GL_MAX_TESS_GEN_LEVEL, &tlvl );
    slib->setRegisteredUniform("maxTessLevel", tlvl);
    std::cout << "Max water tesselation level set to " << tlvl << '\n';
    slib->setRegisteredUniform("pixelstep", ngl::Vec2(1.0f, 1.0f) / m_prefs->getIntPref("WATER_MAP_RES"));
    slib->setRegisteredUniform("viewport", m_viewport);

    slib->use("waterDisplacementNormal");
    slib->setRegisteredUniform("pixelstep", ngl::Vec2(1.0f, 1.0f) / m_prefs->getIntPref("WATER_MAP_RES"));

    slib->use("bokeh");
    slib->setRegisteredUniform("bgl_dim", m_viewport);

    //reads file with list of names
    readNameFile();
    //creates characters with random names
		int m_char_num = m_prefs->getIntPref("NUMBER_OF_CHARACTERS");
		for (int i = 0; i<m_char_num; i++)
    {
        createCharacter();
    }

    initialiseFramebuffers();

    m_shadowMat.assign(3, ngl::Mat4());

    ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();

    prim->createTrianglePlane("plane",14,14,80,80,ngl::Vec3(0,1,0));
    prim->createSphere( "sphere", 0.1, 12 );

    std::cout << "Loading assets...\n";

    store->loadMesh("mountain", "mountain/mountain.obj");
    store->loadTexture("mountain_d", "mountain/mountain_diff.png");

    //playing with trees and houses and such
    store->loadMesh("debugSphere", "sphere.obj");
    store->loadMesh("debugBox", "box.obj");

    store->loadMesh("tree", "tree/tree.obj");
    store->loadTexture("tree_d", "tree/tree_d.png");

    store->loadMesh("house", "house/stilt_house.obj" );
		store->loadTexture("house_d", "house/stilt_house_diff.tif" );
		store->loadMesh("foundation_A", "house/start_building.obj");
		store->loadTexture("foundation_A_d", "house/start_building_diff.tif");
    store->loadMesh("foundation_B", "house/mid_way_building.obj");
		store->loadTexture("foundation_B_d", "house/mid_way_building_diff.tif");

		store->loadMesh("storehouse", "storeHouse/storehouse.obj");
		store->loadTexture("storehouse_d", "storeHouse/storehouse_diff.png");
		store->loadMesh("foundation_C", "house/start_building.obj");
		store->loadTexture("foundation_C_d", "house/start_building_diff.tif");
		store->loadMesh("foundation_D", "house/mid_way_building.obj");
		store->loadTexture("foundation_D_d", "house/mid_way_building_diff.tif");

    store->loadMesh("person", "person/person.obj");

    store->loadTexture("grass", "terrain/grass.png");
    store->loadTexture("rock", "terrain/rock.png");
    store->loadTexture("snow", "terrain/snow.png");

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
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Gui *gui = Gui::instance();
    gui->init(this, _viewport, "button");
    std::cout << "Scene constructor complete.\n";

    initMeshInstances();

    glGenVertexArrays(1, &m_debugVAO);
    glGenBuffers(1, &m_debugVBO);

    //Create and reserve light buffer.
    m_pointLights.reserve(m_maxLights);
    glGenBuffers(1, &m_lightBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_lightBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Light) * m_maxLights, &m_pointLights[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Scene::initMeshInstances()
{
    int meshCount = 0;
    //m_meshPositions.clear()
		m_meshPositions.assign(static_cast<int>(TileType::FOUNDATION_D) + 1, std::vector<ngl::Vec3>());
    for(int i = 0; i < m_grid.getW(); ++i)
        for(int j = 0; j < m_grid.getH(); ++j)
        {
            int index = static_cast<int>( m_grid.getTileType(i, j) );
            m_meshPositions.at( index ).push_back(ngl::Vec3(
                                                      i+0.5,
                                                      m_grid.getInterpolatedHeight(i+0.5, j+0.5) / m_terrainHeightDivider,
                                                      j+0.5
                                                      ));
            meshCount++;
        }

    //Generate TBO for mesh instancing.
    //ngl::Random * rng = ngl::Random::instance();
    std::vector<ngl::Mat4> transforms;
    transforms.reserve( meshCount );
    for(auto &slot : m_meshPositions)
        for(auto &vec : slot)
        {
            ngl::Mat4 m;
            m.translate( vec.m_x, vec.m_y, vec.m_z );
            transforms.push_back( m );
        }

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_TEXTURE_BUFFER, buf);
    glBufferData(GL_TEXTURE_BUFFER, transforms.size() * sizeof(ngl::Mat4), &transforms[0].m_00, GL_STATIC_DRAW);

    glGenTextures(1, &m_instanceTBO);
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture(GL_TEXTURE_BUFFER, m_instanceTBO);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, buf);
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
    m_pickBuffer.addTexture( "terrainpos", GL_RGBA, GL_RGBA16F, GL_COLOR_ATTACHMENT0 );
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

    std::cout << "Initalising displacement framebuffer to " << waterResolution << " by " << waterResolution << '\n';
    m_displacementBuffer.initialise(waterResolution, waterResolution);
    m_displacementBuffer.addTexture("waterDisplacement", GL_RED, GL_R16F, GL_COLOR_ATTACHMENT0);
    m_displacementBuffer.addTexture("waterNormal", GL_RGBA, GL_RGBA16F, GL_COLOR_ATTACHMENT1);
    if(!m_displacementBuffer.checkComplete())
    {
        std::cerr << "Uh oh! Framebuffer incomplete! Error code " << glGetError() << '\n';
        exit(EXIT_FAILURE);
    }
    m_displacementBuffer.unbind();

    std::cout << "Initalising post effects framebuffer to " << m_viewport.m_x << " by " << m_viewport.m_y << '\n';
    m_postEffectsBuffer.initialise(m_viewport.m_x, m_viewport.m_y);
    m_postEffectsBuffer.addTexture("reflection", GL_RGBA, GL_RGBA, GL_COLOR_ATTACHMENT0);
    m_postEffectsBuffer.addTexture("sceneColour", GL_RGBA, GL_RGBA16F, GL_COLOR_ATTACHMENT1);
    m_postEffectsBuffer.addTexture( "linearDepth", GL_RED, GL_R16F, GL_COLOR_ATTACHMENT2 );
    m_postEffectsBuffer.addDepthAttachment("depth");
    if(!m_postEffectsBuffer.checkComplete())
    {
        std::cerr << "Uh oh! Framebuffer incomplete! Error code " << glGetError() << '\n';
        exit(EXIT_FAILURE);
    }
    m_postEffectsBuffer.unbind();
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
	//pick random name
  std::random_device rnd;
  std::mt19937 mt_rand(rnd());
  std::uniform_int_distribution<int> nameNo(0,numberNames - 1);
  int name_chosen = nameNo(mt_rand);
	//create character with random name
  m_characters.push_back(Character(&m_grid, &m_world_inventory, m_file_names[name_chosen]));
	//remove name from list so no multiples
	m_file_names.erase(m_file_names.begin() + name_chosen);
}

void Scene::update()
{
	//Gui::instance()->updateNotifications();
	if (m_grid.hasChanges())
	{
			initMeshInstances();
			m_grid.resetHasChanges();
	}

	if(m_state == GameState::MAIN)
	{
    //translates
    if(m_mouse_trans_active)
    {
      //Compute distance to mouse origin
      ngl::Vec2 mouse_distance = Utility::getMousePos();
      mouse_distance -= m_mouse_trans_origin;

      m_mouse_trans_origin = Utility::getMousePos();

      //Move the camera based on mouse translation.
      float cam_to_pivot_height = abs(m_cam.getPivot().m_y - m_cam.getPos().m_y) + 20;
      cam_to_pivot_height *= cam_to_pivot_height * 0.00003f;
      m_cam.moveRight( mouse_distance.m_x * cam_to_pivot_height);
      m_cam.moveForward( -mouse_distance.m_y * cam_to_pivot_height);
      //---

      if(mouse_distance.lengthSquared() > Utility::Sqr(8.0f))
        m_centre_camera = false;
    }
    //rotates
    else if(m_mouse_rot_active)
    {
      int mouse_origin = 0;
      int mouse_distance = 0;
      SDL_GetMouseState(&mouse_origin, nullptr);
      mouse_distance = mouse_origin - m_mouse_rot_origin;
      m_mouse_rot_origin = mouse_origin;

      //Rotate the camera based on mouse movement.
      m_cam.rotate(0.0f, mouse_distance * 0.125f);
      //---
    }

    //---

    if(m_centre_camera == true)
    {
      for (Character &character : m_characters)
        if (character.isActive())
          m_cam.setPos(-character.getPos());
    }
    else
    {
      m_cam.moveScreenSpace(getCamMoveVec());
    }

    //Terrain-height correction
    ngl::Vec3 cxyz = m_cam.getPos();
    int x = Utility::clamp(std::round(cxyz.m_x),0,m_grid.getW()-1);
    int y = Utility::clamp(std::round(cxyz.m_z),0,m_grid.getH()-1);
    cxyz.m_y = m_grid.getTileHeight(x, y) / m_terrainHeightDivider;
    ngl::Vec3 cp = m_cam.getTargPos();
    //Grab the y component of the current tile, use that as the target y for the camera.
    m_cam.setPos( ngl::Vec3(cp.m_x, -cxyz.m_y - 0.5f, cp.m_z) );
    //---


    //Recalculate view matrix.
    m_cam.updateSmoothCamera();
    m_cam.clearTransforms();
    m_cam.calculateViewMat();
    //---

    //Update mouse selection box.
    m_mouseSelectionBoxPosition.update();
    m_mouseSelectionBoxScale.update();

    for(Character &character : m_characters)
    {
      character.update();
			if (character.isSleeping() && character.getID() == m_active_char_id)
			{
				 std::cout<<"sleeping"<<std::endl;
				 m_active_char_id = -1;
				 Gui::instance()->updateActiveCharacter();
			 }
		 }


    //m_sunAngle.m_x = 150.0f;
    m_sunAngle.m_z = 30.0f - 25.0f * sinf(m_season * M_PI - M_PI / 2.0f);
    m_sunAngle.m_x += 0.01f;
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

  m_pointLights.clear();

  ngl::Vec3 off (0.0f, 0.5f, 0.0f);
  for(auto &vec : m_meshPositions[static_cast<int>(TileType::HOUSE)])
      m_pointLights.push_back( Light(vec + off, ngl::Vec3(1.0f, 0.8f, 0.4f), 0.5f) );
  for(auto &vec : m_meshPositions[static_cast<int>(TileType::STOREHOUSE)])
      m_pointLights.push_back( Light(vec + off, ngl::Vec3(1.0f, 0.8f, 0.4f), 0.5f) );
  for(auto &c : m_characters)
  {
      ngl::Vec3 vec = c.getPos() + off;
      vec.m_y /= m_terrainHeightDivider;
      m_pointLights.push_back( Light(vec + off, ngl::Vec3(1.0f, 0.8f, 0.4f), 0.25f) );
  }
}

//I'm sorry this function is so long :(
//Order of operations:
//  --
//  SETUP                   : Sets up the scene ready for drawing.
//  --
//  UTILITY ID DRAW         : Draws out utility textures from the scene. Currently this is CHARACTER IDS and TERRAIN WORLD POSITION.
//                            This can all be queried to find what the user is mousing-over.
//  SHADOW PASS             : This section draws shadow maps for the sun/moon. These maps are later referenced when lighting is applied.
//  REFLECTIONS             : The scene is drawn upside down, to be used for reflections later on. This is accomplished by scaling the camera by -1 in the y
//                            then drawing as usual.
//  DISPLACEMENT            : The textures that drive the water are generated. This is done in two passes. First, the displacement map is created,
//                            then this texture is fed into another draw call to generate a normal map.
//  --
//  RAW DATA PASS           : The heart of the deferred rendering pipeline, the scene is drawn to a set of buffers, to which lighting is later applied.
//  SKY                     : The sky is drawn to the back buffer.
//  LIGHTING                : Lighting is applied to our data from RAW DATA PASS.
//  --
//  DEFERRED BLUR PASS      : The first of the post-processing stages, the lit scene from the LIGHTING stage is run through a blur shader.
//  FORWARD SHADING         : Forward-shaded elements are drawn. Currently this is only the water, which is drawn into the post effects framebuffer,
//                            ready for a DOF pass.
//  DRAW BOX WHERE MOUSE IS : Inelegantly named, this stage draws a blue box where the mouse is. On Felix's suggestion, there are several
//                            conditions to it being drawn, such as the mouse being over the world/not clicking and dragging.
//  FORWARD BLUR PASS       : Blurs forwarded shaded elements, in the same way that DEFERRED BLUR PASS operates.
//  BUTTONS                 : Draws the GUI to the screen after everything else has completed.
//  DEBUG DRAW              : Originally used to debug the CSM shadowing I was using, this is now largely obsolete. Should be removed before submission,
//                            but for now it can be used to draw points stored in m_debugPoints.

void Scene::draw()
{
    //---------------------------//
    //           SETUP           //
    //---------------------------//
    m_debugPoints.clear();

    ngl::ShaderLib * slib = ngl::ShaderLib::instance();
    glClearColor(0.0,0.0,0.0,0.0);

    m_transform.reset();
    glEnable(GL_DEPTH_TEST);

    ngl::Vec4 mouseWorldPos = getTerrainPosAtMouse();

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
    for(auto &ch : m_characters)
    {
			if(ch.isSleeping() == false)
			{
        ngl::Vec3 pos = ch.getPos();
        pos.m_y /= m_terrainHeightDivider;
        m_transform.setPosition(pos);
        slib->setRegisteredUniform("id", ch.getID());
        drawAsset( "person", "", "");
			}
    }

    m_pickBuffer.unbind();

    //---------------------------//
    //  SHADOW PASS  //
    //---------------------------//
    ngl::Vec3 s = m_sunDir;
    //Flip if the moon is up.
    if(s.dot(ngl::Vec3( 0.0f, 1.0f, 0.0f )) < 0.0f)
        s = -s;

    glViewport(0, 0, m_prefs->getIntPref("SHADOW_MAP_RES"), m_prefs->getIntPref("SHADOW_MAP_RES"));

    //The intervals at which we will draw into shadow buffers.
    std::vector<float> cascadeDistances = {0.5f, 16.0f, 64.0f, 128.0f};

    auto boxes = generateOrthoShadowMatrices( cascadeDistances );

    //Render a pass from each AABB
    size_t index = 0;
    for(size_t i = 0; i < boxes.first.size(); ++i)
    {
        //std::cout << "Shadowpass for " << index << '\n';
        shadowPass( boxes.first[i], boxes.second[i], i );
        index++;
    }

    glViewport(0, 0, m_viewport.m_x, m_viewport.m_y);

    //---------------------------//
    //         REFLECTIONS       //
    //---------------------------//
    glCullFace(GL_FRONT);
    m_transform.reset();

    m_mainBuffer.bind();
    m_mainBuffer.activeColourAttachments();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ngl::Mat4 camFlip;
    camFlip.scale(1.0f, -1.0f, 1.0f);
    ngl::Mat4 camMove;
    camMove.translate(0.0f, -2.0f * m_grid.getGlobalWaterLevel() / m_terrainHeightDivider, 0.0f);

    //Flip camera upside down.

    //m_cam.clearTransforms();
    //m_cam.calculateViewMat();
    m_cam.immediateTransform(camFlip);
    m_cam.immediateTransform(camMove);

    drawTerrain();

    drawMeshes();

    m_postEffectsBuffer.bind();
    m_postEffectsBuffer.activeColourAttachments({GL_COLOR_ATTACHMENT0});
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    drawSky();

    camMove.translate(0.0f, 2.0f * m_grid.getGlobalWaterLevel() / m_terrainHeightDivider, 0.0f);

    //Flip camera right way up.
    //m_cam.clearTransforms();
    //m_cam.calculateViewMat();
    m_cam.immediateTransform(camMove);
    m_cam.immediateTransform(camFlip);

    //Light reflections
    glBindVertexArray(m_screenQuad);

    slib->use("deferredLight");
    GLuint id = slib->getProgramID("deferredLight");
    slib->setRegisteredUniform("sunDir", m_sunDir );
    slib->setRegisteredUniform( "sunInts", Utility::clamp(m_sunDir.dot(ngl::Vec3(0.0f, 1.0f, 0.0f)), 0.2f, 1.0f) * 1.8f );
    slib->setRegisteredUniform( "moonInts", Utility::clamp(m_sunDir.dot(ngl::Vec3(0.0f, -1.0f, 0.0f)), 0.2f, 1.0f) );
    slib->setRegisteredUniform( "iGlobalTime", m_sunAngle.m_x * 8.0f );
    slib->setRegisteredUniform( "directionalLightCol", m_directionalLightCol );
    slib->setRegisteredUniform( "shadowMatrix[0]", m_shadowMat[0] );
    slib->setRegisteredUniform( "shadowMatrix[1]", m_shadowMat[1] );
    slib->setRegisteredUniform( "shadowMatrix[2]", m_shadowMat[2] );

    slib->setRegisteredUniform( "camPos", ngl::Vec4(m_cam.getPos()) );
    for( size_t i = 0; i < cascadeDistances.size(); ++i )
        slib->setRegisteredUniform( "cascades[" + std::to_string(i) + "]", cascadeDistances[i] );

    m_mainBuffer.bindTexture(id, "diffuse", "diffuse", 0);
    m_mainBuffer.bindTexture(id, "normal", "normal", 1);
    m_mainBuffer.bindTexture(id, "position", "position", 2);
    m_mainBuffer.bindTexture( id, "linearDepth", "linearDepth", 3 );
    m_shadowBuffer.bindTexture( id, "depth[0]", "shadowDepths[0]", 4 );
    m_shadowBuffer.bindTexture( id, "depth[1]", "shadowDepths[1]", 5 );
    m_shadowBuffer.bindTexture( id, "depth[2]", "shadowDepths[2]", 6 );

    glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    m_postEffectsBuffer.unbind();

    //---------------------------//
    //       DISPLACEMENT       //
    //---------------------------//
    //Base
    m_displacementBuffer.bind();
    m_displacementBuffer.activeColourAttachments({GL_COLOR_ATTACHMENT0});
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, m_prefs->getIntPref("WATER_MAP_RES"), m_prefs->getIntPref("WATER_MAP_RES"));

    glBindVertexArray(m_screenQuad);

    slib->use("waterDisplacement");
    slib->setRegisteredUniform("iGlobalTime", m_sunAngle.m_x * 8.0f);

    glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);

    //Normals
    m_displacementBuffer.activeColourAttachments({GL_COLOR_ATTACHMENT1});

    slib->use("waterDisplacementNormal");
    id = slib->getProgramID("waterDisplacementNormal");
    m_displacementBuffer.bindTexture(id, "waterDisplacement", "displacement", 0);

    glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);

    m_displacementBuffer.unbind();

    glViewport(0, 0, m_viewport.m_x, m_viewport.m_y);

    //---------------------------//
    //  RAW DATA PASS //
    //---------------------------//
    m_transform.reset();
    m_mainBuffer.bind();
    m_mainBuffer.activeColourAttachments();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawTerrain();

    drawMeshes();

    m_mainBuffer.unbind();

    //This gives opengl error 1282. I guess you can't specify draw buffers for the back buffer?
    //m_mainBuffer.activeColourAttachments( {GL_COLOR_ATTACHMENT0} );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    //---------------------------//
    //            SKY            //
    //---------------------------//
    drawSky();

    //---------------------------//
    //          LIGHTING         //
    //---------------------------//
    //Pass lights to shader.
    m_pointLights.push_back(
                Light(mouseWorldPos + ngl::Vec4(0.0, 2.0, 0.0, 0.0),ngl::Vec3(0.0,1.0,1.0),0.5f)
                );

    glBindBuffer(GL_UNIFORM_BUFFER, m_lightBuffer);
    GLvoid * dat = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(dat, &m_pointLights[0], sizeof(Light) * std::min( m_pointLights.size(), static_cast<size_t>(m_maxLights)));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    m_postEffectsBuffer.bind();
    m_postEffectsBuffer.activeColourAttachments({GL_COLOR_ATTACHMENT1});
    glClear(GL_COLOR_BUFFER_BIT);

    slib->use("deferredLight");
    id = slib->getProgramID("deferredLight");
    slib->setRegisteredUniform("sunDir", m_sunDir );
    slib->setRegisteredUniform( "sunInts", Utility::clamp(m_sunDir.dot(ngl::Vec3(0.0f, 1.0f, 0.0f)), 0.0f, 1.0f) * 1.8f );
    slib->setRegisteredUniform( "moonInts", Utility::clamp(m_sunDir.dot(ngl::Vec3(0.0f, -1.0f, 0.0f)), 0.0f, 1.0f) );
    slib->setRegisteredUniform("iGlobalTime", m_sunAngle.m_x * 8.0f);
    slib->setRegisteredUniform( "directionalLightCol", m_directionalLightCol );
    slib->setRegisteredUniform( "shadowMatrix[0]", m_shadowMat[0] );
    slib->setRegisteredUniform( "shadowMatrix[1]", m_shadowMat[1] );
    slib->setRegisteredUniform( "shadowMatrix[2]", m_shadowMat[2] );
    slib->setRegisteredUniform( "iP", m_cam.getP().inverse() );
    slib->setRegisteredUniform( "iMV", m_cam.getV().inverse() );

    //Draw god rays here.
    //To-do: Add preference bool to control drawing of god rays.
    slib->setRegisteredUniform( "drawGodRays", true);

    slib->setRegisteredUniform( "camPos", ngl::Vec4(m_cam.getPos()) );
    for( size_t i = 0; i < cascadeDistances.size(); ++i )
        slib->setRegisteredUniform( "cascades[" + std::to_string(i) + "]", cascadeDistances[i] );

    m_mainBuffer.bindTexture(id, "diffuse", "diffuse", 0);
    m_mainBuffer.bindTexture(id, "normal", "normal", 1);
    m_mainBuffer.bindTexture(id, "position", "position", 2);
    m_mainBuffer.bindTexture( id, "linearDepth", "linearDepth", 3 );
    m_shadowBuffer.bindTexture( id, "depth[0]", "shadowDepths[0]", 4 );
    m_shadowBuffer.bindTexture( id, "depth[1]", "shadowDepths[1]", 5 );
    m_shadowBuffer.bindTexture( id, "depth[2]", "shadowDepths[2]", 6 );

    GLuint lightBlockIndex = glGetUniformBlockIndex( id, "lightBuffer" );
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_lightBuffer);
    glUniformBlockBinding(id, lightBlockIndex, 1);

    slib->setRegisteredUniform("lbufLen", static_cast<int>(std::min(m_maxLights, m_pointLights.size())));

    glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);

    m_postEffectsBuffer.unbind();

    //---------------------------//
    //    DEFERRED BLUR PASS     //
    //---------------------------//
    slib->use("bokeh");

    id = slib->getProgramID("bokeh");
    m_postEffectsBuffer.bindTexture(id, "sceneColour", "bgl_RenderedTexture", 0);
    m_mainBuffer.bindTexture(id, "linearDepth", "bgl_DepthTexture", 1);
    slib->setRegisteredUniform("focalDepth", m_cam.getFocalDepth());

    glBindVertexArray(m_screenQuad);
    glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);

    //---------------------------//
    //      FORWARD SHADING      //
    //---------------------------//
    m_postEffectsBuffer.bind();
    m_postEffectsBuffer.activeColourAttachments({GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    //Copy depth buffer from main buffer to back buffer.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_mainBuffer.getID());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_postEffectsBuffer.getID());
    glBlitFramebuffer(0, 0, m_viewport.m_x, m_viewport.m_y, 0, 0, m_viewport.m_x, m_viewport.m_y,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    slib->use("water");
    slib->setRegisteredUniform("mouseWorldPos", mouseWorldPos);
    slib->setRegisteredUniform("gEyeWorldPos", m_cam.getPos());
    slib->setRegisteredUniform("iGlobalTime", m_sunAngle.m_x * 8.0f);
    slib->setRegisteredUniform("lightDir", m_sunDir);
    slib->setRegisteredUniform("camPos", m_cam.getPos());
    slib->setRegisteredUniform( "sunInts", Utility::clamp(m_sunDir.dot(ngl::Vec3(0.0f, 1.0f, 0.0f)), 0.0f, 1.0f) * 1.8f );
    slib->setRegisteredUniform( "moonInts", Utility::clamp(m_sunDir.dot(ngl::Vec3(0.0f, -1.0f, 0.0f)), 0.0f, 1.0f) );
    slib->setRegisteredUniform( "directionalLightCol", m_directionalLightCol );
    slib->setRegisteredUniform("waterDimensions", waterDimensions);

    id = slib->getProgramID("water");
    m_displacementBuffer.bindTexture( id, "waterDisplacement", "displacement", 0 );
    m_mainBuffer.bindTexture( id, "position", "terrainPos", 1 );
    m_postEffectsBuffer.bindTexture( id, "reflection", "waterReflection", 2 );
    m_displacementBuffer.bindTexture(id, "waterNormal", "normal", 3);
    glBindVertexArray(m_unitSquareVAO);
    m_transform.reset();

    //Unit size of each water tile.
    int scale = 10;
    for(int i = 0; i < m_grid.getW(); i += scale)
    {
        for(int j = 0; j < m_grid.getH(); j += scale)
        {
            ngl::Vec3 pos (i, m_grid.getGlobalWaterLevel() / m_terrainHeightDivider, j);
            bounds waterBounds;
            waterBounds.first = pos + ngl::Vec3(-scale, -1.0f, -scale);
            waterBounds.second = pos + ngl::Vec3(scale, 1.0f, scale);

            bool br = false;
            for(auto &b : boxes.first)
                br = (br or Utility::boxIntersectBox(b, waterBounds));

            if(!br)
                continue;

            m_transform.setScale(scale, scale, 1.0f);
            m_transform.setRotation(90.0f, 0.0f, 0.0f);
            m_transform.setPosition(pos);
            slib->setRegisteredUniform("MV", m_transform.getMatrix() * m_cam.getV());
            loadMatricesToShader();
            glDrawArraysEXT(GL_PATCHES, 0, 4);
        }
    }

    glBindVertexArray(0);

    m_postEffectsBuffer.unbind();

    //---------------------------//
    //  DRAW BOX WHERE MOUSE IS  //
    //---------------------------//
    Gui * g = Gui::instance();
    int charid = getCharIDAtMouse();

    if(m_state == GameState::MAIN and
            !m_mouse_trans_active and
            !m_mouse_rot_active and
            !g->mousePos( Utility::getMousePos() ))
    {
        glDisable(GL_CULL_FACE);
        m_transform.reset();

        ngl::Vec4 tp = getTerrainPosAtMouse();
        ngl::Vec4 p;
        //Player is not hovering over a character.
        if(charid == -1)
        {
            p = tp;

            p.m_x = floor(p.m_x) + 0.5;
            p.m_z = floor(p.m_z) + 0.5;
            p.m_y = m_grid.getTileHeight(p.m_x, p.m_z);
            p.m_y = std::max(p.m_y, static_cast<float>(m_grid.getGlobalWaterLevel()));
            p.m_y /= m_terrainHeightDivider;

            m_transform.setScale( m_mouseSelectionBoxScale.get() );
            m_transform.setPosition( m_mouseSelectionBoxPosition.get() );
            m_mouseSelectionBoxScale.setEnd(ngl::Vec3(1.0f, 2.0f, 1.0f));
        }
        else
        {
            for(auto &c : m_characters)
                if(c.getID() == charid)
                    p = c.getPos();

            p.m_y /= m_terrainHeightDivider;

            m_transform.setScale( m_mouseSelectionBoxScale.get() );
            m_transform.setPosition( m_mouseSelectionBoxPosition.get() );
            m_mouseSelectionBoxScale.setEnd(ngl::Vec3(0.25f, 8.0f, 0.25f));
        }
        p.m_y -= 1.0f;
        m_mouseSelectionBoxPosition.setEnd(ngl::Vec3(p.m_x, p.m_y, p.m_z));

        m_postEffectsBuffer.bind();
        m_postEffectsBuffer.activeColourAttachments({GL_COLOR_ATTACHMENT1});

        if(tp.m_x != 0.0f and tp.m_z != 0.0f)
        {
            slib->use("mousebox");
            slib->setRegisteredUniform("base", p.m_y);
            slib->setRegisteredUniform("m", 0.025f);
            slib->setRegisteredUniform("colour", ngl::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
            drawAsset("debugBox", "", "mousebox");

            AssetStore * a = AssetStore::instance();
            ngl::Obj * k = a->getModel("debugBox");
            k->bindVAO();
            slib->setRegisteredUniform("m", 0.0125f);
            glLineWidth(5.0f);
            glDrawArraysEXT(GL_LINE_STRIP, 0, k->getMeshSize());
            glLineWidth(1.0f);
        }
        glBindVertexArray(0);

        m_postEffectsBuffer.unbind();

        glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
    }

    //---------------------------//
    //     FORWARD BLUR PASS     //
    //---------------------------//
    slib->use("bokeh");

    id = slib->getProgramID("bokeh");
    m_postEffectsBuffer.bindTexture(id, "sceneColour", "bgl_RenderedTexture", 0);
    m_postEffectsBuffer.bindTexture(id, "linearDepth", "bgl_DepthTexture", 1);

    glBindVertexArray(m_screenQuad);
    glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);

    //---------------------------//
    //          BUTTONS          //
    //---------------------------//

    Gui::instance()->drawButtons();

    //---------------------------//
    //         DEBUG DRAW        //
    //---------------------------//

    //It'd be good to have some kind of m_debugViewModeGLuint to control this section.

    /*glBindVertexArray(m_screenQuad);

    for(int i = 0; i < 3; ++i)
    {
        m_transform.reset();
        m_transform.setPosition(-0.75 + 0.55 * i, -0.75, 0.0);
        m_transform.setScale(0.25, 0.25, 1.0);

        slib->use("debugTexture");
        m_shadowBuffer.bindTexture( id, "depth[" + std::to_string(i) + "]", "tex", 0 );
        slib->setRegisteredUniform( "M", m_transform.getMatrix() );

        glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);
    }

    slib->use("colour");
    for(auto &p : m_debugPoints)
        p.m_w = 1.0;

    glBindBuffer(GL_ARRAY_BUFFER, m_debugVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(ngl::Vec4) * m_debugPoints.size(),
                 &m_debugPoints[0].m_x,
            GL_STATIC_DRAW
            );

    glBindVertexArray(m_debugVAO);
    setBufferLocation(m_debugVBO, 0, 4);

    m_transform.reset();
    loadMatricesToShader();

    slib->setRegisteredUniform("colour", ngl::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
    glDrawArraysEXT(GL_LINES, 0, m_debugPoints.size() / 2);
    slib->setRegisteredUniform("colour", ngl::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    glDrawArraysEXT(GL_LINES, m_debugPoints.size() / 2, m_debugPoints.size() / 2);

    slib->setRegisteredUniform("colour", ngl::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    for(auto &b : boxes.first)
    {
        ngl::BBox bo (b.first.m_x, b.second.m_x,
                      b.first.m_y, b.second.m_y,
                      b.first.m_z, b.second.m_z);
        loadMatricesToShader();
        bo.draw();
    }

    m_transform.setPosition(m_cam.getPivot());
    drawAsset("debugSphere", "", "colour");*/

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void Scene::drawSky()
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    glBindVertexArray(m_screenQuad);

    slib->use("sky");
    slib->setRegisteredUniform("iMV", m_cam.getV().inverse());
    slib->setRegisteredUniform("iP", m_cam.getP().inverse());
    slib->setRegisteredUniform("camPos", m_cam.getPos());
    slib->setRegisteredUniform( "sunDir", m_sunDir );

    slib->setRegisteredUniform("surfaceHeight", 0.8f + 0.2f * m_sunDir.dot(ngl::Vec3(0.0f, -1.0f, 0.0f)));

    glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);
}

void Scene::drawTerrain()
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();
    AssetStore *store = AssetStore::instance();
    slib->use("terrain");
    bindTextureToShader("terrain", store->getTexture("grass"), "grass", 0);
    bindTextureToShader("terrain", store->getTexture("rock"), "rock", 1);
    bindTextureToShader("terrain", store->getTexture("snow"), "snow", 2);

    float waterLevel = m_grid.getGlobalWaterLevel() / m_terrainHeightDivider;
    slib->setRegisteredUniform( "waterlevel", waterLevel);
    float snowLevel = m_grid.getGlobalMountainHeight() / m_terrainHeightDivider;
    float difference = snowLevel - waterLevel;
    float snow = 0.5f * difference * sinf(m_season * 2.0f * M_PI - M_PI / 2.0f) + 0.5f * difference + waterLevel;
    slib->setRegisteredUniform( "snowline", snow);

    glBindVertexArray(m_terrainVAO);
    loadMatricesToShader();
    glDrawArraysEXT(GL_TRIANGLES, 0, m_terrainVAOSize);
    glBindVertexArray(0);
}

void Scene::drawMeshes()
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    slib->use("diffuse");

    int offset = 0;
    for(size_t i = 0; i < m_meshPositions.size(); ++i)
    {
        int instances = m_meshPositions[i].size();
        switch( i )
        {
        case static_cast<int>(TileType::TREES):
            drawInstances( "tree", "tree_d", "diffuse", instances, offset );
            break;
        case static_cast<int>(TileType::MOUNTAINS):
            drawInstances( "mountain", "mountain_d", "diffuse", instances, offset );
            break;
        case static_cast<int>(TileType::STOREHOUSE):
            drawInstances( "storehouse", "storehouse_d", "diffuse", instances, offset );
            break;
        case static_cast<int>(TileType::HOUSE):
						drawInstances( "house", "house_d", "diffuse", instances, offset);
            break;
				case static_cast<int>(TileType::FOUNDATION_A):
						drawInstances("foundation_A", "foundation_A_d", "diffuse", instances, offset);
						break;
				case static_cast<int>(TileType::FOUNDATION_B):
						drawInstances("foundation_B", "foundation_B_d", "diffuse", instances, offset);
						break;
				case static_cast<int>(TileType::FOUNDATION_C):
						drawInstances("foundation_C", "foundation_C_d", "diffuse", instances, offset);
						break;
				case static_cast<int>(TileType::FOUNDATION_D):
						drawInstances("foundation_D", "foundation_D_d", "diffuse", instances, offset);
						break;
        default:
            break;
        }
        offset += instances;
    }

    for(auto &character : m_characters)
    {
			if(character.isSleeping() == false)
			{
					ngl::Vec3 pos = character.getPos();
					pos.m_y /= m_terrainHeightDivider;
					m_transform.setPosition(pos);
					slib->use("colour");
					slib->setRegisteredUniform("colour", ngl::Vec4(character.getColour(),1.0f));
					drawAsset( "person", "", "colour");
			}
    }
}

void Scene::drawMeshes(const std::vector<bounds> &_frustumBoxes)
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    slib->use("diffuse");

    for(size_t i = 0; i < m_meshPositions.size(); ++i)
        for(auto &vec : m_meshPositions[i])
        {
            bool br = false;
            for(auto &fb : _frustumBoxes)
                br = (br or Utility::pointInBox(fb, vec));
            if(!br)
                continue;

            m_transform.setPosition(vec);
            switch( i )
            {
            case static_cast<int>(TileType::TREES):
                drawAsset( "tree", "tree_d", "diffuse" );
                break;
            case static_cast<int>(TileType::MOUNTAINS):
                drawAsset( "mountain", "mountain_d", "diffuse" );
                break;
            case static_cast<int>(TileType::STOREHOUSE):
                drawAsset( "storehouse", "storehouse_d", "diffuse" );
                break;
            case static_cast<int>(TileType::HOUSE):
								drawAsset( "house", "house_d", "diffuse");
                break;
						case static_cast<int>(TileType::FOUNDATION_A):
								drawAsset("foundation_A", "foundation_A_d", "diffuse");
								break;
						case static_cast<int>(TileType::FOUNDATION_B):
								drawAsset("foundation_B", "foundation_B_d", "diffuse");
								break;
						case static_cast<int>(TileType::FOUNDATION_C):
								drawAsset("foundation_C", "foundation_C_d", "diffuse");
								break;
						case static_cast<int>(TileType::FOUNDATION_D):
								drawAsset("foundation_D", "foundation_D_d", "diffuse");
								break;
            default:
                break;
            }
        }

    for(auto &character : m_characters)
    {
			if(character.isSleeping() == false)
			{
        ngl::Vec3 pos = character.getPos();
        pos.m_y /= m_terrainHeightDivider;
        m_transform.setPosition(pos);
        slib->use("colour");
        slib->setRegisteredUniform("colour", ngl::Vec4(character.getColour(),1.0f));
        drawAsset( "person", "", "colour");
			}
    }
}

void Scene::quit()
{
    m_active = false;
}

bool Scene::isActive()
{
    return m_active;
}

//First = world space aabbs
//Second = light space aabbs
std::pair< std::vector< bounds >, std::vector< bounds > > Scene::generateOrthoShadowMatrices(const std::vector<float> &_divisions)
{
    ngl::Vec3 s = m_sunDir;
    //Flip if the moon is up.
    if(s.dot(ngl::Vec3( 0.0f, 1.0f, 0.0f )) > 0.0f)
        s = -s;

    ngl::Mat4 lightView = ngl::lookAt(
                ngl::Vec3(0.0f, 0.0f, 0.0f),
                s,
                ngl::Vec3(0.0f, 1.0f, 0.0f)
                );

    //Get cascades from this.
    std::vector< std::array<ngl::Vec3, 8> > cascades;

    for(size_t i = 0; i <= _divisions.size() - 2; ++i)
    {
        //std::cout << "Calculating for " << _divisions[i] << " to " << _divisions[ i + 1 ] << '\n';
        cascades.push_back( m_cam.calculateCascade( _divisions[i], _divisions[i + 1] ) );
    }

    for(int i = 0; i < 2; ++i)
    {
        auto &c = cascades[i];
        m_debugPoints.push_back(c[0]);
        m_debugPoints.push_back(c[1]);
        m_debugPoints.push_back(c[1]);
        m_debugPoints.push_back(c[3]);
        m_debugPoints.push_back(c[3]);
        m_debugPoints.push_back(c[2]);
        m_debugPoints.push_back(c[2]);
        m_debugPoints.push_back(c[0]);

        m_debugPoints.push_back(c[4]);
        m_debugPoints.push_back(c[5]);
        m_debugPoints.push_back(c[5]);
        m_debugPoints.push_back(c[7]);
        m_debugPoints.push_back(c[7]);
        m_debugPoints.push_back(c[6]);
        m_debugPoints.push_back(c[6]);
        m_debugPoints.push_back(c[4]);

        m_debugPoints.push_back(c[0]);
        m_debugPoints.push_back(c[4]);

        m_debugPoints.push_back(c[1]);
        m_debugPoints.push_back(c[5]);

        m_debugPoints.push_back(c[2]);
        m_debugPoints.push_back(c[6]);

        m_debugPoints.push_back(c[3]);
        m_debugPoints.push_back(c[7]);
    }

    std::pair< std::vector< bounds >, std::vector< bounds > > boxes;

    //Store the world space AABB enclosing the cascade
    for(auto &c : cascades)
        boxes.first.push_back( Utility::enclose(c) );

    //These cascades are in world space. We must convert them into light space.
    for(auto &c : cascades)
    {
        //Loop through each vertex.
        for(auto &vert : c)
        {
            ngl::Vec4 vert4 (vert.m_x, vert.m_y, vert.m_z, 1.0f);
            vert4 = vert4 * lightView;
            vert = ngl::Vec3( vert4.m_x, vert4.m_y, vert4.m_z );
        }
        //std::cout << '\n';
    }

    //Create light space AABBs enclosing each cascade section
    for(auto &c : cascades)
        boxes.second.push_back( Utility::enclose(c) );

    //for(auto &b : boxes.second)
    for(int i = 0; i < 2; ++i)
    {
        auto &b = boxes.second[i];
        ngl::BBox bo (b.first.m_x, b.second.m_x,
                      b.first.m_y, b.second.m_y,
                      b.first.m_z, b.second.m_z
                      );

        ngl::Vec3 * verts = bo.getVertexArray();
        ngl::Mat4 ilv = lightView.inverse();

        for(int i = 0; i < 8; ++i)
        {
            verts[i] = verts[i] * ilv;
        }

        m_debugPoints.push_back(verts[0]);
        m_debugPoints.push_back(verts[1]);
        m_debugPoints.push_back(verts[1]);
        m_debugPoints.push_back(verts[2]);
        m_debugPoints.push_back(verts[2]);
        m_debugPoints.push_back(verts[3]);
        m_debugPoints.push_back(verts[3]);

        m_debugPoints.push_back(verts[0]);
        m_debugPoints.push_back(verts[4]);
        m_debugPoints.push_back(verts[5]);
        m_debugPoints.push_back(verts[5]);
        m_debugPoints.push_back(verts[6]);
        m_debugPoints.push_back(verts[6]);
        m_debugPoints.push_back(verts[7]);
        m_debugPoints.push_back(verts[7]);
        m_debugPoints.push_back(verts[4]);

        m_debugPoints.push_back(verts[0]);
        m_debugPoints.push_back(verts[4]);

        m_debugPoints.push_back(verts[1]);
        m_debugPoints.push_back(verts[5]);

        m_debugPoints.push_back(verts[2]);
        m_debugPoints.push_back(verts[6]);

        m_debugPoints.push_back(verts[3]);
        m_debugPoints.push_back(verts[7]);
    }

    return boxes;
}

void Scene::shadowPass(bounds _worldbox, bounds _lightbox, size_t _index)
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();
    AssetStore *store = AssetStore::instance();

    ngl::Vec3 s = m_sunDir;
    //Flip if the sun is pointing up.
    if(s.dot(ngl::Vec3( 0.0f, 1.0f, 0.0f )) > 0.0f)
        s = -s;

    //Center
    ngl::Vec3 pos = -(_lightbox.first + _lightbox.second) / 2.0f;
    //Half-dimension
    ngl::Vec3 dim = (_lightbox.second - _lightbox.first) / 2.0f;

    ngl::Mat4 project = ngl::ortho(
                -dim.m_x, dim.m_x,
                -dim.m_y, dim.m_y,
                -dim.m_z, dim.m_z
                );

    ngl::Mat4 lightPos = ngl::Mat4();
    lightPos.translate( pos.m_x, pos.m_y, pos.m_z );

    ngl::Mat4 lightDir = ngl::lookAt(
                ngl::Vec3(),
                s,
                ngl::Vec3(0.0f, 1.0f, 0.0f)
                );

    ngl::Mat4 view = lightDir * lightPos;

    m_shadowMat[_index] = view * project;
    m_shadowBuffer.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowBuffer.get( "depth[" + std::to_string(_index) + "]" ), 0);
    glClear( GL_DEPTH_BUFFER_BIT );
    //glCullFace(GL_FRONT);

    slib->use("shadowDepth");
    glBindVertexArray(m_terrainVAO);
    //loadMatricesToShader();
    loadMatricesToShader(ngl::Mat4(), m_shadowMat[_index]);
    glDrawArraysEXT(GL_TRIANGLES, 0, m_terrainVAOSize);
    glBindVertexArray(0);

    slib->use("shadowDepthInstanced");
    int offset = 0;
    for(size_t i = 0; i < m_meshPositions.size(); ++i)
    {
        int instances = m_meshPositions[i].size();
        switch( i )
        {
        case static_cast<int>(TileType::TREES):
            drawInstances( "tree", "tree_d", "diffuse", instances, offset, m_shadowMat[_index] );
            break;
        case static_cast<int>(TileType::MOUNTAINS):
            drawInstances( "mountain", "mountain_d", "diffuse", instances, offset, m_shadowMat[_index] );
            break;
        case static_cast<int>(TileType::STOREHOUSE):
            drawInstances( "storehouse", "storehouse_d", "diffuse", instances, offset, m_shadowMat[_index] );
            break;
        case static_cast<int>(TileType::HOUSE):
						drawInstances( "house", "house_d", "diffuse", instances, offset, m_shadowMat[_index] );
            break;
				case static_cast<int>(TileType::FOUNDATION_A):
						drawInstances("foundation_A", "foundation_A_d", "diffuse", instances, offset, m_shadowMat[_index] );
						break;
				case static_cast<int>(TileType::FOUNDATION_B):
						drawInstances("foundation_B", "foundation_B_d", "diffuse", instances, offset, m_shadowMat[_index]);
						break;
				case static_cast<int>(TileType::FOUNDATION_C):
						drawInstances("foundation_C", "foundation_C_d", "diffuse", instances, offset, m_shadowMat[_index] );
						break;
				case static_cast<int>(TileType::FOUNDATION_D):
						drawInstances("foundation_D", "foundation_D_d", "diffuse", instances, offset, m_shadowMat[_index]);
						break;
        default:
            break;
        }
        offset += instances;
    }

    slib->use("shadowDepth");
    for(auto &character : m_characters)
    {
			if(character.isSleeping() == false)
			{
        ngl::Vec3 pos = character.getPos();
        pos.m_y /= m_terrainHeightDivider;
        m_transform.setPosition(pos);
        ngl::Mat4 mvp = m_transform.getMatrix() * m_shadowMat[_index];

        ngl::Obj * k = store->getModel( "person" );
        loadMatricesToShader( m_transform.getMatrix(), mvp );
        k->draw();
			}
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
        //We want to break the camera following if the player drags, not just if they click.
        //Going to try moving this bit elsewhere.
        //m_centre_camera = false;
        m_mouse_trans_origin[0] = _event.x;
        m_mouse_trans_origin[1] = _event.y;
        //records position of mouse on press
        m_mouse_prev_pos[0] = _event.x;
        m_mouse_prev_pos[1] = _event.y;
        m_mouse_trans_active = true;
        Gui::instance()->mouseDown();
    }

    //checks if the right button has been pressed down and flags start
    else if(_event.button == SDL_BUTTON_RIGHT)
    {
        //Commented this out so the player could follow a character and rotate at the same time.
        //I am not familiar with this section of code, sorry if it broke anything -Ben
        //m_centre_camera = false;
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

        m_mouse_trans_origin = Utility::getMousePos();

        m_mouse_prev_pos.set(0.0f, 0.0f);
        m_mouse_trans_active = false;
        Gui::instance()->mouseUp();
    }

    //checks if the right button has been released and turns off flag
    else if (_event.button == SDL_BUTTON_RIGHT)
    {
        m_mouse_rot_origin = Utility::getMousePos().m_x;

        m_mouse_rot_active = false;
    }
}

void Scene::wheelEvent(const SDL_MouseWheelEvent &_event)
{
  Gui *gui = Gui::instance();
  if(gui->mousePos(Utility::getMousePos()))
  {
    gui->scrollButton(_event.y);
  }
  else
  {
    Gui::instance()->executeAction(_event.y > 0 ? Action::ZOOMIN : Action::ZOOMOUT);
  }
}

void Scene::zoom(int _direction)
{
  m_cam.rotate(-1.0f * _direction, 0.0f);
  m_cam.dolly(-0.5f * _direction);
}

void Scene::keyDownEvent(const SDL_KeyboardEvent &_event)
{
    Gui *gui = Gui::instance();
    switch(_event.keysym.sym)
    {
    case SDLK_SPACE:
        gui->executeAction(Action::CENTRECAMERA);
        break;
    case SDLK_ESCAPE:
    case SDLK_p:
        gui->executeAction(Action::ESCAPE);
        break;
    case SDLK_UP:
        gui->executeAction(Action::MOVEFORWARD);
        break;
    case SDLK_DOWN:
        gui->executeAction(Action::MOVEBACKWARD);
        break;
    case SDLK_LEFT:
        gui->executeAction(Action::MOVELEFT);
        break;
    case SDLK_RIGHT:
        gui->executeAction(Action::MOVERIGHT);
        break;
    default:
        break;
    }

}

void Scene::keyUpEvent(const SDL_KeyboardEvent &_event)
{
    Gui *gui = Gui::instance();
    switch(_event.keysym.sym)
    {
    case SDLK_UP:
        gui->executeAction(Action::STOPFORWARD);
        break;
    case SDLK_DOWN:
        gui->executeAction(Action::STOPBACKWARD);
        break;
    case SDLK_LEFT:
        gui->executeAction(Action::STOPLEFT);
        break;
    case SDLK_RIGHT:
        gui->executeAction(Action::STOPRIGHT);
        break;

    default:break;
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
    m_cam.clearTransforms();
    m_cam.calculateProjectionMat();
    m_cam.calculateViewMat();

    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    slib->use("sky");
    slib->setRegisteredUniform("viewport", m_viewport);

    slib->use("deferredLight");
    slib->setRegisteredUniform("waterLevel", m_grid.getGlobalWaterLevel() / m_terrainHeightDivider);

    slib->use("water");
    GLint tlvl = 0;
    glGetIntegerv( GL_MAX_TESS_GEN_LEVEL, &tlvl );
    slib->setRegisteredUniform("maxTessLevel", tlvl);
    std::cout << "Max water tesselation level set to " << tlvl << '\n';
    slib->setRegisteredUniform("pixelstep", ngl::Vec2(1.0f, 1.0f) / m_prefs->getIntPref("WATER_MAP_RES"));
    slib->setRegisteredUniform("viewport", m_viewport);

    slib->use("bokeh");
    slib->setRegisteredUniform("bgl_dim", m_viewport);

    m_mainBuffer = Framebuffer();
    m_pickBuffer = Framebuffer();
    m_shadowBuffer = Framebuffer();
    m_postEffectsBuffer = Framebuffer();

    initialiseFramebuffers();
    Gui::instance()->setResolution(_dim);
    std::cout << "Viewport resized.\n";
}

void Scene::mouseSelection()
{
    Gui *gui = Gui::instance();
    ngl::Vec2 mouse_coords = Utility::getMousePos();

    if(gui->mousePos( mouse_coords ) )
    {
        gui->click();
    }
    else if(m_state == GameState::MAIN)
    {
        int red = getCharIDAtMouse();

        if(red < (m_characters.size() + 1) && red > 0)
        {
            for (Character &character : m_characters)
            {
                if (character.getID() == red)
                {
                    // probably needs changing because vector address is not guaranteed
                    if(character.isActive() == false)
                    {
                        m_active_char_id = character.getID();
                        character.setActive(true);
                        character.clearState();
                        gui->updateActiveCharacter();
                    }
                }
                else
                    if (character.isActive() == true)
                        character.setActive(false);
            }
        }
        //check grid_id texture
        else
        {
            ngl::Vec4 grid_coord = getTerrainPosAtMouse();

            if(grid_coord[0] != 0 && grid_coord[2] != 0)
            {
                int grid_coord_x = floor(grid_coord[0]);
                grid_coord_x = Utility::clamp(grid_coord_x, 0, m_grid.getW());

                int grid_coord_y = floor(grid_coord[2]);
                grid_coord_y = Utility::clamp(grid_coord_y, 0, m_grid.getH());

                int target_id = m_grid.coordToId(ngl::Vec2(grid_coord_x, grid_coord_y));
                for (Character &character : m_characters)
                {
                    if(character.isActive() == true)
                    {
												character.setState(target_id);
                    }
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

    //Top down camera debug view
    //Filthy hack
    /*ngl::Mat4 pro = ngl::perspective(
                80.0f,
                m_cam.getAspect(),
                0.1,
                1024.0
                );
    ngl::Mat4 vu = ngl::lookAt(
                ngl::Vec3(sinf(Utility::radians(m_sunAngle.m_x) * 16.0f) * 32.0f, 32.0f, cosf(Utility::radians(m_sunAngle.m_x) * 16.0f) * 32.0f),
                ngl::Vec3(25.0f, 0.0f, 25.0f),
                ngl::Vec3(0.0f, 1.0f, 0.0f)
                );
    MVP = M * vu * pro;*/

    slib->setRegisteredUniform( "M", M );
    slib->setRegisteredUniform( "MVP", MVP );
}

void Scene::bindTextureToShader(const std::string &_shaderID, const GLuint _tex, const char *_uniform, int _target, GLenum _type)
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

    glActiveTexture(GL_TEXTURE0 + _target);
    glBindTexture(_type, _tex);
}

void Scene::drawAsset(const std::string &_model, const std::string &_texture, const std::string &_shader)
{
    AssetStore *store = AssetStore::instance();
    if(_shader != "")
    {
        ngl::ShaderLib * slib = ngl::ShaderLib::instance();
        slib->use( _shader );
    }

    ngl::Obj * m = store->getModel(_model);
    if(m == nullptr)
    {
        std::cerr << "Error! Mesh " << _model << " doesn't exist!\n";
        return;
    }

    if(_texture != "")
    {
        GLuint t = store->getTexture( _texture );
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

void Scene::drawInstances(const std::string &_model, const std::string &_texture, const std::string &_shader, const int _instances, const int _offset)
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();
    AssetStore *store = AssetStore::instance();
    if(_shader != "")
    {
        slib->use( _shader );
    }

    ngl::Obj * m = store->getModel(_model);
    if(m == nullptr)
    {
        std::cerr << "Error! Mesh " << _model << " doesn't exist!\n";
        return;
    }

    if(_texture != "")
    {
        GLuint t = store->getTexture( _texture );
        if(t == 0)
        {
            std::cerr << "Error! Texture " << _texture << " doesn't exist!\n";
            return;
        }
        bindTextureToShader(_shader, t, "diffuse", 0);
    }

    slib->setRegisteredUniform("VP", m_cam.getVP());
    slib->setRegisteredUniform("offset", _offset);
    glBindTexture(GL_TEXTURE_BUFFER, m_instanceTBO);
    //bindTextureToShader(_shader, m_instanceTBO, "transform", 1, GL_TEXTURE_BUFFER);

    m->bindVAO();
    glDrawArraysInstanced(GL_TRIANGLES, 0, m->getMeshSize(), _instances);
    m->unbindVAO();
}

void Scene::drawInstances(const std::string &_model, const std::string &_texture, const std::string &_shader, const int _instances, const int _offset, const ngl::Mat4 &_VP)
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();
    AssetStore *store = AssetStore::instance();
    if(_shader != "")
    {
        slib->use( _shader );
    }

    ngl::Obj * m = store->getModel(_model);
    if(m == nullptr)
    {
        std::cerr << "Error! Mesh " << _model << " doesn't exist!\n";
        return;
    }

    if(_texture != "")
    {
        GLuint t = store->getTexture( _texture );
        if(t == 0)
        {
            std::cerr << "Error! Texture " << _texture << " doesn't exist!\n";
            return;
        }
        bindTextureToShader(_shader, t, "diffuse", 0);
    }

    slib->setRegisteredUniform("VP", _VP);
    slib->setRegisteredUniform("offset", _offset);
    glBindTexture(GL_TEXTURE_BUFFER, m_instanceTBO);
    //bindTextureToShader(_shader, m_instanceTBO, "transform", 1, GL_TEXTURE_BUFFER);

    m->bindVAO();
    glDrawArraysInstanced(GL_TRIANGLES, 0, m->getMeshSize(), _instances);
    m->unbindVAO();
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
            float height = m_grid.getTileHeight(i, j) / m_terrainHeightDivider;
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
    for(size_t i = 0; i < faces.size(); ++i)
    {
        facenorms.push_back( std::vector<ngl::Vec3>() );
        for(size_t j = 0; j < faces[i].size(); ++j)
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

    for(size_t i = 0; i < faces.size(); ++i)
    {
        for(size_t j = 0; j < faces[i].size(); ++j)
        {
            terrainFace f = terrainVerticesToFace(
                        i,
                        j,
                        faces,
                        facenorms
                        );

            ngl::Vec3 t1na = f.m_verts[2].m_norm + f.m_verts[1].m_norm + f.m_verts[0].m_norm;
            t1na /= 3.0f;
            //t1
            trimesh.push_back( f.m_verts[2].m_pos );
            trimesh.push_back( f.m_verts[1].m_pos );
            trimesh.push_back( f.m_verts[0].m_pos );

            normesh.push_back( t1na );
            normesh.push_back( t1na );
            normesh.push_back( t1na );

            ngl::Vec3 t2na = f.m_verts[1].m_norm + f.m_verts[2].m_norm + f.m_verts[3].m_norm;
            t2na /= 3.0f;
            //t2
            trimesh.push_back( f.m_verts[1].m_pos );
            trimesh.push_back( f.m_verts[2].m_pos );
            trimesh.push_back( f.m_verts[3].m_pos );

            normesh.push_back( t2na );
            normesh.push_back( t2na );
            normesh.push_back( t2na );
        }
    }

    for(auto &vec : trimesh)
    {
      vec += ngl::Vec4(0.5, 0, 0.5, 0);
        uvmesh.push_back( ngl::Vec2(vec.m_x, vec.m_z) );
    }

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

    //Generate xy positions
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
    bool horizontal = (_x + _dirX) >= 0 and (_x + _dirX) <= _facePositions[0].size() - 1;
    //Can we move in the vertical direction?
    bool vertical = (_y + _dirY) >= 0 and (_y + _dirY) <= _facePositions.size() - 1;

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

    //Normalise normal
    normal /= static_cast<float>(count);
    normal.normalize();

    return std::make_pair(
                yCoord / static_cast<float>(count),
                normal
                );
}

void Scene::centreCamera()
{
    for (Character &character : m_characters)
    {
        if (character.isActive())
        {
            m_centre_camera = true;
        }
    }
}

Character *Scene::getActiveCharacter()
{
    if(m_active_char_id == -1)
    {
        return nullptr;
    }
    for(Character &c : m_characters)
    {
        if(c.getID() == m_active_char_id)
        {
            return &c;
        }
    }
    return nullptr;
}

std::string Scene::getActiveCharacterName()
{
    std::string char_name("");
    if(getActiveCharacter())
    {
        char_name = getActiveCharacter()->getName();
    }
    return char_name;
}

void Scene::togglePause()
{
    Gui *gui = Gui::instance();
    std::cout << "pause toggled" << std::endl;
    if(m_state != GameState::MAIN)
    {
        m_state = GameState::MAIN;
        gui->unpause();
    }
    else
    {
        m_state = GameState::PAUSE;
        gui->pause();
    }
}

void Scene::startGame()
{
  m_state = GameState::MAIN;
  m_game_started = true;
  Gui::instance()->unpause();
}

void Scene::startMove(Direction _d)
{
    m_movement_held[_d] = true;
}

void Scene::stopMove(Direction _d)
{
    m_movement_held[_d] = false;
}

ngl::Vec3 Scene::getCamMoveVec()
{
    ngl::Vec3 move(0,0,0);
    if(m_movement_held[Direction::FORWARDS])
        move.m_z -= 1;
    if(m_movement_held[Direction::BACKWARDS])
        move.m_z += 1;
		if(m_movement_held[Direction::LEFTWARDS])
        move.m_x += 1;
		if(m_movement_held[Direction::RIGHTWARDS])
        move.m_x -= 1;
    move *= 10;
    float cam_to_pivot_height = abs(m_cam.getPivot().m_y - m_cam.getPos().m_y) + 20;
    cam_to_pivot_height *= cam_to_pivot_height * 0.00004f;
    move *= cam_to_pivot_height;
    return move;
}

void Scene::prefsMode()
{
    m_state = GameState::PREFERENCES;
    Gui::instance()->createPrefsButtons();
}

void Scene::escapeState()
{
  Gui *gui = Gui::instance();
  switch (m_state) {
  case GameState::START_MENU:
    startGame();
    break;
  case GameState::MAIN:
  case GameState::PAUSE:
    togglePause();
    break;
  case GameState::PREFERENCES:
    if(m_game_started)
    {
      m_state = GameState::PAUSE;
      gui->pause();
    }
    else
    {
      m_state = GameState::START_MENU;
      gui->createStartMenuButtons();
    }
    break;
  default:
    break;
  }
}

GameState Scene::getState()
{
    return m_state;
}

void Scene::focusCamToGridPos(ngl::Vec2 _pos)
{
    std::cout << "moved camera" << std::endl;
    m_cam.setPos(ngl::Vec3(_pos.m_x, 0, _pos.m_y));
}

ngl::Vec4 Scene::getTerrainPosAtMouse()
{
    m_pickBuffer.bind();
    GLuint grid_texID = getTerrainPickTexture();
    glBindTexture(GL_TEXTURE_2D, grid_texID);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    ngl::Vec2 mouse_coords = Utility::getMousePos();
    ngl::Vec4 grid_coord = ngl::Vec4();

    // x, window height - y
    glReadPixels(mouse_coords.m_x, (m_viewport.m_y - mouse_coords.m_y), 1, 1, GL_RGBA, GL_FLOAT, &grid_coord.m_x);
    m_pickBuffer.unbind();

    //std::cout << "wmp = " << grid_coord << '\n';
    return grid_coord;
}

int Scene::getCharIDAtMouse()
{
    m_pickBuffer.bind();

    ngl::Vec2 mousePos = Utility::getMousePos();

    //check character_id texture
    GLuint char_texID = getCharPickTexture();
    glBindTexture(GL_TEXTURE_2D, char_texID);
    glReadBuffer(GL_COLOR_ATTACHMENT1);

    int red = 0;

    glReadPixels(mousePos.m_x, (m_viewport.m_y - mousePos.m_y), 1, 1, GL_RED_INTEGER, GL_INT, &red);

    m_pickBuffer.unbind();

    return red - 1;
}
