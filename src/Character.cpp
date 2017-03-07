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

Character::Character(Grid *_grid, Inventory *_world_inventory, std::string _name):
	m_id(m_id_counter++),
	m_name(_name),
	m_active(true),
	m_grid(_grid),
	m_world_inventory(_world_inventory),
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
	std::uniform_int_distribution<int> chopping_speed(5,10);
	m_chopping_speed = chopping_speed(mt_rand);

	std::uniform_int_distribution<int> building_speed(10,15);
	m_building_speed = building_speed(mt_rand);

	m_pos = ngl::Vec2(19,25);
	m_target_id = m_grid->coordToId(m_pos);

	//check if chosen grid tile is current position tile


}

void Character::setState()
{
	GridTile target = m_grid->get(m_target_id);

  if (target.getType() == TileType::TREES)
		{
			if(findNearestEmptyTile() == true)
			{
				m_state_stack.push_back(State::MOVE);
				m_state_stack.push_back(State::GET_WOOD);
				m_state_stack.push_back(State::MOVE);
				m_state_stack.push_back(State::STORE_WOOD);
				m_state_stack.push_back(State::MOVE);
				std::cout<<"TREE"<<std::endl;
			}
			else
			{
				std::cout<<"NO TREE :("<<std::endl;
			}
		}
	//WILL NEED OPTION BETWEEN BUILDING AND MOVING
  else if (target.getType() == TileType::STOREHOUSE)
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
					//std::cout<<"character pos"<<m_pos<<std::endl;
					if (move())
					{
						std::cout<<"TARGET HAS BEEN REACHED"<<std::endl;
						//when the target has been reached, remove the state from the stack
						m_state_stack.pop_front();
						m_timer.restart();
					}
					break;
				}
				case(State::GET_WOOD):
				{
					//when chopping speed has been reached, gain a piece of wood
					if(m_timer.elapsed() >= 1000 * m_chopping_speed)
					{
						m_wood_inventory += 1;
						std::cout<<m_timer.elapsed()<<std::endl;
						std::cout<<"GOT WOOD"<<std::endl;
						//when recieved piece of wood, remove the state from the stack
						m_state_stack.pop_front();
						m_timer.restart();

						//finds nearest storage house and sets as target for storing wood
						if(!findNearestStorage())
							//remove states from stack as well, as can't find/move to storage house
							m_state_stack.clear();
					}
					break;
				}
				case(State::STORE_WOOD):
				{
					if(m_timer.elapsed() >= 1000)
					{
						m_world_inventory->addWood(1);
						m_wood_inventory -=1;
						m_state_stack.pop_front();
						m_timer.restart();
						std::cout<<"WOOD DEPOSITED"<<std::endl;
						std::cout<<m_name<<"'s wood inventory: "<<m_wood_inventory<<std::endl;
						std::cout<<"storage inventory: "<<m_world_inventory->getWoodInventory()<<std::endl;

						//if made it to storage house, should be able to find empty tile
						findNearestEmptyTile();

						m_timer.restart();
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
		//when stack cleared, character deactivated
		//setActive(false);
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
	//if (m_grid->coordToId(current_target) != m_grid->coordToId(m_pos))
	//	setTarget(current_target);
	//else
	//	setActive(false);
	setTarget(m_grid->coordToId(_target_pos));

}

void Character::setTarget(int _tile_id)
{
	//if the chosen tile isnt equal to the target and isnt equal to the character's pos
	if(_tile_id != m_target_id && _tile_id != m_grid->coordToId(m_pos))
	{
		std::cout<<"STATE"<<std::endl;
		m_target_id = _tile_id;
		m_path = findPath(m_target_id);
		//if no path was found, deactivate character
		if(m_path.size() <= 0)
			setActive(false);
	}
	else
		//deactivate character
		setActive(false);

	if(isActive())
		setState();

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
			//go through grid and find storage houses
			ngl::Vec2 coord = {j, i};
			GridTile current_tile = m_grid->get(coord);
      if (current_tile.getType() == TileType::STOREHOUSE)
      {
        storage_houses.push_back(coord);
			}
		}
	}
	return findNearest(storage_houses);
}

bool Character::findNearestEmptyTile()
{
	std::vector<ngl::Vec2> neighbours;
	ngl::Vec2 target = m_grid->idToCoord(m_target_id);

	ngl::Vec2 right = {target[0]+1, target[1]};
	GridTile neighbour = m_grid->get(right);
	if(neighbour.getType() == TileType::NONE)
		neighbours.push_back(right);

	ngl::Vec2 left = {target[0]-1, target[1]};
	neighbour = m_grid->get(left);
	if(neighbour.getType() == TileType::NONE)
		neighbours.push_back(left);

	ngl::Vec2 up = {target[0], target[1]+1};
	neighbour = m_grid->get(up);
	if(neighbour.getType() == TileType::NONE)
		neighbours.push_back(up);

	ngl::Vec2 down = {target[0], target[1]-1};
	neighbour = m_grid->get(down);
	if(neighbour.getType() == TileType::NONE)
		neighbours.push_back(down);

	return findNearest(neighbours);

}


bool Character::findNearest(std::vector<ngl::Vec2> _coord_data)
{
	if (_coord_data.size() > 0)
	{
		m_target_id = m_grid->coordToId(_coord_data[0]);
		std::vector<ngl::Vec2> shortest_path = findPath(m_target_id);
		_coord_data.erase(_coord_data.begin());

		//find shortest distance to storage house, storage house becomes target
		if(_coord_data.size() > 0)
		{
			for (auto coord : _coord_data)
			{
				int id = m_grid->coordToId(coord);
				std::vector<ngl::Vec2> path = findPath(id);
				if (shortest_path.size() == 0 && path.size() > shortest_path.size())
				{
					m_target_id = id;
					shortest_path = path;
				}
				else if (path.size() < shortest_path.size() && path.size()!= 0)
				{
					m_target_id = id;
					std::cout<<"TARGET_COORD_"<<m_grid->idToCoord(m_target_id)<<std::endl;
					shortest_path = path;
				}
			}
		}
		m_path = shortest_path;
		return true;
	}
	else
	{
		std::cout<<"NO FOUNDY :((((((((((((((("<<std::endl;
		return false;
	}
}

void Character::build(BuildingType _building)
{
	//Start building the building type at current position
	std::cout << "building YAY" << std::endl;
}
