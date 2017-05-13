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
    /// @brief endGame
    /// @param _message
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
    /// @brief m_prefs
    ///
    Prefs* m_prefs;
    ///
    /// @brief m_cam
    ///
    Camera m_cam;
    ///
    /// @brief m_grid
    ///
    Grid m_grid;
    ///
    /// @brief m_world_inventory, global inventory of store houses, controls how much can be stored 
    /// and keeps track of what has been stored
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
    /// @brief loadMatricesToShader
    /// @param _M
    /// @param _MVP
    ///
    void loadMatricesToShader(const ngl::Mat4 _M, const ngl::Mat4 _MVP);
    ///
    /// @brief loadMatricesToShader
    ///
    void loadMatricesToShader();
    ///
    /// @brief checks the colour of the pixel where the user has clicked and sets states on objects
    /// 
    void mouseSelection();
    ///
    /// @brief bindTextureToShader
    /// @param _shaderID
    /// @param _tex
    /// @param _uniform
    /// @param _target
    /// @param _type
    ///
    void bindTextureToShader(const std::string &_shaderID, const GLuint _tex, const char *_uniform, int _target, GLenum _type = GL_TEXTURE_2D);
    ///
    /// @brief drawAsset
    /// @param _model
    /// @param _texture
    /// @param _shader
    ///
    void drawAsset(const std::string &_model, const std::string &_texture, const std::string &_shader);
    ///
    /// @brief drawInstances
    /// @param _model
    /// @param _texture
    /// @param _shader
    /// @param _instances
    /// @param _offset
    /// @param _index
    ///
    void drawInstances(const std::string &_model, const std::string &_texture, const std::string &_shader, const int _instances, const int _offset, const int _index);
    ///
    /// @brief drawInstances
    /// @param _model
    /// @param _texture
    /// @param _shader
    /// @param _instances
    /// @param _offset
    /// @param _index
    /// @param _VP
    ///
    void drawInstances(const std::string &_model, const std::string &_texture, const std::string &_shader, const int _instances, const int _offset, const int _index, const ngl::Mat4 &_VP);
    ///
    /// @brief createShader
    /// @param _name
    /// @param _vert
    /// @param _frag
    /// @param _geo
    /// @param _tessctrl
    /// @param _tesseval
    ///
    void createShader(const std::string _name, const std::string _vert, const std::string _frag, const std::string _geo = "", const std::string _tessctrl = "", const std::string _tesseval = "");
    ///
    /// @brief createVAO
    /// @param _verts
    /// @return 
    ///
    GLuint createVAO(std::vector<ngl::Vec4> &_verts);
    ///
    /// @brief createVAO
    /// @param _verts
    /// @param _uvs
    /// @return 
    ///
    GLuint createVAO(std::vector<ngl::Vec4> &_verts, std::vector<ngl::Vec2> &_uvs);
    ///
    /// @brief createVAO
    /// @param _verts
    /// @param _normals
    /// @param _uvs
    /// @return 
    ///
    GLuint createVAO(std::vector<ngl::Vec4> &_verts, std::vector<ngl::Vec3> &_normals, std::vector<ngl::Vec2> &_uvs);
    ///
    /// @brief createBuffer4f
    /// @param _vec
    /// @return 
    ///
    GLuint createBuffer4f(std::vector<ngl::Vec4> _vec);
    ///
    /// @brief createBuffer3f
    /// @param _vec
    /// @return 
    ///
    GLuint createBuffer3f(std::vector<ngl::Vec3> _vec);
    ///
    /// @brief createBuffer2f
    /// @param _vec
    /// @return 
    ///
    GLuint createBuffer2f(std::vector<ngl::Vec2> _vec);
    ///
    /// @brief createBuffer1f
    /// @param _vec
    /// @return 
    ///
    GLuint createBuffer1f(std::vector<float> _vec);
    ///
    /// @brief setBufferLocation
    /// @param _buffer
    /// @param _index
    /// @param _size
    ///
    void setBufferLocation(GLuint _buffer, int _index, int _size);
    ///
    /// @brief Wrapper for getTerrainPickTexture, returns the encoded world pos of the "terrainpos" texture at the mouse position.
    /// 
    ngl::Vec4 getTerrainPosAtMouse();
    ///
    /// @brief getTerrainPickTexture
    /// @return 
    ///
    GLuint getTerrainPickTexture() {return m_utilityBuffer.get("terrainpos");}
    ///
    /// @brief Wrapper for getTerrainPickTexture, returns the encoded id of the "charid" texture at the mouse position.
    /// 
    int getCharIDAtMouse();
    ///
    /// @brief getCharPickTexture
    /// @return 
    ///
    GLuint getCharPickTexture() {return m_utilityBuffer.get("charid");}
    ///
    /// @brief generateOrthoShadowMatrices
    /// @param _divisions
    /// @return 
    ///
    std::pair<std::vector<bounds>, std::vector<bounds> > generateOrthoShadowMatrices(const std::vector<float> &_divisions);
    ///
    /// @brief shadowPass
    /// @param _worldbox
    /// @param _lightbox
    /// @param _index
    ///
    void shadowPass(bounds _worldbox, bounds _lightbox, size_t _index );
    ///
    /// @brief m_globalTime
    ///
    float m_globalTime;
    ///
    /// @brief m_sunAngle
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
    /// @brief The terrainVertex struct
    ///
    struct terrainVertex
    {
        ngl::Vec4 m_pos;
        ngl::Vec3 m_norm;
    };
    ///
    /// @brief The terrainFace struct
    ///
    struct terrainFace
    {
        //  2---3
        //  |   |
        //  0---1
        std::array< terrainVertex, 4 > m_verts;
    };
    ///
    /// @brief terrainVerticesToFace
    /// @param _x
    /// @param _y
    /// @param _facePositions
    /// @param _faceNormals
    /// @return 
    ///
    terrainFace terrainVerticesToFace(const int _x,
                                      const int _y, const float _worldX, const float _worldY,
                                      const std::vector<std::vector<ngl::Vec3> > &_facePositions,
                                      const std::vector<std::vector<ngl::Vec3> > &_faceNormals);
    ///
    /// @brief generateTerrainFaceData
    /// @param _x
    /// @param _y
    /// @param _dirX
    /// @param _dirY
    /// @param _facePositions
    /// @param _faceNormals
    /// @return 
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
    ///
    /// @brief Rather than looping through the grid every frame, and drawing based on tile id, I extract out the positions of the meshes that need
    /// to be drawn, and place them in this 2D vector, where the outer index matches their ID. Obviously, there is some wasted space here, I may
    /// improve the design at a later date.
    /// This is now stored in blocks, which cuts down on the recalculation time when a mesh changes.
    const int m_meshInstanceBlockTileSize = 20;
    struct meshInstanceBlock
    {
        //The positions (and implicitly IDs) of the meshes.
        std::vector< std::vector<ngl::Vec3> > m_meshPositions;
        //The bounds of the block.
        std::pair<ngl::Vec3, ngl::Vec3> m_bounds;
        //The texture buffer object containing the transformation data.
        GLuint m_instanceTBO;
    };
    std::vector<meshInstanceBlock> m_meshInstances;
    void initMeshInstances();
    meshInstanceBlock generateInstanceMeshTile(const int _x, const int _y);
    void recalculateInstancedMeshes(int _tilex, int _tiley);

    //Use these to draw debug points to the screen. Should be deleted/hidden at some point.
    GLuint m_debugVAO;
    ///
    /// @brief m_debugVBO
    ///
    GLuint m_debugVBO;
    ///
    /// @brief m_debugPoints
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
