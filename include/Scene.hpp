#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "Grid.hpp"

class Scene
{
	public:
		Scene() = default;
		~Scene() = default;
		bool isActive();
		void update();
		void draw();
	private:
		bool m_active = true;
    Grid m_grid;

};

#endif//__SCENE_HPP__
