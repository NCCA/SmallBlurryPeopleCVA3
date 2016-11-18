#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "Grid.hpp"
#include "Camera.hpp"
#include "Character.hpp"

class Scene
{
public:
    Scene();
    ~Scene() = default;
    bool isActive();
    void update();
    void draw();
private:
    bool m_active = true;
    Camera m_cam;
    Grid m_grid;
    Character m_character;
    float m_temp_counter_delete = 0.0f;
};

#endif//__SCENE_HPP__
