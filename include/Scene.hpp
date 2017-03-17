#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include<memory>

#include <ngl/Transformation.h>

#include "AssetStore.hpp"
#include "Grid.hpp"
#include "Camera.hpp"
#include "Character.hpp"
#include "Inventory.hpp"
#include "Preferences.hpp"

#include "Framebuffer.hpp"

typedef std::pair<ngl::Vec3, ngl::Vec3> bounds;

class Scene
{
public:
    Scene(ngl::Vec2 _viewport);
    ~Scene() = default;
    void draw();
    /// \brief draws the sky
    void drawSky();
    /// \brief draws the terrain
    void drawTerrain();
    /// \brief draws the meshes
    void drawMeshes();
    /// \brief draws the meshes, but culls each mesh which falls out of all of the _frustumBoxes.
    void drawMeshes(const std::vector<bounds> &_frustumBoxes);

    void update();
    ///
    /// \brief quit close the program
    ///
    void quit();
    ///
    /// \brief isActive check whether program should still be running
    /// \return m_active to see if program should quit if it's false
    ///
    bool isActive();

    /// @brief Checks which mouse button has been pressed and sets relevenat mouse state to active
    /// @param _event, SDL mouse event structure
    void mousePressEvent(const SDL_MouseButtonEvent &_event);

    /// @brief Checks which mouse button has been released and sets relevant mouse state to false,
    /// also calls mouseSelection for clicks
    /// @param _event, SDL mouse event structure
    void mouseReleaseEvent(const SDL_MouseButtonEvent &_event);

    /// @brief Sets the mouse pan according to the mouse wheel
    /// @param _event, SDL mouse event structure
    void wheelEvent(const SDL_MouseWheelEvent &_event);
    ///
    /// \brief updateMousePos check where mouse is for gui
    ///
    void updateMousePos();
    ///
    /// \brief windowEvent called upon window event such as resize to update resolution parameters
    /// \param _event SDL window event
    ///
    void windowEvent(const SDL_WindowEvent &_event);

    /// \brief Initialises all of the framebuffers. Separated into its own unit so it can be called when the viewport resizes.
    void initialiseFramebuffers();

    /// \brief All the messy code to resize the viewport. Updates shaders, framebuffers etc.
    void resize(const ngl::Vec2 &_dim);

    /// \brief Centre camera on the active character
    void centreCamera();

private:
    ///
    /// \brief m_active whether scene is still active (if user has not quit)
    ///
    bool m_active;

    Preferences* m_prefs;

    Camera m_cam;
    ngl::Vec3 m_camCurPos;
    ngl::Vec3 m_camTargPos;
    float m_curFocalDepth;
    float m_targFocalDepth;

    Grid m_grid;
    Inventory m_world_inventory;

    /// @brief Vector of available names for characters
    std::vector<std::string> m_file_names;
    /// @brief Vector of character objects that have been created
    std::vector<Character> m_characters;

    Character *m_active_char;

    ngl::Transformation m_transform;

    /// @brief sets if mouse is being used to translate the scene
    bool m_mouse_trans_active;
    /// @brief sets if mouse is being used to rotate the scene
    bool m_mouse_rot_active;
    /// @brief sets if centering camera on character
    bool m_centre_camera;
    /// @brief target zoom of the camera
    float m_mouse_zoom_targ;
    /// @brief current zoom of the camera
    float m_mouse_zoom_cur;
    /// @brief current pan of the camera
    float m_mouse_pan;

    ngl::Vec2 m_mouse_translation;
    float m_mouse_rotation;
    /// @brief stores mouse position when m_mouse_trans_active is true
    ngl::Vec2 m_mouse_trans_origin;
    /// @brief stores mouse x position when m_mouse_rot_active is true
    float m_mouse_rot_origin;
    /// @brief stores mouse position when pressed down to check if mouse has moved at release
    ngl::Vec2 m_mouse_prev_pos;

    /// @brief stores textures and meshes
    AssetStore m_store;

    const float m_terrainHeightDivider = 16.0f;
    /// @brief terrain base mesh
    GLuint m_terrainVAO;
    /// @brief vert count of terrain mesh
    size_t m_terrainVAOSize;
    /// @brief Unit square VAO
    GLuint m_unitSquareVAO;

    /// @brief a screen quad, used mostly for postprocessing
    GLuint m_screenQuad;
    /// @brief framebuffer to store data for the deferred rendering pipeline
    Framebuffer m_mainBuffer;
    /// @brief framebuffer to store id and positional data, to be clicked on
    Framebuffer m_pickBuffer;
    /// @brief buffer to store depth for shadowing
    Framebuffer m_shadowBuffer;
    /// @brief buffer to store displacement map, currently for water only.
    Framebuffer m_displacementBuffer;
    /// @brief This buffer will store textures for post effects (DOF, reflections).
    Framebuffer m_postEffectsBuffer;

    /// @brief opens game_names.txt and stores the names in the vector m_file_names
    void readNameFile();
    /// @brief choses a random name and creates a character object with the name chosen
    void createCharacter();

    void loadMatricesToShader(const ngl::Mat4 _M, const ngl::Mat4 _MVP);
    void loadMatricesToShader();

    /// @brief checks the colour of the pixel where the user has clicked and sets states on objects
    void mouseSelection();

    void bindTextureToShader(const std::string &_shaderID, const GLuint _tex, const char *_uniform, int _target);
    void drawAsset(const std::string &_model, const std::string &_texture, const std::string &_shader);

    void createShader(const std::string _name, const std::string _vert, const std::string _frag, const std::string _geo = "", const std::string _tessctrl = "", const std::string _tesseval = "");
    GLuint createVAO(std::vector<ngl::Vec4> &_verts);
    GLuint createVAO(std::vector<ngl::Vec4> &_verts, std::vector<ngl::Vec2> &_uvs);
    GLuint createVAO(std::vector<ngl::Vec4> &_verts, std::vector<ngl::Vec3> &_normals, std::vector<ngl::Vec2> &_uvs);
    GLuint createBuffer4f(std::vector<ngl::Vec4> _vec);
    GLuint createBuffer3f(std::vector<ngl::Vec3> _vec);
    GLuint createBuffer2f(std::vector<ngl::Vec2> _vec);
    void setBufferLocation(GLuint _buffer, int _index, int _size);

    GLuint getTerrainPickTexture() {return m_pickBuffer.get("terrainpos");}
    GLuint getCharPickTexture() {return m_pickBuffer.get("charid");}

    std::pair<std::vector<bounds>, std::vector<bounds> > generateOrthoShadowMatrices(const std::vector<float> &_divisions);
    void shadowPass(bounds _worldbox, bounds _lightbox, size_t _index );

    ngl::Vec3 m_sunAngle;
    //The direction the sun is pointing (not unit vector TO the light).
    ngl::Vec3 m_sunDir;
    ngl::Vec3 m_directionalLightCol;
    //Tracks days since the start of the game.
    int m_day;
    /// @brief 0 = January 1st, 1 = December 31st.
    float m_season;

    std::vector<ngl::Mat4> m_shadowMat;

    /// @brief Tells me how big the screen is.
    ngl::Vec2 m_viewport;

    /// @brief Takes data from the grid and turns the ids into shapes like mountains.
    GLuint constructTerrain();

    struct terrainVertex
    {
        ngl::Vec4 m_pos;
        ngl::Vec3 m_norm;
    };

    struct terrainFace
    {
        //  2---3
        //  |   |
        //  0---1
        std::array< terrainVertex, 4 > m_verts;
    };

    terrainFace terrainVerticesToFace(const int _x,
                                      const int _y,
                                      const std::vector<std::vector<ngl::Vec3> > &_facePositions,
                                      const std::vector<std::vector<ngl::Vec3> > &_faceNormals);

    std::pair<float, ngl::Vec3> generateTerrainFaceData(const int _x,
                                                        const int _y,
                                                        const int _dirX,
                                                        const int _dirY,
                                                        const std::vector<std::vector<ngl::Vec3>> &_facePositions,
                                                        const std::vector<std::vector<ngl::Vec3>> &_faceNormals
                                                        );

    /// @brief Rather than looping through the grid every frame, and drawing based on tile id, I extract out the positions of the meshes that need
    /// to be drawn, and place them in this 2D vector, where the outer index matches their ID. Obviously, there is some wasted space here, I may
    /// improve the design at a later date.
    std::vector< std::vector<ngl::Vec3> > m_meshPositions;

    //Use these to draw debug points to the screen. Should be deleted/hidden at some point.
    GLuint m_debugVAO;
    GLuint m_debugVBO;
    std::vector<ngl::Vec4> m_debugPoints;
};

#endif//__SCENE_HPP__
