#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "Grid.hpp"
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
    Grid m_grid;
    Character m_character;
};

#endif//__SCENE_HPP__
