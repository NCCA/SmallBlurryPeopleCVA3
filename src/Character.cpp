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
	m_speed(0.1),
	m_wood_inventory(0)
{

	//timer for actions
	m_timer.start();

	//create random chopping and building speed
	ngl::Random *rand = ngl::Random::instance();
	rand->setSeed(0);

	std::random_device rnd;
	std::mt19937 mt_rand(rnd());
	std::uniform_int_distribution<int> chopping_speed(1,5);
	m_chopping_speed = chopping_speed(mt_rand);

	std::uniform_int_distribution<int> building_speed(5,10);
	m_building_speed = building_speed(mt_rand);

  m_pos = ngl::Vec2(0,0);
	m_target_id = m_grid->coordToId(m_pos);


	//creates state stack, will be called when selection made
	ngl::Vec2 target = {19,25};
		//check if chosen grid tile is current position tile
	if (m_grid->coordToId(target) != m_grid->coordToId(m_pos))
	{
		setTarget(ngl::Vec2(19,25));
		setState();
	}
	else
		setActive(false);


  std::cout << "character created" << std::endl;
  std::cout << "character target: " << m_target_id << std::endl;
}

void Character::setState()
{
	GridTile target = m_grid->get(m_target_id);

	if (target.isTrees() == true)
		{
			m_state_stack.push_back(State::MOVE);
			m_state_stack.push_back(State::GET_WOOD);
			m_state_stack.push_back(State::MOVE);
			std::cout<<"TREE"<<std::endl;
		}
	//WILL NEED OPTION BETWEEN BUILDING AND MOVING
	else if (target.isBuilding() == true)
		{
		//SHOULD COLLECT THINGS FROM STORE HOUSE?
			m_state_stack.push_back(State::MOVE);
			m_state_stack.push_back(State::BUILD);
			std::cout<<"BUILDING"<<std::endl;
		}
	else
		{
			m_state_stack.push_back(State::MOVE);
			std::cout<<"MOVE"<<std::endl;
		}
}

void Character::update()
{
	//check if states are still in stack
	if(m_state_stack.size() > 0)
	{
		//check bottom state of stack
		State current_state = m_state_stack[0];
		switch(current_state)
		{
				case(State::MOVE):
				{
					std::cout<<"character pos"<<m_pos<<std::endl;
					if (move())
					{
						std::cout<<"TARGET HAS BEEN REACHED"<<std::endl;
						GridTile target = m_grid->get(m_target_id);
						if (target.getBuildingType() == BuildingType::NONE && m_wood_inventory > 0)
						{
							m_wood_inventory-=1;
							std::cout<<"WOOD DEPOSITED"<<std::endl;
						}
						//when the target has been reached, remove the state from the stack
						m_state_stack.pop_front();
						m_timer.restart();
						std::cout<<m_timer.elapsed()<<std::endl;
					}
					break;
				}
				case(State::GET_WOOD):
				{
					std::cout<<m_timer.elapsed()<<std::endl;
					//when chopping speed has been reached, gain a piece of wood
					if(m_timer.elapsed() >= 1000 * m_chopping_speed)
					{
						m_wood_inventory += 1;
						std::cout<<"GOT WOOD"<<std::endl;
						//when recieved piece of wood, remove the state from the stack
						m_state_stack.pop_front();
						m_timer.restart();

						//finds nearest storage house and sets as target for storing wood
						if(!findNearestStorage())
							//remove MOVE state from stack as well, as can't find/move to storage house
							m_state_stack.clear();
					}
					break;
				}
				case(State::BUILD):
				{
					std::cout<<m_timer.elapsed()<<std::endl;
					//CHECK FOR CHARACTER INVENTORY OR STORE HOUSE?
					if(m_timer.elapsed() >= 1000 * m_building_speed)
					{
						std::cout<<"BUILT"<<std::endl;
						m_state_stack.pop_back();
						m_timer.restart();
					}
					break;
				}
		}
	}
	else
	{
		setActive(false);
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

void Character::draw()
{
	std::cout << "character position: " << m_pos << std::endl;
	std::cout << "character drawn" << std::endl;
}

std::vector<ngl::Vec2> Character::findPath(int _target_id)
{
	NodeNetwork network(m_grid, m_pos, m_grid->idToCoord(_target_id));

	return network.findPath();

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
		m_path = findPath(m_target_id);

	}
}

bool Character::findNearestStorage()
{
	std::vector<ngl::Vec2> storage_houses;
	int height = m_grid->getH();
	int width = m_grid->getW();

	for (int i=0; i< height; i++)
	{
		for (int j=0; j<width; j++)
		{
			ngl::Vec2 coord = {j, i};
			GridTile current_tile = m_grid->get(coord);
			if (current_tile.isBuilding())
			{
				BuildingType current_building = current_tile.getBuildingType();
				//CHANGE TYPE WHEN FOUND
				if (current_building == BuildingType::NONE)
				{
					storage_houses.push_back(coord);
				}
			}
		}
	}

	if (storage_houses.size() > 0)
	{
		m_target_id = m_grid->coordToId(storage_houses[0]);
		std::vector<ngl::Vec2> shortest_path = findPath(m_target_id);
		storage_houses.erase(storage_houses.begin());

		if(storage_houses.size() > 0)
		{
			for (auto &storage : storage_houses)
			{
				int id = m_grid->coordToId(storage);
				std::vector<ngl::Vec2> path = findPath(id);
				if (path.size() < shortest_path.size())
				{
					m_target_id = id;
					shortest_path = path;
				}
			}
		}
		m_path = shortest_path;
		return true;

	}
	else
	{
		std::cout<<"NO STORAGE HOUSE :((((((((((((((("<<std::endl;
		return false;
	}
}
