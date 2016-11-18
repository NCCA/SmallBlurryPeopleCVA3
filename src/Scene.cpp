#include <iostream>
#include "Scene.hpp"

Scene::Scene() :
  m_character(&m_grid)
{

}

void Scene::update()
{
	std::cout<<"update\n";
}

void Scene::draw()
{
	std::cout<<"drawing\n";
}

bool Scene::isActive()
{
	return m_active;
}
