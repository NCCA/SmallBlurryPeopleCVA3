#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include<memory>

#include <ngl/Transformation.h>

#include "AssetStore.hpp"
#include "Grid.hpp"
#include "Camera.hpp"
#include "Character.hpp"

#include "Framebuffer.hpp"

typedef std::pair<ngl::Vec3, ngl::Vec3> bounds;

class Scene
{
public:
    Scene(ngl::Vec2 _viewport);
    ~Scene() = default;
    void draw();
    void update();

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

private:

    Camera m_cam;
    Grid m_grid;

    /// @brief Vector of available names for characters
    std::vector<std::string> m_file_names;
    /// @brief Vector of character objects that have been created
    std::vector<Character> m_characters;

    ngl::Transformation m_transform;

    /// @brief sets if mouse is being used to translate the scene
    bool m_mouse_trans_active;
    /// @brief sets if mouse is being used to rotate the scene
    bool m_mouse_rot_active;
    /// @brief current zoom of the camera
    float m_mouse_zoom;
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

    AssetStore m_store;
    GLuint m_terrainVAO;
    size_t m_terrainVAOSize;
    GLuint m_screenQuad;
    Framebuffer m_mainBuffer;
    Framebuffer m_pickBuffer;
    Framebuffer m_shadowBuffer;

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

    void createShader(const std::string _name, const std::string _vert, const std::string _frag);
    GLuint createVAO(std::vector<ngl::Vec4> &_verts);
    GLuint createVAO(std::vector<ngl::Vec4> &_verts, std::vector<ngl::Vec2> &_uvs);
    GLuint createBuffer4f(std::vector<ngl::Vec4> _vec);
    GLuint createBuffer3f(std::vector<ngl::Vec3> _vec);
    GLuint createBuffer2f(std::vector<ngl::Vec2> _vec);
    void setBufferLocation(GLuint _buffer, int _index, int _size);

    GLuint getTerrainPickTexture() {return m_pickBuffer.get("terrainpos");}
    GLuint getCharPickTexture() {return m_pickBuffer.get("charid");}

    std::vector< bounds > generateOrthoShadowMatrices(const std::vector<float> &_divisions);
    void shadowPass( bounds _box, size_t _index );

    ngl::Vec3 m_sunAngle;
    ngl::Vec3 m_sunDir;

    std::vector<ngl::Mat4> m_shadowMat;

    /// @brief Tells me how big the screen is.
    ngl::Vec2 m_viewport;
};

#endif//__SCENE_HPP__
