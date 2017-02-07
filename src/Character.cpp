#include "Character.hpp"

#include "Node.hpp"
#include "NodeNetwork.hpp"

#include "ngl/Random.h"
#include "ngl/NGLStream.h"

#include <iostream>
#include <vector>
#include <array>
#include <memory>

int Character::m_id_counter(0);

Character::Character(Grid *_grid, std::string _name):
	m_id(m_id_counter++),
	m_name(_name),
	m_active(true),
	m_grid(_grid),
	m_speed(1)
{
	ngl::Random *rand = ngl::Random::instance();
	rand->setSeed(0);

	std::random_device rnd;
	std::mt19937 mt_rand(rnd());
	std::uniform_int_distribution<int> chopping_speed(1,5);
	m_chopping_speed = chopping_speed(mt_rand);


  //m_pos = ngl::Vec2(1, 4);
  m_pos = ngl::Vec2(0,0);
	m_target_id = m_grid->coordToId(m_pos);

  //setTarget(ngl::Vec2(9, 1));
	setTarget(ngl::Vec2(19,25));

	setState();


  std::cout << "character created" << std::endl;
  std::cout << "character target: " << m_target_id << std::endl;
 // m_grid->printTrees();
}

void Character::setState()
{
	if(m_active == true)
	{
		GridTile target = m_grid->get(m_target_id);

		if (target.isTrees() == true)
			{
				m_state = State::GET_WOOD;
				std::cout<<"TREE"<<std::endl;
			}
		else if (target.isBuilding() == true)
			{
				m_state = State::BUILD;
				std::cout<<"BUILDING"<<std::endl;
			}
		else
			{
				m_state = State::MOVE;
				std::cout<<"MOVE"<<std::endl;
			}
	}
	else m_state = State::NONE;

}

void Character::update()
{
	switch(m_state)
	{
		case(State::MOVE):
		{
			bool finished = move();
			std::cout<<"character pos"<<m_pos<<std::endl;
			if (finished == true)
			{
				setActive(false);
				std::cout<<"TARGET HAS BEEN REACHED"<<std::endl;
			}
			break;
		}
		case(State::GET_WOOD):
		{
			bool finished = move();
			std::cout<<"character pos"<<m_pos<<std::endl;
			if (finished == true)
			{
				SDL_AddTimer(m_chopping_speed * 10000, &Character::getWood, NULL);
				m_active = false;
			}
			break;
		}
		case(State::NONE):
		{
			break;
		}
	}
}

bool Character::move()
{
  // move by distance up to speed
  float dist_moved = 0;
  while(m_path.size() > 0 && dist_moved < m_speed)
  {
    float dist = 1;
    ngl::Vec2 aim_vec = calcAimVec(&dist);
    if(dist < m_speed)
    {
      aim_vec *= dist;
      m_path.pop_back();
    }
    else
    {
      aim_vec *= m_speed;
    }
    dist_moved += aim_vec.length();
    m_pos += aim_vec;
  }
	if(m_path.size() <= 0)
	{
		return true;
	}
	else return false;
}

unsigned int Character::getWood(unsigned int interval, void *param)
{
	//ADD INVENTORY STUFF
	std::cout<<"GOT WOOD"<<std::endl;
	return 0;
}
void Character::draw()
{
	std::cout << "character position: " << m_pos << std::endl;
	std::cout << "character drawn" << std::endl;
}

void Character::findPath()
{
	NodeNetwork network(m_grid, m_pos, m_grid->idToCoord(m_target_id));

	m_path = network.findPath();
	if (m_path.size() <= 0)
		setActive(false);

}

ngl::Vec2 Character::calcAimVec(float *dist)
{
	ngl::Vec2 aim_vec(0,0);
	if(m_path.size() > 0)
	{
	 aim_vec = m_path.back() - m_pos;
	}
	//std::cout << aim_vec << std::endl;
	if(dist)
	{
		*dist = aim_vec.length();
	}
	if(aim_vec != ngl::Vec2(0,0))
	{
		aim_vec.normalize();
	}
	return aim_vec;
}

void Character::setTarget(ngl::Vec2 _target_pos)
{
	// convert to tile id
	setTarget(m_grid->coordToId(_target_pos));
}

void Character::setTarget(int _tile_id)
{
	if(_tile_id != m_target_id)
	{
		m_target_id = _tile_id;
		findPath();
	}
	else setActive(false);
}
