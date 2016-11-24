#include <iostream>
#include "Scene.hpp"

void Scene::update()
{
	std::cout<<"update\n";
  m_grid.newLayout("python/randomMap.py");
  m_grid.print();
}

void Scene::draw()
{
	std::cout<<"drawing\n";
}

bool Scene::isActive()
{
	return m_active;
}
