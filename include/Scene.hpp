#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include<memory>

#include <ngl/Transformation.h>

#include "AssetStore.hpp"
#include "Grid.hpp"
#include "Camera.hpp"
#include "Character.hpp"

#include "Framebuffer.hpp"

class Scene
{
public:
    Scene();
    ~Scene() = default;
    void draw();
    void update();

    void mousePressEvent(const SDL_MouseButtonEvent &_event);
    void mouseReleaseEvent(const SDL_MouseButtonEvent &_event);
    void wheelEvent(const SDL_MouseWheelEvent &_event);

private:
    bool m_active = true;
    Camera m_cam;
    Grid m_grid;
		std::vector<std::string> m_file_names;
    std::vector<Character> m_characters;

    ngl::Transformation m_transform;

    bool m_mouse_trans_active;
    bool m_mouse_rot_active;
    float m_mouse_zoom;
    float m_mouse_pan;

    ngl::Vec2 m_mouse_translation;
    float m_mouse_rotation;
    ngl::Vec2 m_mouse_trans_origin;
    float m_mouse_rot_origin;

    AssetStore m_store;
    GLuint m_terrainVAO;
    size_t m_terrainVAOSize;
    GLuint m_screenQuad;
    Framebuffer m_mainBuffer;

		void readNameFile();
		void createCharacter();

		void loadMatricesToShader();

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
};

#endif//__SCENE_HPP__
