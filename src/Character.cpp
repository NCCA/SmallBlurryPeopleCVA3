#include "Character.hpp"

#include "Node.hpp"
#include "NodeNetwork.hpp"
#include "Preferences.hpp"

#include "Utility.hpp"

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
	m_speed(0.05),
	m_wood_inventory(0),
	m_fish_inventory(0),
	m_called(0)
{

	Preferences* p = Preferences::instance();
	m_speed = p->getCharacterSpeed();
	//timer for actions
	m_timer.start();

	//create random times for abilities
	//wood chopping speed
	m_chopping_speed = Utility::randInt(5, 10);

	//building houses speed
	m_building_speed = Utility::randInt(10,15);

	//fishing speed = m_fishing_catch+3
	//probability of catching a fish
	m_fishing_catch = Utility::randInt(1,5);

  m_pos = ngl::Vec2(19,25);
  m_target_id = m_grid->coordToId(m_pos);
}

void Character::setState()
{
	m_state_stack.clear();
	//reset speed
	Preferences* p = Preferences::instance();
	m_speed = p->getCharacterSpeed();

	m_called = 0;

	GridTile target = m_grid->get(m_target_id);

  if (target.getType() == TileType::TREES)
	{
		if(findNearestEmptyTile() == true)
		{
			m_state_stack.push_back(State::MOVE);
			m_state_stack.push_back(State::CHOP_WOOD);
			m_state_stack.push_back(State::MOVE);
			m_state_stack.push_back(State::STORE_WOOD);
			m_state_stack.push_back(State::MOVE);
			m_state_stack.push_back(State::WAIT);
			std::cout<<"TREE"<<std::endl;
		}
	}
	else if (target.getType() == TileType::WATER)
	{
		m_state_stack.push_back(State::MOVE);
		m_state_stack.push_back(State::FISH);
		m_state_stack.push_back(State::MOVE);
		m_state_stack.push_back(State::STORE_FISH);
		m_state_stack.push_back(State::MOVE);
		m_state_stack.push_back(State::WAIT);
		std::cout<<"WATER"<<std::endl;
	}
	//WILL NEED OPTION BETWEEN BUILDING AND MOVING
	else if (target.getType() == TileType::STOREHOUSE)
	{
		m_final_target_id = m_target_id;
		m_state_stack.push_back(State::COLLECT);
		m_state_stack.push_back(State::MOVE);
		m_state_stack.push_back(State::GET_WOOD);
		m_state_stack.push_back(State::MOVE);
		m_state_stack.push_back(State::BUILD);
		m_state_stack.push_back(State::WAIT);
		std::cout<<"BUILDING"<<std::endl;
	}
	else if (target.getType() == TileType::NONE)
	{
		m_state_stack.push_back(State::MOVE);
		m_state_stack.push_back(State::WAIT);
		std::cout<<"MOVE"<<std::endl;
	}
}

void Character::setIdleState()
{
	if (m_called == 0)
	{
		m_idle_target_id = m_target_id;
		m_speed -= 0.07;
	}

	ngl::Vec2 idle_pos = m_grid->idToCoord(m_idle_target_id);
	int dist = 5;

	bool valid = false;
	while(!valid)
	{
		int x_min_range = Utility::clamp((idle_pos.m_x - dist), 0, 50);
		int x_max_range = Utility::clamp((idle_pos.m_x + dist), 0, 50);
		int y_min_range = Utility::clamp((idle_pos.m_y - dist), 0, 50);
		int y_max_range = Utility::clamp((idle_pos.m_y + dist), 0, 50);

		int x = Utility::randInt(x_min_range, x_max_range);
		int y = Utility::randInt(y_min_range, y_max_range);
		valid = setTarget(ngl::Vec2(x,y));
	}

	m_state_stack.push_back(State::MOVE);
	m_called++;
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
				if (move())
				{
					//when the target has been reached, remove the state from the stack
					m_state_stack.pop_front();
					m_timer.restart();
				}
				break;
			}

			case(State::CHOP_WOOD):
			{
				//when chopping speed has been reached, gain a piece of wood
				if(m_timer.elapsed() >= 1000 * m_chopping_speed)
				{
					m_wood_inventory += 1;
					std::cout<<"GOT WOOD"<<std::endl;

					///change type from TREE to NONE
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

			case(State::FISH):
			{
				//when fishing speed reached, gain piece of fish
				if(m_timer.elapsed() >= 1000 * m_fishing_catch + 3)
				{
					ngl::Random *rand = ngl::Random::instance();
					if(int(rand->randomPositiveNumber(10)) % m_fishing_catch == 0)
					{
						m_fish_inventory += 1;
						std::cout<<"GOT FISH"<<std::endl;
						//when recieved fish, remove state from stack
						m_state_stack.pop_front();
						m_timer.restart();
						//find nearest storage house and sets as target
						if(!findNearestStorage())
							//remove states from stack, can't find/move to storage house
							m_state_stack.clear();
					}
					else
					{
						std::cout<<"FISH NOT CAUGHT"<<std::endl;
						m_state_stack.clear();
					}
				}
				break;
			}

			case(State::STORE_FISH):
			{
				if(m_timer.elapsed() >= 1000)
				{
					m_world_inventory->addFish(1);
					m_fish_inventory -=1;
					m_state_stack.pop_front();
					m_timer.restart();

					std::cout<<"FISH DEPOSITED"<<std::endl;
					std::cout<<m_name<<"'s fish inventory: "<<m_fish_inventory<<std::endl;
					std::cout<<"storage inventory: "<<m_world_inventory->getFishInventory()<<std::endl;

					//if made it to storage house, should be able to find empty tile
					findNearestEmptyTile();
					m_timer.restart();
				}
				break;
			}

		case(State::GET_WOOD):
		{
			if(m_world_inventory->getWoodInventory() >= 1)
			{
				if(m_timer.elapsed() >= 1000)
				{
					std::cout<<"COLLECTED WOOD"<<std::endl;
					m_wood_inventory += 1;
					m_world_inventory->takeWood(1);
					m_state_stack.pop_front();
					setTarget(m_final_target_id);
					m_timer.restart();
				}
			}
			else
			{
				std::cout<<"NOT ENOUGH WOOD"<<std::endl;
				m_state_stack.clear();
			}
			break;
		}

		case(State::COLLECT):
		{
			if(!findNearestStorage())
			{
				std::cout<<"NO NEARBY STORAGE"<<std::endl;
				m_state_stack.clear();
			}
			else
			{
				m_state_stack.pop_front();
				m_timer.restart();
			}
			break;
		}

			case(State::BUILD):
			{
				if(m_timer.elapsed() >= 1000 * m_building_speed)
				{
					m_wood_inventory -= 1;
					///Change grid tile from NONE type to HOUSE type
					std::cout<<"BUILT HOUSE"<<std::endl;
					m_state_stack.pop_front();
					m_timer.restart();
				}
				break;
			}

			case(State::WAIT):
			{
				if(m_timer.elapsed() >= 3000)
				{
					m_state_stack.pop_front();
					m_timer.restart();
				}
				break;
			}

		}
	}
	else
	{
		//setIdleState();
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

bool Character::setTarget(ngl::Vec2 _target_pos)
{
	// convert to tile id
	return setTarget(m_grid->coordToId(_target_pos));
}

bool Character::setTarget(int _tile_id)
{
	m_pos.m_x = int(m_pos.m_x);
	m_pos.m_y = int(m_pos.m_y);
	//if the chosen tile isnt equal to the target and isnt equal to the character's pos
	if(_tile_id != m_target_id && _tile_id != m_grid->coordToId(m_pos))
	{
		m_target_id = _tile_id;
		m_path = findPath(m_target_id);
		//if no path was found, deactivate character
		if(m_path.size() <= 0)
		{
			//setActive(false);
			return false;
		}
		else
			return true;
	}
	else
	{
		//deactivate character
		//setActive(false);
		return false;
	}
}

bool Character::findNearestStorage()
{
	//empty vector to hold positions of storage house
  std::vector<ngl::Vec2> storage_houses;
  int height = m_grid->getH();
  int width = m_grid->getW();

	//go thorugh grid
  for (int i=0; i< height; i++)
  {
    for (int j=0; j<width; j++)
    {
      ngl::Vec2 coord = {j, i};
      GridTile current_tile = m_grid->get(coord);
			//if the current grid tile is a storehouse, add to vector
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
	//checks neighbouring tiles to see if they are empty,
	//if empty add to vector neighbours
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
	//if the vector is not empty
  if (_coord_data.size() > 0)
  {
		//set the first element to the target id and shortest path
    m_target_id = m_grid->coordToId(_coord_data[0]);
    std::vector<ngl::Vec2> shortest_path = findPath(m_target_id);
		//erase the first element in the vector
    _coord_data.erase(_coord_data.begin());

		//if the vector is not empty (has more than one element)
    if(_coord_data.size() > 0)
    {
      for (auto coord : _coord_data)
      {
				//compare the current elemets path length to the shortest_path's length
        int id = m_grid->coordToId(coord);
        std::vector<ngl::Vec2> path = findPath(id);
				//if shorter path, set as target
        if (shortest_path.size() == 0 && path.size() > shortest_path.size())
        {
          m_target_id = id;
          shortest_path = path;
        }
        else if (path.size() < shortest_path.size() && path.size()!= 0)
        {
          m_target_id = id;
          shortest_path = path;
        }
      }
    }
		//set character path to the shortest path found
    m_path = shortest_path;
    return true;
  }
  else
  {
		//no path was found
    std::cout<<"NO FOUNDY :((((((((((((((("<<std::endl;
    return false;
  }
}

void Character::build(BuildingType _building)
{
  //Start building the building type at current position
  std::cout << "building YAY" << std::endl;
}

ngl::Vec3 Character::getPos()
{
	//get grid height at character's position
  float height = m_grid->getInterpolatedHeight(m_pos[0], m_pos[1]);
  return ngl::Vec3(m_pos[0], height, m_pos[1]);
}
