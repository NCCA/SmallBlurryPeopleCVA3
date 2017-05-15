#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include<memory>

#include <ngl/Transformation.h>

#include "AssetStore.hpp"
#include "Grid.hpp"
#include "Camera.hpp"
#include "AI.hpp"
#include "Character.hpp"
#include "Baddie.hpp"
#include "Inventory.hpp"
#include "IVal.hpp"
#include "Light.hpp"
#include "ParticleSystem.hpp"
#include "Prefs.hpp"
#include "TerrainHeightTracer.hpp"

#include "Framebuffer.hpp"

///
/// @brief The Direction enum clarification for which direction is being used
///
enum Direction
{
  FORWARDS,
  BACKWARDS,
  LEFTWARDS,
  RIGHTWARDS
};
///
/// @brief The GameState enum refers to what state the game is currently in:
/// MAIN <--> PAUSE <--> PREFERENCES
///
enum GameState
{
  START_MENU,
  MAIN,
  PAUSE,
  PREFERENCES,
  ENDGAME
};

///
/// @brief bounds
///
typedef std::pair<ngl::Vec3, ngl::Vec3> bounds;

class Scene
{
public:
    ///
    /// @brief constructor
    /// @param _viewport
    ///
    Scene(ngl::Vec2 _viewport);
    ///
    /// @brief deconstructor
    /// 
    ~Scene() = default;
    ///
    /// @brief draw
    ///
    void draw();
    ///
    /// @brief draws the sky
    ///
    void drawSky(bool _flipped = false);
    ///
    /// @brief draws the terrain
    ///
    void drawTerrain(bool _shouldClip = false);
    ///
    /// @brief draws the meshes
    ///
    void drawMeshes();
    ///
    /// @brief update
    ///
    void update();
    ///
    /// @brief quit close the program
    ///
    void quit();
    ///
    /// @brief isActive check whether program should still be running
    /// @return m_active to see if program should quit if it's false
    ///
    bool isActive();
    ///
    /// @brief Checks which mouse button has been pressed and sets relevenat mouse state to active
    /// @param _event, SDL mouse event structure
    /// 
    void mousePressEvent(const SDL_MouseButtonEvent &_event);
    ///
    /// @brief Checks which mouse button has been released and sets relevant mouse state to false,
    /// also calls mouseSelection for clicks
    /// @param _event, SDL mouse event structure
    /// 
    void mouseReleaseEvent(const SDL_MouseButtonEvent &_event);
    ///
    /// @brief Sets the mouse pan according to the mouse wheel
    /// @param _event, SDL mouse event structure
    /// 
    void wheelEvent(const SDL_MouseWheelEvent &_event);
    ///
    /// @brief zoom move the camera in or out
    /// @param _direction direction, positive to zoom in, negative to zoom out
    ///
    void zoom(int _direction);
    ///
    /// @brief keyDownEvent called when a key is pressed
    /// @param _event SDL keyboard event structure
    ///
    void keyDownEvent(const SDL_KeyboardEvent &_event);
    ///
    /// @brief keyUpEvent called when a key is released
    /// @param _event SDL keyboard event structure
    ///
    void keyUpEvent(const SDL_KeyboardEvent &_event);
    ///
    /// @brief updateMousePos check where mouse is for gui
    ///
    void updateMousePos();
    ///
    /// @brief windowEvent called upon window event such as resize to update resolution parameters
    /// @param _event SDL window event
    ///
    void windowEvent(const SDL_WindowEvent &_event);
    ///
    /// @brief Initialises all of the framebuffers. Separated into its own unit so it can be called when the viewport resizes.
    ///
    void initialiseFramebuffers();
    ///
    /// @brief All the messy code to resize the viewport. Updates shaders, framebuffers etc.
    ///
    void resize(const ngl::Vec2 &_dim);
    ///
    /// @brief Centre camera on the active character
    /// 
    void centreCamera();
    ///
    /// @brief getActiveCharacter get the active character in the scene
    /// @return a pointer to the active character
    ///
    Character *getActiveCharacter();
    ///
    /// @brief getActiveCharacterName get the name of the active character
    /// @return active character's name, or an empty string if no active character
    ///
    std::string getActiveCharacterName();
    ///
    /// @brief togglePause switch between paused and unpaused mode
    ///
    void togglePause();
    ///
    /// @brief startGame leave main menu and start the game
    ///
    void startGame();
    ///
    /// @brief startMove set movement flag in given direction to true
    /// @param _d direction to move
    ///
    void startMove(Direction _d);
    ///
    /// @brief stopMove set movement flag in given direction to false
    /// @param _d direction to move
    ///
    void stopMove(Direction _d);
    ///
    /// @brief prefsMode show/hide preferences
    ///
    void prefsMode();
    ///
    /// @brief escapeState leave current state
    ///
    void escapeState();
    ///
    /// @brief getState return the game state
    /// @return m_state
    ///
    GameState getState();
    ///
    /// @brief focusCamToGridPos send the target to the given position
    /// @param _pos position for the camera to focus in on
    ///
    void focusCamToGridPos(ngl::Vec2 _pos);
    ///
    /// @brief baddiesSpawn manage baddie spawning
    ///
    void baddiesSpawn();
    ///
    /// @brief charactersSpawn spawn characters at houses if there's space
    ///
    void charactersSpawn();
    ///
    /// \brief endGame return to main menu
    /// \param _message text for first button in main menu
    ///
    void endGame(const std::string &_message);
    ///
    /// @brief getPopulation get the number of characters in the scene
    /// @return size of the characters vector
    ///
    int getPopulation() {return m_characters.size();}
    ///
    /// @brief getMaxPopulation get maximum possible population
    /// @return maximum population
    ///
    int getMaxPopulation() {return m_grid.getNumHouses()*Prefs::instance()->getIntPref("CHARACTERS_PER_HOUSE");}

private:
    ///
    /// @brief m_active whether scene is still active (if user has not quit)
    ///
    bool m_active;
    ///
    /// \brief m_prefs pointer to preferences
    ///
    Prefs* m_prefs;
    ///
    /// \brief m_cam camera that user sees through
    ///
    Camera m_cam;
    ///
    /// \brief m_grid grid to use for map traversal and terrain generation etc
    ///
    Grid m_grid;
    ///
    /// \brief m_world_inventory user's total inventory
    ///
    Inventory m_world_inventory;
    ///
    /// @brief m_file_names, vector of available names for characters
    ///
    std::vector<std::string> m_file_names;
    ///
    /// @brief m_character, vector of characters that have been created
    ///
    std::vector<Character> m_characters;
    ///
    /// @brief m_tombstones, vector of positions for tombstones
    ///
    std::vector<ngl::Vec3> m_tombstones;
    ///
    /// @brief m_baddies vector of baddie objects
    ///
    std::vector<Baddie> m_baddies;
    ///
    /// @brief m_active_char_id id used to keep track of active character, more stable than using addresses
    ///
    int m_active_char_id;
    ///
    /// @brief m_transform
    ///
    ngl::Transformation m_transform;
    ///
    /// @brief sets if mouse is being used to translate the scene
    /// 
    bool m_mouse_trans_active;
    ///
    /// @brief sets if mouse is being used to rotate the scene
    /// 
    bool m_mouse_rot_active;
    ///
    /// @brief sets if centering camera on character
    /// 
    bool m_centre_camera;
    ///
    /// @brief stores mouse position when m_mouse_trans_active is true
    /// 
    ngl::Vec2 m_mouse_trans_origin;
    ///
    /// @brief stores mouse x position when m_mouse_rot_active is true
    /// 
    float m_mouse_rot_origin;
    ///
    /// @brief stores mouse position when pressed down to check if mouse has moved at release
    /// 
    ngl::Vec2 m_mouse_prev_pos;
    ///
    /// @brief terrain base mesh
    /// 
    GLuint m_terrainVAO;
    ///
    /// @brief vert count of terrain mesh
    /// 
    size_t m_terrainVAOSize;
    ///
    /// @brief Unit square VAO
    /// 
    GLuint m_unitSquareVAO;
    ///
    /// @brief a screen quad, used mostly for postprocessing
    /// 
    GLuint m_screenQuad;
    ///
    /// @brief framebuffer to store data for the deferred rendering pipeline
    /// 
    Framebuffer m_mainBuffer;
    ///
    /// @brief framebuffer to store id and positional data, to be clicked on
    /// 
    Framebuffer m_utilityBuffer;
    ///
    /// @brief buffer to store depth for shadowing
    /// 
    Framebuffer m_shadowBuffer;
    ///
    /// @brief buffer to store displacement map, currently for water only.
    /// 
    Framebuffer m_displacementBuffer;
    ///
    /// @brief This buffer will store textures for post effects (DOF, reflections).
    /// 
    Framebuffer m_postEffectsBuffer;
    ///
    /// @brief opens game_names.txt and stores the names in the vector m_file_names
    ///
    void readNameFile();
    ///
    /// @brief choses a random name and creates a character object with the name chosen
    ///
    void createCharacter();
    ///
    /// @brief Loads a custom set of matrices to the active shader
    /// @param _M, The model matrix.
    /// @param _MVP, The model-view-project matrix.
    ///
    void loadMatricesToShader(const ngl::Mat4 _M, const ngl::Mat4 _MVP);
    ///
    /// @brief Loads the MVP matrix to the active shader.
    ///
    void loadMatricesToShader();
    ///
    /// @brief checks the colour of the pixel where the user has clicked and sets states on objects
    /// 
    void mouseSelection();
    ///
    /// @brief Binds a provided texture to a provided shader.
    /// @param _shaderID, The id of the shader
    /// @param _tex, The opengl id of the texture
    /// @param _uniform, The uniform name
    /// @param _target, The texture target
    /// @param _type, The type of texture (normally GL_TEXTURE_2D)
    ///
    void bindTextureToShader(const std::string &_shaderID, const GLuint _tex, const char *_uniform, int _target, GLenum _type = GL_TEXTURE_2D);
    ///
    /// @brief Draws a specified asset with a texture and shader.
    /// @param _model, The string id of the model, from the asset store.
    /// @param _texture, The string id of the texture, from the asset store.
    /// @param _shader, The string id of the shader.
    ///
    void drawAsset(const std::string &_model, const std::string &_texture, const std::string &_shader);
    ///
    /// @brief Draws a set of instances meshes.
    /// @param _model, The string id of the model, from the asset store.
    /// @param _texture, The string id of the texture, from the asset store.
    /// @param _shader, The string id of the shader.
    /// @param _instances, The number of instances to draw.
    /// @param _offset, The offset into the mesh instance TBO to start from.
    /// @param _index, The index of the mesh instance block to source transformation data from.
    ///
    void drawInstances(const std::string &_model, const std::string &_texture, const std::string &_shader, const int _instances, const int _offset, const int _index);
    ///
    /// @brief Draws a set of instances meshes, with a custom VP matrix. Used for shadow mapping.
    /// @param _model, The string id of the model, from the asset store.
    /// @param _texture, The string id of the texture, from the asset store.
    /// @param _shader, The string id of the shader.
    /// @param _instances, The number of instances to draw.
    /// @param _offset, The offset into the mesh instance TBO to start from.
    /// @param _index, The index of the mesh instance block to source transformation data from.
    /// @param _VP, The custom VP matrix to use
    ///
    void drawInstances(const std::string &_model, const std::string &_texture, const std::string &_shader, const int _instances, const int _offset, const int _index, const ngl::Mat4 &_VP);
    ///
    /// @brief Creates a shader with the given attributes
    /// @param _name, The name the shader will be stored under in ShaderLib
    /// @param _vert, The filename of the vertex shader (without '.glsl').
    /// @param _frag, The filename of the fragment shader (without '.glsl').
    /// @param _geo, The filename of the geometry shader (without '.glsl').
    /// @param _tessctrl, The filename of the tesselation control shader (without '.glsl').
    /// @param _tesseval, The filename of the tesselation evaluation shader (without '.glsl').
    ///
    void createShader(const std::string _name, const std::string _vert, const std::string _frag, const std::string _geo = "", const std::string _tessctrl = "", const std::string _tesseval = "");
    ///
    /// @brief Creates a VAO from a list of supplied points
    /// @param _verts, The verts to use.
    /// @return _vao, The finished VAO.
    ///
    GLuint createVAO(std::vector<ngl::Vec4> &_verts);
    ///
    /// @brief Creates a VAO from a list of supplied points and UV coordinates
    /// @param _verts, The verts to use.
    /// @param _uvs, The UVs to use.
    /// @return _vao, The finished VAO.
    ///
    GLuint createVAO(std::vector<ngl::Vec4> &_verts, std::vector<ngl::Vec2> &_uvs);
    ///
    /// @brief Creates a VAO from a list of supplied points, normal vectors and UV coordinates
    /// @param _verts, The verts to use.
    /// @param _normals, The normals to use.
    /// @param _uvs, The UVs to use.
    /// @return _vao, The finished VAO.
    ///
    GLuint createVAO(std::vector<ngl::Vec4> &_verts, std::vector<ngl::Vec3> &_normals, std::vector<ngl::Vec2> &_uvs);
    ///
    /// @brief Creates a VBO containing vec4s.
    /// @param _vec, The vec4s to use.
    /// @return _vbo, The finished VBO.
    ///
    GLuint createBuffer4f(std::vector<ngl::Vec4> _vec);
    ///
    /// @brief Creates a VBO containing vec3s.
    /// @param _vec, The vec3s to use.
    /// @return _vbo, The finished VBO.
    ///
    GLuint createBuffer3f(std::vector<ngl::Vec3> _vec);
    ///
    /// @brief Creates a VBO containing vec2s.
    /// @param _vec, The vec2s to use.
    /// @return _vbo, The finished VBO.
    ///
    GLuint createBuffer2f(std::vector<ngl::Vec2> _vec);
    ///
    /// @brief Creates a VBO containing floats.
    /// @param _vec, The floats to use.
    /// @return _vbo, The finished VBO.
    ///
    GLuint createBuffer1f(std::vector<float> _vec);
    ///
    /// @brief Wraps a call to glVertexAttribPointer.
    /// @param _buffer, The VBO to use.
    /// @param _index, The index to attach to.
    /// @param _size, The size of the VBO.
    ///
    void setBufferLocation(GLuint _buffer, int _index, int _size);
    ///
    /// @brief Wrapper for getTerrainPickTexture, returns the encoded world pos of the "terrainpos" texture at the mouse position.
    /// 
    ngl::Vec4 getTerrainPosAtMouse();
    ///
    /// @brief Gets the texture with terrain position data.
    /// @return m_utilityBuffer.get("terrainpos");, The texture in the utility FBO with ID "terrainpos".
    ///
    GLuint getTerrainPickTexture() {return m_utilityBuffer.get("terrainpos");}
    ///
    /// @brief Wrapper for getTerrainPickTexture, returns the encoded id of the "charid" texture at the mouse position.
    /// 
    int getCharIDAtMouse();
    ///
    /// @brief Gets the texture with character id data.
    /// @return m_utilityBuffer.get("charid");, The texture in the utility FBO with ID "charid".
    ///
    GLuint getCharPickTexture() {return m_utilityBuffer.get("charid");}
    ///
    /// @brief Generates the set of bounds for AABBs around different sections of the camera cascades.
    /// @param _divisions, The distances to split the camera cascades at.
    /// @return _boxes, A set of AABBs enclosing sections of the camera frustum.
    ///
    std::pair<std::vector<bounds>, std::vector<bounds> > generateOrthoShadowMatrices(const std::vector<float> &_divisions);
    ///
    /// @brief Creates a depth texture from sun-light-space of the scene.
    /// @param _worldbox, The world-space bounding box for the orthographic render box.
    /// @param _lightbox, The light space bounding box for the orthographic render box, this is used to control the projection matrix.
    /// @param _index, The index in the vector of depth textures to store the result of the shadow pass in.
    ///
    void shadowPass(bounds _worldbox, bounds _lightbox, size_t _index );
    ///
    /// @brief The time since the start of the game.
    ///
    float m_globalTime;
    ///
    /// @brief The angle of the sun.
    ///
    ngl::Vec3 m_sunAngle;
    ///
    /// @brief m_sunDir, The direction the sun is pointing (not unit vector TO the light)
    ///
    ngl::Vec3 m_sunDir;
    ///
    /// @brief m_directionalLightCol
    ///
    ngl::Vec3 m_directionalLightCol;
    ///
    /// @brief m_day, Tracks days since the start of the game.
    ///
    int m_day;
    ///
    /// @brief 0 = January 1st, 1 = December 31st.
    /// 
    float m_season;
    ///
    /// @brief This drives the clouds
    /// 
    IVal<ngl::Vec3> m_windDirection;
    ///
    /// @brief m_cloudParticles
    ///
    ParticleSystem m_cloudParticles;
    ///
    /// @brief m_cloudParticlesVAO
    ///
    GLuint m_cloudParticlesVAO;
    ///
    /// @brief m_cloudParticlesPositionVBO
    ///
    GLuint m_cloudParticlesPositionVBO;
    ///
    /// @brief m_cloudParticlesScaleVBO
    ///
    GLuint m_cloudParticlesScaleVBO;
    ///
    /// @brief m_cloudParticlesTimeVBO
    ///
    GLuint m_cloudParticlesTimeVBO;
    ///
    /// @brief m_cloudParticlesAlphaVBO
    ///
    GLuint m_cloudParticlesAlphaVBO;
    ///
    /// @brief m_shadowMat
    ///
    std::vector<ngl::Mat4> m_shadowMat;
    ///
    /// @brief Tells me how big the screen is.
    /// 
    ngl::Vec2 m_viewport;
    ///
    /// @brief Takes data from the grid and turns the ids into shapes like mountains.
    ///
    GLuint constructTerrain();
    ///
    /// @brief The terrainVertex struct, contains a position and a normal.
    ///
    struct terrainVertex
    {
        ngl::Vec4 m_pos;
        ngl::Vec3 m_norm;
    };
    ///
    /// @brief The terrainFace struct, contains 4 terrain vertices.
    ///
    struct terrainFace
    {
        //  2---3
        //  |   |
        //  0---1
        std::array< terrainVertex, 4 > m_verts;
    };
    ///
    /// @brief Constructs a face for a given terrain tile.
    /// @param _x, The x coordinate of the tile.
    /// @param _y, The y coordinate of the tile.
    /// @param _facePositions, A 2d vector of face center positions.
    /// @param _faceNormals, A 2d vector of face center normals.
    /// @return _face, The face constructed from the input data.
    ///
    terrainFace terrainVerticesToFace(const int _x,
                                      const int _y, const float _worldX, const float _worldY,
                                      const std::vector<std::vector<ngl::Vec3> > &_facePositions,
                                      const std::vector<std::vector<ngl::Vec3> > &_faceNormals);
    ///
    /// @brief Generates a vertex given a terrain tile, and information about the surrounding tiles.
    /// @param _x, The x coordinate of the tile.
    /// @param _y, The y coordinate of the tile.
    /// @param _dirX, The x direction to compare to, ie vertices with a dirX of 1 will look at tiles to the right.
    /// @param _dirY, The x direction to compare to, ie vertices with a dirY of 1 will look at tiles upwards.
    /// @param _facePositions, A 2d vector of face center positions.
    /// @param _faceNormals, A 2d vector of face center normals.
    /// @return _vert, A vertex creates from nearby tiles.
    ///
    std::pair<ngl::Vec4, ngl::Vec3> generateTerrainFaceData(const int _x,
                                                        const int _y,
                                                        const int _dirX,
                                                        const int _dirY,
                                                        const std::vector<std::vector<ngl::Vec3>> &_facePositions,
                                                        const std::vector<std::vector<ngl::Vec3>> &_faceNormals
                                                        );
    ///
    /// @brief getCamMoveVec get the user's movement input
    /// @return vector for camera to move
    ///
    ngl::Vec3 getCamMoveVec();

    /// @brief The size of one side of each mesh instance block. Values below 50 should work well.
    const int m_meshInstanceBlockTileSize = 20;

    ///
    /// @brief Rather than looping through the grid every frame, and drawing based on tile id, I extract out the positions of the meshes that need
    /// to be drawn, and place them in this 2D vector, where the outer index matches their ID. Obviously, there is some wasted space here, I may
    /// improve the design at a later date.
    /// This is now stored in blocks, which cuts down on the recalculation time when a mesh changes.
    /// Each block contains the positions and IDs of each mesh, CPU side, the world space bounds of the chunk (can be used for culling) and the texture buffer object containing the transformation data.
    struct meshInstanceBlock
    {
        /// @brief The positions (and implicitly IDs) of the meshes.
        std::vector< std::vector<ngl::Vec3> > m_meshPositions;
        /// @brief The bounds of the block.
        std::pair<ngl::Vec3, ngl::Vec3> m_bounds;
        /// @brief The texture buffer object containing the transformation data.
        GLuint m_instanceTBO;
    };
    /// @brief A vector of all the mesh instance blocks in the game.
    std::vector<meshInstanceBlock> m_meshInstances;

    /// @brief Generates mesh instance blocks. This should be called on a new game.
    void initMeshInstances();

    /// @brief Generates a mesh instance block based on an input block (tilecoordinate / m_meshInstanceBlockTileSize) coordinate.
    meshInstanceBlock generateInstanceMeshTile(const int _x, const int _y);

    /// @brief Recalculates the mesh instance block based on a tile coordinate. When the map changes, we can call this function with the coordinates of the tile that has changed, and it will update.
    void recalculateInstancedMeshes(int _tilex, int _tiley);

    ///
    /// @brief Used to draw debug data to the screen.
    ///
    GLuint m_debugVAO;
    ///
    /// @brief Used to draw debug data to the screen.
    ///
    GLuint m_debugVBO;
    ///
    /// @brief Used to draw debug data to the screen.
    ///
    std::vector<ngl::Vec4> m_debugPoints;
    ///
    /// @brief m_state current game state, MAIN, PAUSE, PREFERENCES etc
    ///
    GameState m_state;
    ///
    /// @brief m_game_started false if game is in title screen, true otherwise
    ///
    bool m_game_started;
    ///
    /// @brief m_movement_held which movement are currently held
    ///
    bool m_movement_held[4];
    ///
    /// @brief m_baddie_timer timer to spawn baddies
    ///
    size_t m_baddie_timer = 0;
    ///
    /// @brief m_character_timer timer to spawn characters
    ///
    size_t m_character_timer = 0;
    ///
    /// @brief Where the mouse selection box is.
    /// 
    IVal<ngl::Vec3> m_mouseSelectionBoxPosition;
    ///
    /// @brief How the mouse selection box is scaled. For example, when hovering over a character it gets taller and thinner.
    ///
    IVal<ngl::Vec3> m_mouseSelectionBoxScale;
    ///
    /// @brief Put your point lights in this vector and they'll get drawn as a part of the lighting pass.
    ///
    std::vector<Light> m_pointLights;
    ///
    /// @brief m_maxLights
    ///
    const size_t m_maxLights = 512;
    ///
    /// @brief m_lightBuffer
    ///
    GLuint m_lightBuffer;
    ///
    /// @brief m_height_tracer
    ///
    TerrainHeightTracer m_height_tracer;
};

#endif//__SCENE_HPP__
