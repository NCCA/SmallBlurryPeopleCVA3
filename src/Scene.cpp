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
#include "MapList.hpp"

#include <ngl/NGLStream.h>

#include <ngl/AABB.h>

//If you see this delete it and talk to Ben.
std::vector<std::vector<ngl::Vec3>> faces;

//This should be the same as the size of the map, for now.
const ngl::Vec2 waterDimensions (20.0f, 20.0f);

const int shadowResolution = 4096;
const int waterResolution = 1024;

Scene::Scene(ngl::Vec2 _viewport) :
    m_globalTime(0.0f),
    m_active(true),
    m_grid(&m_world_inventory),
    m_active_char_id(-1),
    m_mouse_trans_active(false),
    m_mouse_rot_active(false),
    m_centre_camera(true),
    m_mouse_prev_pos(0.0f, 0.0f),
    m_sunAngle(90.0f, 0.0f, 5.0f),
    m_windDirection(ngl::Vec3(0.05f, 0.0f, 0.0f), ngl::Vec3(0.05f, 0.0f, 0.0f), 0.01f),
    m_state(GameState::START_MENU),
    m_game_started(false),
    m_movement_held{false},
    m_mouseSelectionBoxPosition( ngl::Vec3(), ngl::Vec3(), 0.75f),
    m_mouseSelectionBoxScale( ngl::Vec3(1.0f, 1.0f, 1.0f), ngl::Vec3(1.0f, 1.0f, 1.0f), 0.75f)
{
    Gui *gui = Gui::instance();
    gui->init(this, _viewport, "button");

    //reads file with list of names
    readNameFile();
    //pass reference of world inventory to character class
    Character::setWorldInventory(&m_world_inventory);

    ngl::Random * rnd = ngl::Random::instance();
    rnd->setSeed();

    m_prefs = Prefs::instance();
    AssetStore *store = AssetStore::instance();
    m_viewport = _viewport;

    float aspect = _viewport.m_x / _viewport.m_y;
    m_cam.setAspect( aspect );
    m_cam.calculateProjectionMat();
    m_cam.clearTransforms();
    m_cam.calculateViewMat();
    zoom(-30);

    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    createShader("deferredLight", "vertScreenQuad", "fragBasicLight");
    createShader("diffuseInstanced", "vertDeferredDataInstanced", "fragDeferredDiffuse");
    createShader("diffuse", "vertDeferredData", "fragDeferredDiffuse");
    createShader("diffuseCharacter", "vertDeferredData", "fragCharDiffuse");
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
    createShader("waterDisplacementBlend", "vertScreenQuad", "fragWaterDisplacementBlend");
    createShader("waterDisplacementNormal", "vertScreenQuad", "fragWaterDisplacementNormal");
    createShader("bokeh", "vertScreenQuad", "fragBokehBlur");
    createShader("debugTexture", "vertScreenQuadTransform", "fragDebugTexture");
    createShader("mousebox", "vertDeferredData", "fragMousebox");
    createShader("fragNormals", "vertDeferredData", "fragNormal");
    createShader("clouds", "vertBillboard", "fragCloud", "geoBillboard");
    createShader("levelSelect", "vertScreenQuad", "levelSelFrag");

    slib->use("sky");
    slib->setRegisteredUniform("viewport", m_viewport);

    slib->use("deferredLight");
    slib->setRegisteredUniform("waterLevel", m_grid.getGlobalWaterLevel());

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

    initialiseFramebuffers();
    std::cout << "Framebuffers initialised!\n";

    m_shadowMat.assign(3, ngl::Mat4());

    ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();

    prim->createTrianglePlane("plane",14,14,80,80,ngl::Vec3(0,1,0));
    prim->createSphere( "sphere", 0.1, 12 );

    std::cout << "Loading assets...\n";

    //playing with trees and houses and such
    store->loadMesh("debugSphere", "sphere.obj");
    store->loadMesh("raySphere", "raySphere.obj");
    store->loadMesh("debugBox", "box.obj");

    store->loadMesh("tree", "tree/tree.obj");
    store->loadTexture("tree_d", "tree/tree_d.png");
    store->loadMesh("tree_stump", "tree/tree_stump.obj");
    store->loadTexture("tree_stump_d", "tree/tree_stump_d.png");

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
    store->loadTexture("person_d", "person/person.tif");
    store->loadTexture("baddie_d", "baddie/skelly.tif");

    store->loadTexture("grass", "terrain/grass.png");
    store->loadTexture("rock", "terrain/rock.png");
    store->loadTexture("snow", "terrain/snow.png");

    store->loadTexture("level_arena", "levels/arena.png");
    store->loadTexture("level_connectedDots", "levels/connectedDots.png");
    store->loadTexture("level_simplexMap", "levels/simplexMap.png");
    store->loadTexture("level_survivalIsland", "levels/survivalIsland.png");
    store->loadTexture("level_uk", "levels/uk.png");
    store->loadTexture("level_custom", "levels/custom.png");

    store->loadMesh("tombstone", "tombstone/tombstone.obj");
    store->loadTexture("tombstone_d", "tombstone/tombstone_diff.tif");

    store->loadTexture("cloud0", "cloud/cloud0.png");
    store->loadTexture("cloud1", "cloud/cloud1.png");
    store->loadTexture("cloud2", "cloud/cloud2.png");
    store->loadTexture("sphericalNormal", "cloud/sphericalNormal.png");

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

    glGenVertexArrays(1, &m_debugVAO);
    glGenBuffers(1, &m_debugVBO);

    //Create and reserve light buffer.
    m_pointLights.reserve(m_maxLights);
    glGenBuffers(1, &m_lightBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_lightBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Light) * m_maxLights, &m_pointLights[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    std::cout << "Scene contructed.\n";
}

void Scene::initMeshInstances()
{
    m_meshInstances.clear();
    int iiterations = std::ceil(m_grid.getW() / (float)m_meshInstanceBlockTileSize);
    int jiterations = std::ceil(m_grid.getH() / (float)m_meshInstanceBlockTileSize);

    for(int x = 0; x < iiterations; ++x)
    {
        for(int y = 0; y < jiterations; ++y)
        {
            meshInstanceBlock b = generateInstanceMeshTile(x,y);
            m_meshInstances.push_back(b);
        }
    }
}

Scene::meshInstanceBlock Scene::generateInstanceMeshTile(const int _x, const int _y)
{
    meshInstanceBlock b;

    int meshCount = 0;
    int max_trees = Prefs::instance()->getIntPref("TREES_PER_TILE");
    //m_meshPositions.clear()
    b.m_meshPositions.assign(static_cast<int>(TileType::FOUNDATION_D) + 1, std::vector<ngl::Vec3>());
    int minx = _x * m_meshInstanceBlockTileSize;
    int maxx = _x * m_meshInstanceBlockTileSize + m_meshInstanceBlockTileSize;
    int miny = _y * m_meshInstanceBlockTileSize;
    int maxy = _y * m_meshInstanceBlockTileSize + m_meshInstanceBlockTileSize;
    b.m_bounds.first = ngl::Vec3(minx, 0.0f, miny);
    b.m_bounds.second = ngl::Vec3(maxx, 0.0f, maxy);
    for(int i = minx; i < maxx and i < m_grid.getW(); ++i)
        for(int j = miny; j < maxy and j < m_grid.getH(); ++j)
        {
            TileType index = m_grid.getTileType(i, j);
            if (index == TileType::TREES)
            {
                //get num trees
                int num_trees = m_grid.getNumTrees(i, j);
                //get position vector
                std::vector<ngl::Vec2> positions = m_grid.getTreePositions(i, j);
                //for i in trees:
                for (int n = 0; n < max_trees; n++)
                {
                    if (n < num_trees)
                    {
                        //push back tree
                        ngl::Vec2 p = positions[n];
                        b.m_meshPositions.at((int)index).push_back(ngl::Vec3(
                                                                       i+p[0],
                                                                   m_height_tracer.getHeight(i+p[0], j+p[1]),
                                j+p[1]
                                ));
                    }
                    else
                    {
                        ngl::Vec2 p = positions[n];
                        b.m_meshPositions.at((int)TileType::STUMPS).push_back(ngl::Vec3(
                                                                                  i+p[0],
                                                                              m_height_tracer.getHeight(i+p[0], j+p[1]),
                                j+p[1]
                                ));
                    }
                    //increment meshCount
                    meshCount++;
                }
            }
            else{
                b.m_meshPositions.at( (int)index ).push_back(ngl::Vec3(
                                                                 i+0.5,
                                                                 m_height_tracer.getHeight(i+0.5, j+0.5),
                                                                 j+0.5
                                                                 ));
                meshCount++;
            }
        }

    //Generate TBO for mesh instancing.
    //ngl::Random * rng = ngl::Random::instance();
    std::vector<ngl::Mat4> transforms;
    transforms.reserve( meshCount );
    for(auto &slot : b.m_meshPositions)
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

    glGenTextures(1, &b.m_instanceTBO);
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture(GL_TEXTURE_BUFFER, b.m_instanceTBO);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, buf);

    return b;
}

void Scene::recalculateInstancedMeshes(int _tilex, int _tiley)
{
    std::cout << "recalculateInstancedMeshes start\n";
    int x = _tilex / m_meshInstanceBlockTileSize;
    int y = _tiley / m_meshInstanceBlockTileSize;
    int index = x * std::floor(m_grid.getH() / m_meshInstanceBlockTileSize) + y;

    std::cout << "Recalculation requested from " << _tilex << " (" << x << "), " << _tiley << " (" << y << ")" << '\n';
    std::cout << "index " <<  (x * m_grid.getH() / m_meshInstanceBlockTileSize) << ", " << y << '\n';
    meshInstanceBlock b = generateInstanceMeshTile(x, y);
    std::cout << "Accessing " << index << " of " << m_meshInstances.size() << '\n';
    m_meshInstances[index] = b;
    std::cout << "recalculateInstancedMeshes end\n";
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
    m_utilityBuffer.initialise( m_viewport.m_x, m_viewport.m_y );
    m_utilityBuffer.addTexture( "terrainpos", GL_RGBA, GL_RGBA16F, GL_COLOR_ATTACHMENT0 );
    m_utilityBuffer.addTexture( "charid", GL_RED_INTEGER, GL_R16I, GL_COLOR_ATTACHMENT1, GL_INT );
    m_utilityBuffer.addTexture("rayDir", GL_RGBA, GL_RGBA16F, GL_COLOR_ATTACHMENT2);
    m_utilityBuffer.addDepthAttachment( "depth" );
    if(!m_utilityBuffer.checkComplete())
    {
        std::cerr << "Uh oh! Framebuffer incomplete! Error code " << glGetError() << '\n';
        exit(EXIT_FAILURE);
    }
    m_utilityBuffer.unbind();

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
    m_displacementBuffer.addTexture("waterDisplacementBlended", GL_RED, GL_R16F, GL_COLOR_ATTACHMENT2);
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
    //open file of names
    std::ifstream namesFile;
    namesFile.open("names/game_names.txt");

    //if can't open file
    if(!namesFile.is_open())
    {
        std::cerr<<"Couldnt open file\n";
        exit(EXIT_FAILURE);
    }

    //go through file line by line and add to vector of names, m_file_names
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
        namesFile.close();
    }
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
    m_characters.push_back(Character(&m_height_tracer, &m_grid, m_file_names[name_chosen], &m_baddies));
    //remove name from list so no multiples
    m_file_names.erase(m_file_names.begin() + name_chosen);
}

void Scene::update()
{
    //Gui::instance()->updateNotifications();
    if (m_grid.hasChanges())
    {
        std::cout << "tiles change start\n";
        std::vector< std::vector< bool > > regenTiles;
        regenTiles.assign( m_grid.getW() / m_meshInstanceBlockTileSize + 1, std::vector< bool >() );
        for(auto &vec : regenTiles)
            vec.assign( m_grid.getH() / m_meshInstanceBlockTileSize + 1, false );

        for(auto &coord : m_grid.getChangedTiles())
        {
            int x = std::floor(coord.m_x / m_meshInstanceBlockTileSize);
            int y = std::floor(coord.m_y / m_meshInstanceBlockTileSize);
            if(!regenTiles[x][y])
                recalculateInstancedMeshes(std::floor(coord.m_x), std::floor(coord.m_y));
            regenTiles[x][y] = true;
        }

        //initMeshInstances();
        m_grid.resetChangedTiles();
        m_grid.resetHasChanges();
        std::cout << "tiles change end\n";
    }

    if(m_state == GameState::MAIN)
    {
        m_globalTime += 0.1f * m_prefs->getFloatPref("TIME_SCALE");
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
            //centre camera onto character
            for (Character &character : m_characters)
                //if the character has been selected/ the one being controlled by the user
                if (character.isActive())
                    m_cam.setPos(-character.getPos());
        }
        else
        {
            m_cam.moveScreenSpace(getCamMoveVec());
        }

        //Terrain-height correction0
        ngl::Vec3 cxyz = m_cam.getPos();
        int x = Utility::clamp(std::round(cxyz.m_x),0,m_grid.getW()-1);
        int y = Utility::clamp(std::round(cxyz.m_z),0,m_grid.getH()-1);
        cxyz.m_y = m_grid.getTileHeight(x, y);
        ngl::Vec3 cp = m_cam.getTargPos();
        //Grab the y component of the current tile, use that as the target y for the camera.
        m_cam.setPos( ngl::Vec3(cp.m_x, -cxyz.m_y - 0.5f, cp.m_z) );
        //---

        //Update camera focal depth
        ngl::Vec4 tpm = getTerrainPosAtMouse();
        m_cam.setFocalDepth(
                    ngl::Vec3(m_cam.getPos() - ngl::Vec3(tpm.m_x, tpm.m_y, tpm.m_z)).length()
                    );
        //std::cout << m_cam.getFocalDepth() << '\n';

        //Recalculate view matrix.
        m_cam.updateSmoothCamera();
        m_cam.clearTransforms();
        m_cam.calculateViewMat();
        //---

        //Update mouse selection box.
        m_mouseSelectionBoxPosition.update();
        m_mouseSelectionBoxScale.update();

        charactersSpawn();

        for(size_t i=0; i<m_characters.size(); i++)
        {
            //if a character has died
            if (m_characters[i].getHealth() <= 0.0)
            {
                //print a message in the UI
                ngl::Vec2 pos = m_characters[i].getPos2D();
                std::string message = m_characters[i].getName() + " has died!";
                Gui::instance()->notify(message, pos );
                //if its the active character, update the selected character to none
                if (m_active_char_id == m_characters[i].getID())
                {
                    m_active_char_id = -1;
                    Gui::instance()->updateActiveCharacter();
                }
                //add name back to available list
                m_file_names.push_back(m_characters[i].getName());
                //add position to tombstone positions
                ngl::Vec3 stone_pos(m_characters[i].getPos());
                m_tombstones.push_back(stone_pos);
                //remove character from vector
                m_characters.erase(m_characters.begin() + i);
                break;
            }
            else
            {
                //update characters
                m_characters[i].update();
                if (m_characters[i].isSleeping() && m_characters[i].getID() == m_active_char_id)
                {
                    m_characters[i].update();
                    if (m_characters[i].isSleeping() && m_characters[i].getID() == m_active_char_id)
                    {
                        //check if character is sleeping, if it is, dont make it the active character
                        m_active_char_id = -1;
                        Gui::instance()->updateActiveCharacter();
                    }
                }
            }
        }
        //if all characters have died
        if (m_characters.size() == 0)
        {
            endGame("GAME OVER: everybody died :(");
            return;
        }

        // preference for this?
        if(m_prefs->getBoolPref("ENEMIES"))
        {
            baddiesSpawn();
        }

        for(size_t i=0; i<m_baddies.size(); i++)
        {
            //if have health, update
            if(m_baddies[i].getHealth() > 0.0)
                m_baddies[i].update();
            else
            {
                //if the baddies has died, message and erase from vector
                Gui::instance()->notify("Enemy defeated", m_baddies[i].getPos2D());
                m_baddies.erase(m_baddies.begin() + i);
            }
        }


        //m_sunAngle.m_x = 150.0f;
        m_sunAngle.m_z = 30.0f - 25.0f * sinf(m_season * M_PI - M_PI / 2.0f);
        m_sunAngle.m_x = m_globalTime * 4.0f;
        m_day = floor(m_globalTime / 365) + 80;
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
                t_sundown * ngl::Vec3(1.0f, 0.6f, 0.1f);

        m_directionalLightCol /= t_midday + t_midnight + t_sundown;

        m_pointLights.clear();

        for(auto &b : m_meshInstances)
        {
            ngl::Vec3 off (0.0f, 0.5f, 0.0f);
            for(auto &vec : b.m_meshPositions[static_cast<int>(TileType::HOUSE)])
                m_pointLights.push_back( Light(vec + off, ngl::Vec3(1.0f, 0.8f, 0.4f), 0.5f) );
            for(auto &vec : b.m_meshPositions[static_cast<int>(TileType::STOREHOUSE)])
                m_pointLights.push_back( Light(vec + off, ngl::Vec3(1.0f, 0.8f, 0.4f), 0.5f) );
        }

        for(auto &c : m_characters)
        {
            ngl::Vec3 off (0.0f, 0.5f, 0.0f);
            //We don't want everything to light up at the same time, so the characters ids offer a tiny offset.
            if(m_sunDir.dot(ngl::Vec3(0.0f, 1.0f, 0.0f)) < (float)c.getID() * 0.05f)
            {
                ngl::Vec3 vec = c.getPos() + off;
                m_pointLights.push_back( Light(vec + off, ngl::Vec3(1.0f, 0.8f, 0.4f), 0.15f) );
            }
        }

        glBindVertexArray(m_cloudParticlesVAO);

        //Create a VAO for the clouds.
        glBindBuffer(GL_ARRAY_BUFFER, m_cloudParticlesPositionVBO);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(ngl::Vec3) * m_cloudParticles.m_pos.size(),
                     &m_cloudParticles.m_pos[0].m_x,
                GL_STATIC_DRAW
                );
        setBufferLocation(m_cloudParticlesPositionVBO, 0, 3);

        glBindBuffer(GL_ARRAY_BUFFER, m_cloudParticlesScaleVBO);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(float) * m_cloudParticles.m_scale.size(),
                     &m_cloudParticles.m_scale[0],
                GL_STATIC_DRAW
                );
        setBufferLocation(m_cloudParticlesScaleVBO, 1, 1);

        glBindBuffer(GL_ARRAY_BUFFER, m_cloudParticlesTimeVBO);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(float) * m_cloudParticles.m_time.size(),
                     &m_cloudParticles.m_time[0],
                GL_STATIC_DRAW
                );
        setBufferLocation(m_cloudParticlesTimeVBO, 2, 1);

        glBindBuffer(GL_ARRAY_BUFFER, m_cloudParticlesAlphaVBO);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(float) * m_cloudParticles.m_alpha.size(),
                     &m_cloudParticles.m_alpha[0],
                GL_STATIC_DRAW
                );
        setBufferLocation(m_cloudParticlesAlphaVBO, 3, 1);

        glBindVertexArray(0);

        //Update velocity
        for(auto &vec : m_cloudParticles.m_vel)
        {
            vec += m_windDirection.get() * 0.001f;
            vec -= vec * 0.01f;
        }
        //Update position
        for(size_t i = 0; i < m_cloudParticles.size(); ++i)
            m_cloudParticles.m_pos[i] += m_cloudParticles.m_vel[i];

        //Bubble sort the particles. Approach from http://answers.unity3d.com/questions/20984/depth-sorting-of-billboard-particles-how-can-i-do.html
        for(int i = 0; i < 32; ++i)
        {
            for(size_t j = 0; j < m_cloudParticles.size() - 1; ++j)
            {
                ngl::Vec3 diffc = (m_cloudParticles.m_pos[j] - m_cam.getPos());
                ngl::Vec3 diffn = (m_cloudParticles.m_pos[j + 1] - m_cam.getPos());
                if(diffc.lengthSquared() < diffn.lengthSquared())
                {
                    std::swap(m_cloudParticles.m_pos[j], m_cloudParticles.m_pos[j+1]);
                    std::swap(m_cloudParticles.m_vel[j], m_cloudParticles.m_vel[j+1]);
                    std::swap(m_cloudParticles.m_scale[j], m_cloudParticles.m_scale[j+1]);
                    std::swap(m_cloudParticles.m_time[j], m_cloudParticles.m_time[j+1]);
                    std::swap(m_cloudParticles.m_alpha[j], m_cloudParticles.m_alpha[j+1]);
                }
            }
        }
        for(auto &t : m_cloudParticles.m_time)
        {
            t += 0.01f;
            if(t > 3.0f)
                t -= 3.0f;
        }

        //Simple wrapping, now with alpha fading when out of bounds.
        for(size_t i = 0; i < m_cloudParticles.size(); ++i)
        {
            auto vec = m_cloudParticles.m_pos[i];
            auto vel = m_cloudParticles.m_vel[i];
            auto alpha = m_cloudParticles.m_alpha[i];

            bool fading = false;

            if(vec.m_x < 0.0f and vel.m_x < 0.0f)
            {
                fading = true;
                m_cloudParticles.m_alpha[i] -= 0.005f;
                if(alpha <= 0.0f)
                    m_cloudParticles.m_pos[i].m_x += m_grid.getW();
            }
            else if(vec.m_x > m_grid.getW() and vec.m_x > 0.0f)
            {
                fading = true;
                m_cloudParticles.m_alpha[i] -= 0.005f;
                if(alpha <= 0.0f)
                    m_cloudParticles.m_pos[i].m_x -= m_grid.getW();
            }

            if(vec.m_z < 0.0f and vel.m_z < 0.0f)
            {
                fading = true;
                m_cloudParticles.m_alpha[i] -= 0.005f;
                if(alpha <= 0.0f)
                    m_cloudParticles.m_pos[i].m_z += m_grid.getH();
            }
            else if(vec.m_z > m_grid.getH() and vel.m_z > 0.0f)
            {
                fading = true;
                m_cloudParticles.m_alpha[i] -= 0.005f;
                if(alpha <= 0.0f)
                    m_cloudParticles.m_pos[i].m_z -= m_grid.getH();
            }

            if(!fading)
                m_cloudParticles.m_alpha[i] += 0.005f;

            m_cloudParticles.m_alpha[i] = Utility::clamp(
                        m_cloudParticles.m_alpha[i], 0.0f, 1.0f
                        );
        }

        ngl::Random * rnd = ngl::Random::instance();

        if(rnd->randomPositiveNumber() > 0.99f)
        {
            ngl::Vec3 v = rnd->getRandomNormalizedVec3() * 0.05f;
            v.m_y /= 6.0f;

            m_windDirection.setEnd( v );
        }

        m_windDirection.setEnd( m_windDirection.getEnd() * 0.001f );

        //Bubble sort the particles. Approach from http://answers.unity3d.com/questions/20984/depth-sorting-of-billboard-particles-how-can-i-do.html
        for(int i = 0; i < 32; ++i)
        {
            for(size_t j = 0; j < m_cloudParticles.size() - 1; ++j)
            {
                ngl::Vec3 diffc = (m_cloudParticles.m_pos[j] - m_cam.getPos());
                ngl::Vec3 diffn = (m_cloudParticles.m_pos[j + 1] - m_cam.getPos());
                if(diffc.lengthSquared() < diffn.lengthSquared())
                {
                    std::swap(m_cloudParticles.m_pos[j], m_cloudParticles.m_pos[j+1]);
                    std::swap(m_cloudParticles.m_vel[j], m_cloudParticles.m_vel[j+1]);
                    std::swap(m_cloudParticles.m_scale[j], m_cloudParticles.m_scale[j+1]);
                    std::swap(m_cloudParticles.m_time[j], m_cloudParticles.m_time[j+1]);
                    std::swap(m_cloudParticles.m_alpha[j], m_cloudParticles.m_alpha[j+1]);
                }
            }
        }
    }

}

//Order of operations:
//  --
//  SETUP                   : Sets up the scene ready for drawing.
//  --
//  UTILITY ID DRAW         : Draws out utility textures from the scene. Currently this is CHARACTER IDS and TERRAIN WORLD POSITION.
//                            This can all be queried to find what the user is mousing-over.
//  RAY DIR                 : For some of the shading stages, I cast rays out into the world. Unprojecting from the viewport to the world is
//                            pretty tedious, and pretty costly in a fragment shader. In an effort to lower the overhead, I draw the normals
//                            of a sphere to a texture to be queried later (which gives nearly the same result as projection). Although
//                            texture sampling can be expensive my hope is that it is cheaper than doing matrix multiplications on every
//                            fragment on screen a couple of times per frame.
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
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();
    glClearColor(0.0,0.0,0.0,0.0);

    if(m_game_started)
    {
        //---------------------------//
        //           SETUP           //
        //---------------------------//
        m_debugPoints.clear();

        m_transform.reset();
        glEnable(GL_DEPTH_TEST);

        //ngl::Vec4 mouseWorldPos = getTerrainPosAtMouse();

        //---------------------------//
        // UTILITY ID DRAW //
        //---------------------------//
        //Draw to pick buffer.
        m_utilityBuffer.bind();
        m_utilityBuffer.activeColourAttachments({GL_COLOR_ATTACHMENT0});
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        slib->use("terrainPick");
        glBindVertexArray(m_terrainVAO);
        loadMatricesToShader();
        glDrawArraysEXT(GL_TRIANGLES, 0, m_terrainVAOSize);
        glBindVertexArray(0);
        m_utilityBuffer.activeColourAttachments({GL_COLOR_ATTACHMENT1});
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        slib->use("charPick");
        //Draw characters...
        for(auto &ch : m_characters)
        {
            if(ch.isInside() == false)
            {
                ngl::Vec3 pos = ch.getPos();
                m_transform.setPosition(pos);
                m_transform.setScale(1.5f * 2.8, 1.5f * 1.4, 1.5f * 2.8);
                slib->setRegisteredUniform("id", ch.getID());
                drawAsset( "person", "", "");
            }
        }

        //---------------------------//
        //          RAY DIR          //
        //---------------------------//
        m_utilityBuffer.activeColourAttachments({GL_COLOR_ATTACHMENT2});
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        m_transform.reset();
        m_transform.setPosition(m_cam.getPos());
        drawAsset("raySphere", "", "fragNormals");
        m_utilityBuffer.unbind();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

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
        m_transform.reset();

        m_mainBuffer.bind();
        m_mainBuffer.activeColourAttachments();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ngl::Mat4 camFlip;
        camFlip.scale(1.0f, -1.0f, 1.0f);
        ngl::Mat4 camMove;
        camMove.translate(0.0f, -2.0f * m_grid.getGlobalWaterLevel(), 0.0f);

        //Flip camera upside down.

        //m_cam.clearTransforms();
        //m_cam.calculateViewMat();
        m_cam.immediateTransform(camFlip);
        m_cam.immediateTransform(camMove);

        drawTerrain(true);

        drawMeshes();

        m_postEffectsBuffer.bind();
        m_postEffectsBuffer.activeColourAttachments({GL_COLOR_ATTACHMENT0});
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);

        drawSky(true);

        camMove.translate(0.0f, 2.0f * m_grid.getGlobalWaterLevel(), 0.0f);

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
        slib->setRegisteredUniform( "gridDimensions", ngl::Vec2(m_grid.getW(), m_grid.getH() ));

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
        slib->setRegisteredUniform("iGlobalTime", m_globalTime * 16.0f);

        glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);

        //Blending
        m_displacementBuffer.activeColourAttachments({GL_COLOR_ATTACHMENT2});
        slib->use("waterDisplacementBlend");
        id = slib->getProgramID("waterDisplacementBlend");
        m_displacementBuffer.bindTexture(id, "waterDisplacement", "displacement", 0);
        glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);

        //Normals
        m_displacementBuffer.activeColourAttachments({GL_COLOR_ATTACHMENT1});

        slib->use("waterDisplacementNormal");
        id = slib->getProgramID("waterDisplacementNormal");
        m_displacementBuffer.bindTexture(id, "waterDisplacementBlended", "displacement", 0);

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

        drawMeshes();

        drawTerrain();

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
        ngl::Vec4 mlpos = m_mouseSelectionBoxPosition.get();
        mlpos += ngl::Vec4(0.0, 2.0, 0.0, 0.0);
        Gui * g = Gui::instance();
        ngl::Vec4 mouseTerrainPos = getTerrainPosAtMouse();
        bool shouldDrawMouseBox = (m_state == GameState::MAIN and
                                   !m_mouse_trans_active and
                                   !m_mouse_rot_active and
                                   !g->mousePos( Utility::getMousePos() ) and
                                   mouseTerrainPos.m_x > 0.0f and
                                   mouseTerrainPos.m_x < m_grid.getW() and
                                   mouseTerrainPos.m_z > 0.0f and
                                   mouseTerrainPos.m_z < m_grid.getH()
                                   );

        if(shouldDrawMouseBox)
        {
            m_pointLights.push_back(
                        Light(mlpos,ngl::Vec3(0.0,1.0,1.0),0.5f)
                        );
        }

        glBindBuffer(GL_UNIFORM_BUFFER, m_lightBuffer);
        GLvoid * dat = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
        memcpy(dat, &m_pointLights[0], sizeof(Light) * std::min( m_pointLights.size(), static_cast<size_t>(m_maxLights)));
        glUnmapBuffer(GL_UNIFORM_BUFFER);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        //If blur is enabled draw to blur buffer. Else draw to back buffer.
        Prefs * pref = Prefs::instance();
        bool blur = pref->getBoolPref("DOF");

        if(blur)
        {
            m_postEffectsBuffer.bind();
            m_postEffectsBuffer.activeColourAttachments({GL_COLOR_ATTACHMENT1});
            glClear(GL_COLOR_BUFFER_BIT);
        }

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
        slib->setRegisteredUniform( "gridDimensions", ngl::Vec2(m_grid.getW(), m_grid.getH() ));

        //Draw god rays here.
        //To-do: Add preference bool to control drawing of god rays.
        slib->setRegisteredUniform( "drawGodRays", false);

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
        m_utilityBuffer.bindTexture( id, "rayDir", "rayDir", 7 );

        GLuint lightBlockIndex = glGetUniformBlockIndex( id, "lightBuffer" );
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_lightBuffer);
        glUniformBlockBinding(id, lightBlockIndex, 1);

        slib->setRegisteredUniform("lbufLen", static_cast<int>(std::min(m_maxLights, m_pointLights.size())));

        glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);

        m_postEffectsBuffer.unbind();

        //---------------------------//
        //    DEFERRED BLUR PASS     //
        //---------------------------//
        if(blur)
        {
            slib->use("bokeh");

            id = slib->getProgramID("bokeh");
            m_postEffectsBuffer.bindTexture(id, "sceneColour", "bgl_RenderedTexture", 0);
            m_mainBuffer.bindTexture(id, "linearDepth", "bgl_DepthTexture", 1);
            slib->setRegisteredUniform("focalDepth", m_cam.getFocalDepth());

            glBindVertexArray(m_screenQuad);
            glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);
        }

        //---------------------------//
        //      FORWARD SHADING      //
        //---------------------------//
        //If using DOF copy depth data to the post effects buffer. If not copy it straight to the back buffer.
        if(blur)
        {
            m_postEffectsBuffer.bind();
            m_postEffectsBuffer.activeColourAttachments({GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //Copy depth buffer from main buffer to back buffer.
            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_mainBuffer.getID());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_postEffectsBuffer.getID());
            glBlitFramebuffer(0, 0, m_viewport.m_x, m_viewport.m_y, 0, 0, m_viewport.m_x, m_viewport.m_y,
                              GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        }
        else
        {
            glClear(GL_DEPTH_BUFFER_BIT);

            //Copy depth buffer from main buffer to back buffer.
            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_mainBuffer.getID());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, m_viewport.m_x, m_viewport.m_y, 0, 0, m_viewport.m_x, m_viewport.m_y,
                              GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        }

        glEnable(GL_DEPTH_TEST);

        slib->use("water");
        slib->setRegisteredUniform("mouseWorldPos", mlpos);
        slib->setRegisteredUniform("drawMouseGlow", shouldDrawMouseBox);
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
                ngl::Vec3 pos (i, m_grid.getGlobalWaterLevel(), j);
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

        glDepthMask(false);
        slib->use("clouds");
        slib->setRegisteredUniform("camPos", m_cam.getPos());
        slib->setRegisteredUniform("directionalLightCol", m_directionalLightCol);
        if(m_sunDir.dot(ngl::Vec3(0.0f, 1.0f, 0.0f)) > 0.0f)
            slib->setRegisteredUniform("lightDir", m_sunDir);
        else
            slib->setRegisteredUniform("lightDir", -m_sunDir);
        bindTextureToShader("clouds", AssetStore::instance()->getTexture("cloud0"), "t0", 0);
        bindTextureToShader("clouds", AssetStore::instance()->getTexture("cloud1"), "t1", 1);
        bindTextureToShader("clouds", AssetStore::instance()->getTexture("cloud2"), "t2", 2);
        bindTextureToShader("clouds", AssetStore::instance()->getTexture("sphericalNormal"), "normalMap", 3);

        glBindVertexArray(m_cloudParticlesVAO);
        m_transform.reset();
        loadMatricesToShader();
        glDrawArraysEXT(GL_POINTS, 0, m_cloudParticles.size());
        glDepthMask(true);

        glBindVertexArray(0);

        m_postEffectsBuffer.unbind();

        //---------------------------//
        //  DRAW BOX WHERE MOUSE IS  //
        //---------------------------//
        int charid = getCharIDAtMouse();

        if(shouldDrawMouseBox)
        {
            glDisable(GL_CULL_FACE);
            m_transform.reset();

            ngl::Vec4 p;
            //Player is not hovering over a character.
            if(charid == -1)
            {
                p = mouseTerrainPos;

                p.m_x = floor(p.m_x) + 0.5;
                p.m_z = floor(p.m_z) + 0.5;
                p.m_y = m_height_tracer.getHeight(p.m_x, p.m_z);
                p.m_y = std::max(p.m_y, static_cast<float>(m_grid.getGlobalWaterLevel()));
                ngl::Vec3 pos = m_mouseSelectionBoxPosition.get();
                pos[1] = m_height_tracer.getHeight(pos[0], pos[2]);
                m_transform.setScale( m_mouseSelectionBoxScale.get() );
                m_transform.setPosition( pos );
                m_mouseSelectionBoxScale.setEnd(ngl::Vec3(1.0f, 2.0f, 1.0f));
            }
            else
            {
                for(auto &c : m_characters)
                    if(c.getID() == charid)
                        p = c.getPos();

                ngl::Vec3 pos = m_mouseSelectionBoxPosition.get();
                pos[1] = m_height_tracer.getHeight(pos[0], pos[2]);
                m_transform.setScale( m_mouseSelectionBoxScale.get() );
                m_transform.setPosition( pos );
                m_mouseSelectionBoxScale.setEnd(ngl::Vec3(0.25f, 8.0f, 0.25f));
            }
            p.m_y -= 1.0f;
            m_mouseSelectionBoxPosition.setEnd(ngl::Vec3(p.m_x, p.m_y, p.m_z));

            if(blur)
            {
                m_postEffectsBuffer.bind();
                m_postEffectsBuffer.activeColourAttachments({GL_COLOR_ATTACHMENT1});
            }

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
            glBindVertexArray(0);

            m_postEffectsBuffer.unbind();

            glEnable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);
        }

        //---------------------------//
        //     FORWARD BLUR PASS     //
        //---------------------------//
        if(blur)
        {
            slib->use("bokeh");
            slib->setRegisteredUniform("focalDepth", m_cam.getFocalDepth());

            id = slib->getProgramID("bokeh");
            m_postEffectsBuffer.bindTexture(id, "sceneColour", "bgl_RenderedTexture", 0);
            m_postEffectsBuffer.bindTexture(id, "linearDepth", "bgl_DepthTexture", 1);

            glBindVertexArray(m_screenQuad);
            glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);
        }
    }
    else
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        slib->use("levelSelect");
        slib->setRegisteredUniform("resolution", m_viewport);
        GLuint texture_id = AssetStore::instance()->getTexture("level_"+MapList::instance()->getCurrentMapName());
        if(texture_id == 0)
        {
            texture_id = AssetStore::instance()->getTexture("level_custom");
        }
        bindTextureToShader("levelSelect", texture_id, "diffuse", 0);
        glBindVertexArray(m_screenQuad);

        glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);

        glBindVertexArray(0);

    }

    //---------------------------//
    //          BUTTONS          //
    //---------------------------//
    glViewport(0, 0, m_viewport.m_x, m_viewport.m_y);

    Gui::instance()->drawButtons();

    //---------------------------//
    //         DEBUG DRAW        //
    //---------------------------//

    //It'd be good to have some kind of m_debugViewModeGLuint to control this section.

    glBindVertexArray(m_screenQuad);

    for(int i = 0; i < 3; ++i)
    {
        m_transform.reset();
        m_transform.setPosition(-0.75 + 0.55 * i, -0.75, 0.0);
        m_transform.setScale(0.25, 0.25, 1.0);

        slib->use("debugTexture");
        GLuint id = slib->getProgramID("debugTexture");
        m_displacementBuffer.bindTexture( id, "waterDisplacementBlended", "tex", 0);
        //m_shadowBuffer.bindTexture( id, "depth[" + std::to_string(i) + "]", "tex", 0 );
        slib->setRegisteredUniform( "M", m_transform.getMatrix() );

        glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);
    }

    /*m_transform.reset();
    m_transform.setScale(0.1,0.1,0.1);
    slib->use("colour");
    slib->setRegisteredUniform("colour", ngl::Vec4(1.0,0.0,0.0,1.0));
    for(auto &row : faces)
        for(auto &vec : row)
        {
            m_transform.setPosition(vec + ngl::Vec3(0.5f, 0.0f, 0.5f));
            drawAsset("debugSphere", "", "");
        }*/

    /*slib->use("colour");
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

void Scene::drawSky(bool _flipped)
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    glBindVertexArray(m_screenQuad);

    slib->use("sky");
    m_utilityBuffer.bindTexture(slib->getProgramID("sky"), "rayDir", "rayDir", 0);
    slib->setRegisteredUniform("camPos", m_cam.getPos());
    slib->setRegisteredUniform( "sunDir", m_sunDir );

    slib->setRegisteredUniform("flipped", _flipped);

    slib->setRegisteredUniform("surfaceHeight", 0.8f + 0.2f * m_sunDir.dot(ngl::Vec3(0.0f, -1.0f, 0.0f)));

    glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);
}

void Scene::drawTerrain(bool _shouldClip)
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();
    AssetStore *store = AssetStore::instance();
    slib->use("terrain");
    bindTextureToShader("terrain", store->getTexture("grass"), "grass", 0);
    bindTextureToShader("terrain", store->getTexture("rock"), "rock", 1);
    bindTextureToShader("terrain", store->getTexture("snow"), "snow", 2);

    float waterLevel = m_grid.getGlobalWaterLevel();
    slib->setRegisteredUniform( "waterlevel", waterLevel);
    float snowLevel = m_grid.getGlobalMountainHeight();
    float difference = snowLevel - waterLevel;
    float snow = 0.5f * difference * sinf(m_season * 2.0f * M_PI - M_PI / 2.0f) + 0.5f * difference + waterLevel;
    slib->setRegisteredUniform( "snowline", snow);

    if(_shouldClip)
    {
        slib->setRegisteredUniform("clipAgainstHeight", true);
        slib->setRegisteredUniform("clipHeight", m_grid.getGlobalWaterLevel());
    }
    else
        slib->setRegisteredUniform("clipAgainstHeight", false);

    //We shouldn't transform the terrain, this stops us from doing it accidentally.
    m_transform.reset();

    glBindVertexArray(m_terrainVAO);
    loadMatricesToShader();
    glDrawArraysEXT(GL_TRIANGLES, 0, m_terrainVAOSize);
    glBindVertexArray(0);
}

void Scene::drawMeshes()
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    slib->use("diffuseInstanced");

    for(size_t b = 0; b < m_meshInstances.size(); ++b)
    {
        const auto &block = m_meshInstances[b];
        int offset = 0;
        for(size_t i = 0; i < block.m_meshPositions.size(); ++i)
        {
            int instances = block.m_meshPositions[i].size();
            switch( i )
            {
            case static_cast<int>(TileType::TREES):
                drawInstances( "tree", "tree_d", "diffuseInstanced", instances, offset, b );
                break;
            case static_cast<int>(TileType::STUMPS):
                drawInstances( "tree_stump", "tree_stump_d", "diffuseInstanced", instances, offset, b );
                break;
            case static_cast<int>(TileType::STOREHOUSE):
                drawInstances( "storehouse", "storehouse_d", "diffuseInstanced", instances, offset, b );
                break;
            case static_cast<int>(TileType::HOUSE):
                drawInstances( "house", "house_d", "diffuseInstanced", instances, offset, b );
                break;
            case static_cast<int>(TileType::FOUNDATION_A):
                drawInstances( "foundation_A", "foundation_A_d", "diffuseInstanced", instances, offset, b );
                break;
            case static_cast<int>(TileType::FOUNDATION_B):
                drawInstances( "foundation_B", "foundation_B_d", "diffuseInstanced", instances, offset, b );
                break;
            case static_cast<int>(TileType::FOUNDATION_C):
                drawInstances( "foundation_C", "foundation_C_d", "diffuseInstanced", instances, offset, b );
                break;
            case static_cast<int>(TileType::FOUNDATION_D):
                drawInstances( "foundation_D", "foundation_D_d", "diffuseInstanced", instances, offset, b );
                break;
            default:
                break;
            }
            offset += instances;
        }
    }

    for(Character &character : m_characters)
    {
        //if not inside house or store house, draw
        if(!character.isInside())
        {
            ngl::Vec3 pos = character.getPos();
            m_transform.reset();
            m_transform.setPosition(pos);
            m_transform.setRotation(0, character.getRot(), 0);

            slib->use("diffuseCharacter");
            slib->setRegisteredUniform("colour", ngl::Vec4(character.getColour(),1.0f));
            drawAsset("person", "person_d", "diffuseCharacter");
        }
    }

    for(Baddie &baddie : m_baddies)
    {
        ngl::Vec3 pos = baddie.getPos();
        m_transform.reset();
        m_transform.setPosition(pos);
        m_transform.setRotation(0, baddie.getRot(), 0);
        m_transform.setScale(2.0f, 2.0f, 2.0f);

        slib->use("diffuse");
        drawAsset("person", "baddie_d", "diffuse");
    }

    for(ngl::Vec3 &stone : m_tombstones)
    {
        m_transform.reset();
        m_transform.setPosition(stone);
        drawAsset("tombstone", "tombstone_d", "diffuse");
    }
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
    for(size_t b = 0; b < m_meshInstances.size(); ++b)
    {
        int offset = 0;
        const auto &block = m_meshInstances[b];
        for(size_t i = 0; i < block.m_meshPositions.size(); ++i)
        {
            int instances = block.m_meshPositions[i].size();
            switch( i )
            {
            case static_cast<int>(TileType::TREES):
                drawInstances( "tree", "tree_d", "diffuseInstanced", instances, offset, b, m_shadowMat[_index] );
                break;
            case static_cast<int>(TileType::STOREHOUSE):
                drawInstances( "storehouse", "storehouse_d", "diffuseInstanced", instances, offset, b, m_shadowMat[_index] );
                break;
            case static_cast<int>(TileType::HOUSE):
                drawInstances( "house", "house_d", "diffuseInstanced", instances, offset, b, m_shadowMat[_index] );
                break;
            case static_cast<int>(TileType::FOUNDATION_A):
                drawInstances("foundation_A", "foundation_A_d", "diffuseInstanced", instances, offset, b, m_shadowMat[_index] );
                break;
            case static_cast<int>(TileType::FOUNDATION_B):
                drawInstances("foundation_B", "foundation_B_d", "diffuseInstanced", instances, offset, b, m_shadowMat[_index] );
                break;
            case static_cast<int>(TileType::FOUNDATION_C):
                drawInstances("foundation_C", "foundation_C_d", "diffuseInstanced", instances, offset, b, m_shadowMat[_index] );
                break;
            case static_cast<int>(TileType::FOUNDATION_D):
                drawInstances("foundation_D", "foundation_D_d", "diffuseInstanced", instances, offset, b, m_shadowMat[_index] );
                break;
            default:
                break;
            }
            offset += instances;
        }
    }

    slib->use("shadowDepth");
    for(auto &character : m_characters)
    {
        //if character is not in house or store house, draw
        if(character.isInside() == false)
        {
            ngl::Vec3 pos = character.getPos();
            m_transform.reset();
            m_transform.setPosition(pos);
            m_transform.setRotation(0, character.getRot(), 0);
            ngl::Mat4 mvp = m_transform.getMatrix() * m_shadowMat[_index];

            ngl::Obj * k = store->getModel( "person" );
            loadMatricesToShader( m_transform.getMatrix(), mvp );
            k->draw();
        }
    }

    for(Baddie &baddie : m_baddies)
    {
        ngl::Vec3 pos = baddie.getPos();
        m_transform.reset();
        m_transform.setPosition(pos);
        m_transform.setRotation(0, baddie.getRot(), 0);
        m_transform.setScale(baddie.getScale(), baddie.getScale(), baddie.getScale());
        ngl::Mat4 mvp = m_transform.getMatrix() * m_shadowMat[_index];

        ngl::Obj * k = store->getModel( "person" );
        loadMatricesToShader( m_transform.getMatrix(), mvp );
        k->draw();
    }

    for(ngl::Vec3 &stone : m_tombstones)
    {
        m_transform.reset();
        m_transform.setPosition(stone);
        ngl::Mat4 mvp = m_transform.getMatrix() * m_shadowMat[_index];

        ngl::Obj * k = store->getModel( "tombstone" );
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
    slib->setRegisteredUniform("waterLevel", m_grid.getGlobalWaterLevel());

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
    m_utilityBuffer = Framebuffer();
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

    if( gui->mousePos( mouse_coords ) )
    {
        gui->click();
    }
    else if(m_state == GameState::MAIN)
    {
        //read from charPick texture
        int red = getCharIDAtMouse();
        //if the value is not -1 (the default), then a character has been clicked
        if(red > 0)
        {
            for (Character &character : m_characters)
            {
                //if the value of the pixel matches a character's ID
                if (character.getID() == red)
                {
                    //if the character is not already the active character
                    if(!character.isActive())
                    {
                        //make the active character
                        m_active_char_id = character.getID();
                        character.setActive(true);
                        gui->updateActiveCharacter();
                    }
                }
                else
                    //make any other active character unactive
                    if (character.isActive())
                        character.setActive(false);
            }
        }
        //check grid_id texture
        else
        {
            //get the pixel value from the terrainPick texture
            ngl::Vec4 grid_coord = getTerrainPosAtMouse();
            //if the coordinate doesn't equal the default (0,0), then the grid has been clicked
            if(grid_coord[0] != 0 && grid_coord[2] != 0)
            {
                //floor to get integer value (to find tile)
                int grid_coord_x = floor(grid_coord[0]);
                grid_coord_x = Utility::clamp(grid_coord_x, 0, m_grid.getW());

                int grid_coord_y = floor(grid_coord[2]);
                grid_coord_y = Utility::clamp(grid_coord_y, 0, m_grid.getH());
                //set as target for active character
                int target_id = m_grid.coordToId(ngl::Vec2(grid_coord_x, grid_coord_y));
                for (Character &character : m_characters)
                {
                    if(character.isActive())
                    {
                        character.setState(target_id);
                    }
                }
            }
        }
        glReadBuffer(GL_NONE);
        m_utilityBuffer.unbind();
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

void Scene::drawInstances(const std::string &_model, const std::string &_texture, const std::string &_shader, const int _instances, const int _offset, const int _index)
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
    glBindTexture(GL_TEXTURE_BUFFER, m_meshInstances[_index].m_instanceTBO);
    //bindTextureToShader(_shader, m_instanceTBO, "transform", 1, GL_TEXTURE_BUFFER);

    m->bindVAO();
    glDrawArraysInstanced(GL_TRIANGLES, 0, m->getMeshSize(), _instances);
    m->unbindVAO();
}

void Scene::drawInstances(const std::string &_model, const std::string &_texture, const std::string &_shader, const int _instances, const int _offset, const int _index, const ngl::Mat4 &_VP)
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
    glBindTexture(GL_TEXTURE_BUFFER, m_meshInstances[_index].m_instanceTBO);
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

GLuint Scene::createBuffer1f(std::vector<float> _vec)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * _vec.size(),
                 &_vec[0],
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

    std::vector<std::vector<ngl::Vec3>> facenorms;

    faces.assign(m_grid.getW(), std::vector<ngl::Vec3>() );

    //Face centers to start.
    for(int i = 0; i < m_grid.getW(); ++i)
    {
        for(int j = 0; j < m_grid.getH(); ++j)
        {
            float height = m_grid.getTileHeight(i, j);
            ngl::Vec3 face (i, height, j);
            faces[i].push_back(face);
        }
    }

    std::cout << "Doing edges...\n";
    //Create edges.
    //x edges inner
    faces.insert(faces.begin(), std::vector<ngl::Vec3>());
    faces.push_back(std::vector<ngl::Vec3>());
    for(int i = 0; i < m_grid.getH(); ++i)
    {
        faces[0].push_back(ngl::Vec3(-1,m_grid.getTileHeight(0,i),i));
        faces[faces.size() - 1].push_back(ngl::Vec3(m_grid.getW(),m_grid.getTileHeight(m_grid.getW() - 1,i),i));
    }
    //x edges outer
    faces.insert(faces.begin(), std::vector<ngl::Vec3>());
    faces.push_back(std::vector<ngl::Vec3>());
    for(int i = 0; i < m_grid.getH(); ++i)
    {
        faces[0].push_back(ngl::Vec3(-1,-4.0f,i));
        faces[faces.size() - 1].push_back(ngl::Vec3(m_grid.getW(),-4.0f,i));
    }

    //y edges
    for(int i = 0; i < faces.size(); ++i)
    {
        //break;
        int clamped = Utility::clamp(i, 0, m_grid.getW() - 1);
        float fx = faces[i][0].m_x;
        faces[i].insert(faces[i].begin(), ngl::Vec3(fx, m_grid.getTileHeight(clamped,0), -1.0f));
        faces[i].insert(faces[i].begin(), ngl::Vec3(fx, -4.0f, -1.0f));
        faces[i].push_back(ngl::Vec3(fx, m_grid.getTileHeight(clamped, m_grid.getH() - 1), m_grid.getH()));
        faces[i].push_back(ngl::Vec3(fx, -4.0f, m_grid.getH()));
    }

    std::cout << "Calculating terrain  normals\n";
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
                        faces[i][j].m_x,
                        faces[i][j].m_z,
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

    //exit(EXIT_SUCCESS);
    m_terrainVAOSize = trimesh.size();

    m_height_tracer = TerrainHeightTracer(trimesh);

    return createVAO(
                trimesh,
                normesh,
                uvmesh
                );
}

Scene::terrainFace Scene::terrainVerticesToFace( const int _x,
                                                 const int _y,
                                                 const float _worldX,
                                                 const float _worldY,
                                                 const std::vector<std::vector<ngl::Vec3> > &_facePositions,
                                                 const std::vector<std::vector<ngl::Vec3> > &_faceNormals
                                                 )
{
    //std::cout << "terrainVerticesToFace()\n";
    //  2---3
    //  |   |
    //  0---1
    terrainFace face;

    //Generate xz positions
    face.m_verts[0].m_pos = ngl::Vec4(_worldX - 0.5f,0.0f,  _worldY - 0.5f,   1.0f);
    face.m_verts[1].m_pos = ngl::Vec4(_worldX + 0.5f,0.0f,  _worldY - 0.5f,   1.0f);
    face.m_verts[2].m_pos = ngl::Vec4(_worldX - 0.5f,0.0f,  _worldY + 0.5f,   1.0f);
    face.m_verts[3].m_pos = ngl::Vec4(_worldX + 0.5f,0.0f,  _worldY + 0.5f,   1.0f);

    std::pair<ngl::Vec4, ngl::Vec3> v0 = generateTerrainFaceData( _x, _y, -1, -1, _facePositions, _faceNormals );
    std::pair<ngl::Vec4, ngl::Vec3> v1 = generateTerrainFaceData( _x, _y, 1, -1, _facePositions, _faceNormals );
    std::pair<ngl::Vec4, ngl::Vec3> v2 = generateTerrainFaceData( _x, _y, -1, 1, _facePositions, _faceNormals );
    std::pair<ngl::Vec4, ngl::Vec3> v3 = generateTerrainFaceData( _x, _y, 1, 1, _facePositions, _faceNormals );

    face.m_verts[0].m_pos = v0.first;
    face.m_verts[1].m_pos = v1.first;
    face.m_verts[2].m_pos = v2.first;
    face.m_verts[3].m_pos = v3.first;

    face.m_verts[0].m_norm = v0.second;
    face.m_verts[1].m_norm = v1.second;
    face.m_verts[2].m_norm = v2.second;
    face.m_verts[3].m_norm = v3.second;

    return face;
}

// We give this function an x and y coordinate to start from, and a desired direction.
// The function averages attributes from neighbouring faces in the desired directions, if it is able.
// For example, starting at 0,0 (bottom left) with desired direction 1,-1 would explore only 1,0, since 0,-1 and 1,-1 do not exist.
std::pair<ngl::Vec4, ngl::Vec3> Scene::generateTerrainFaceData(const int _x,
                                                               const int _y,
                                                               const int _dirX,
                                                               const int _dirY,
                                                               const std::vector<std::vector<ngl::Vec3>> &_facePositions,
                                                               const std::vector<std::vector<ngl::Vec3>> &_faceNormals
                                                               )
{
    ngl::Vec4 position = _facePositions[_x][_y];
    ngl::Vec3 normal = _faceNormals[_x][_y];
    size_t count = 1;

    //Can we move in the horizontal direction?
    bool horizontal = (_x + _dirX) >= 0 and (_x + _dirX) < _facePositions.size();
    //Can we move in the vertical direction?
    bool vertical = (_y + _dirY) >= 0 and (_y + _dirY) < _facePositions[_x].size();
    //Can we move in the diagonal direction?
    bool diagonal = horizontal and (_y + _dirY) < _facePositions[_x + _dirX].size();

    if(horizontal)
    {
        position += _facePositions[_x + _dirX][_y];
        normal += _faceNormals[_x + _dirX][_y];
        count++;
    }
    if(vertical)
    {
        position += _facePositions[_x][_y + _dirY];
        normal += _faceNormals[_x][_y + _dirY];
        count++;
    }
    if(diagonal)
    {
        position += _facePositions[_x + _dirX][_y + _dirY];
        normal += _faceNormals[_x + _dirX][_y + _dirY];
        count++;
    }

    //Normalise normal
    normal /= static_cast<float>(count);
    normal.normalize();

    position /= static_cast<float>(count);
    position.m_w = 1.0f;
    //position.m_y = _facePositions[_x][_y].m_y;

    return std::make_pair(
                position,
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
    ngl::Random *rnd = ngl::Random::instance();
    Gui *gui = Gui::instance();
    MapList *maplist = MapList::instance();
    m_state = GameState::MAIN;
    m_game_started = true;
    gui->unpause();
    m_grid.updateScript(maplist->getCurrentMapPath(), maplist->getW(), maplist->getH(), maplist->getSeed());
    std::cout << "Constructing terrain...\n";
    m_terrainVAO = constructTerrain();

    initMeshInstances();

    // add first character
    createCharacter();
    m_active_char_id = m_characters[0].getID();
    gui->updateActiveCharacter();
    m_characters[0].setActive(true);

    //Number of clouds.
    int num_clouds = m_grid.getW() * m_grid.getH() / 200;
    //Number of particles per cloud.
    int num_cloud_particles = 32;

    for(int i = 0; i < num_clouds; ++i)
    {
        ngl::Vec3 p = rnd->getRandomPoint( m_grid.getW() / 2.0f, 4.0f, m_grid.getH() / 2.0f )
                + ngl::Vec3(m_grid.getW() / 2.0f, 25.0f, m_grid.getH() / 2.0f);
        m_cloudParticles.m_pos.push_back( p );
        m_cloudParticles.m_vel.push_back(ngl::Vec3());
        m_cloudParticles.m_scale.push_back( rnd->randomPositiveNumber(1.0f) + 2.5f );
        m_cloudParticles.m_time.push_back(rnd->randomPositiveNumber(3.0f));
    }

    for(int i = 0; i < num_clouds; ++i)
    {
        for(int j = 0; j < num_cloud_particles; ++j)
        {
            ngl::Vec3 p = m_cloudParticles.m_pos[i] + rnd->getRandomPoint( 4.0f, 0.5f, 4.0f );
            m_cloudParticles.m_pos.push_back(p);
            m_cloudParticles.m_vel.push_back(ngl::Vec3());
            m_cloudParticles.m_scale.push_back( rnd->randomPositiveNumber(1.0f) + 2.5f );
            m_cloudParticles.m_time.push_back(rnd->randomPositiveNumber(3.0f));
        }
    }
    m_cloudParticles.m_alpha.assign( m_cloudParticles.size(), 1.0f );

    glGenVertexArrays(1, &m_cloudParticlesVAO);
    glBindVertexArray(m_cloudParticlesVAO);
    m_cloudParticlesPositionVBO = createBuffer3f(m_cloudParticles.m_pos);
    setBufferLocation( m_cloudParticlesPositionVBO, 0, 3 );
    m_cloudParticlesScaleVBO = createBuffer1f(m_cloudParticles.m_scale);
    setBufferLocation( m_cloudParticlesScaleVBO, 1, 1 );
    m_cloudParticlesTimeVBO = createBuffer1f(m_cloudParticles.m_time);
    setBufferLocation( m_cloudParticlesTimeVBO, 2, 1 );
    m_cloudParticlesAlphaVBO = createBuffer1f(m_cloudParticles.m_alpha);
    setBufferLocation( m_cloudParticlesAlphaVBO, 3, 1 );
    glBindVertexArray(0);
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
    case GameState::ENDGAME:
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
    // negative values needed?
    m_cam.setPos(ngl::Vec3(-_pos.m_x, 0, -_pos.m_y));
}

void Scene::baddiesSpawn()
{
    m_baddie_timer++;

    if(m_baddie_timer > (1000 / m_prefs->getFloatPref("ENEMIE_SPAWN_RATE")))
    {
        m_baddie_timer = 0;
        ngl::Random *rnd = ngl::Random::instance();

        size_t character_index = floor(rnd->randomPositiveNumber(m_characters.size()));
        for (size_t i = 0; i < 1000; i++)
        {
            ngl::Vec2 direction = rnd->getRandomNormalizedVec2();
            float dist = 10 + rnd->randomPositiveNumber(10);
            direction *= dist;
            ngl::Vec2 rand_pos = m_characters[character_index].getPos2D() + direction;
            if(m_grid.isTileTraversable(rand_pos.m_x, rand_pos.m_y))
            {
                m_baddies.push_back(Baddie(rand_pos, &m_height_tracer, &m_grid, &m_characters));
                break;
            }
        }
    }
}

void Scene::charactersSpawn()
{
    m_character_timer++;
    if(m_character_timer > (1000 / m_prefs->getFloatPref("CHARACTER_SPAWN_RATE")))
    {
        m_character_timer = 0;
        int population = getPopulation();
        int max_population = getMaxPopulation();
        if (population < max_population)
        {
            createCharacter();
        }
    }
}

void Scene::endGame(const std::string &_message)
{
    m_state = GameState::ENDGAME;
    Gui::instance()->createEndGameButtons(_message);
    m_game_started = false;
    m_grid.hasChanges();
    m_meshInstances.clear();
    // clear baddie vector
    m_baddies.clear();
    m_characters.clear();
    m_active_char_id = -1;
}

ngl::Vec4 Scene::getTerrainPosAtMouse()
{
    m_utilityBuffer.bind();
    GLuint grid_texID = getTerrainPickTexture();
    glBindTexture(GL_TEXTURE_2D, grid_texID);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    ngl::Vec2 mouse_coords = Utility::getMousePos();
    ngl::Vec4 grid_coord = ngl::Vec4();

    // x, window height - y
    glReadPixels(mouse_coords.m_x, (m_viewport.m_y - mouse_coords.m_y), 1, 1, GL_RGBA, GL_FLOAT, &grid_coord.m_x);
    m_utilityBuffer.unbind();

    //std::cout << "wmp = " << grid_coord << '\n';
    return grid_coord;
}

int Scene::getCharIDAtMouse()
{
    //bind fbo
    m_utilityBuffer.bind();

    ngl::Vec2 mousePos = Utility::getMousePos();

    //bind charPick texture
    GLuint char_texID = getCharPickTexture();
    glBindTexture(GL_TEXTURE_2D, char_texID);
    glReadBuffer(GL_COLOR_ATTACHMENT1);

    int red = 0;
    //read the pixel in the texture
    glReadPixels(mousePos.m_x, (m_viewport.m_y - mousePos.m_y), 1, 1, GL_RED_INTEGER, GL_INT, &red);

    //unbind fbo
    m_utilityBuffer.unbind();

    //red defaults to 0, so use -1 for checking
    return red - 1;
}

void Scene::quit()
{
    m_active = false;
}

bool Scene::isActive()
{
    return m_active;
}
