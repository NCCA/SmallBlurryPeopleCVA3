#include "Character.hpp"

#include "Node.hpp"
#include "NodeNetwork.hpp"
#include "Prefs.hpp"

#include "Utility.hpp"

#include "ngl/Random.h"
#include "ngl/NGLStream.h"

#include <iostream>
#include <memory>

int Character::m_id_counter(1);

Character::Character(Grid *_grid, Inventory *_world_inventory, std::string _name):
  m_id(m_id_counter++),
  m_name(_name),
  m_active(false),
  m_grid(_grid),
  m_world_inventory(_world_inventory),
  m_speed(0),
	m_inventory(CharInventory::NONE),
	m_called(0),
	m_building_amount(0),
	m_building_type(TileType::NONE)
{
  Prefs* prefs = Prefs::instance();
  m_speed = prefs->getFloatPref("CHARACTER_SPEED");
	m_building_amount = prefs->getFloatPref("CHARACTER_BUILDING");
  //timer for actions
  m_timer.start();

  //create random times for abilities
  //wood chopping speed
  m_chopping_speed = Utility::randInt(5, 10);

  //building houses speed
	m_building_speed = Utility::randInt(5,10);

  //probability of catching a fish
  m_fishing_catch = Utility::randInt(1,3);
	m_fishing_speed = Utility::randInt(m_fishing_catch,5);

	m_forage_amount = Utility::randInt(5,10);

	float red = Utility::randFlt(0, 1);
	float blue = Utility::randFlt(0,1);
	float green = Utility::randFlt(0,1);

  m_colour = ngl::Vec3(red, blue, green);

  bool valid = false;
  float x,y = 0;

  while(!valid)
  {
    x = Utility::randInt(0, m_grid->getW());
    y = Utility::randInt(0, m_grid->getH());

    if (m_grid->getTileType(x, y) == TileType::NONE)
      valid = true;
  }
  m_pos = ngl::Vec2(x, y);
  m_target_id = m_grid->getTileId(x, y);
}

void Character::setState()
{
  m_state_stack.clear();
  //reset speed
  Prefs* prefs = Prefs::instance();
  m_speed = prefs->getFloatPref("CHARACTER_SPEED");

  m_called = 0;

  if (m_grid->getTileType(m_target_id) == TileType::TREES)
		chopState();
  else if (m_grid->getTileType(m_target_id) == TileType::WATER)
		fishState();
  else if (m_grid->getTileType(m_target_id) == TileType::NONE)
		buildState(TileType::HOUSE);
}

void Character::buildState(TileType _building)
{
	//Start building the building type at current position
	if(findNearestEmptyTile())
	{
		m_final_target_id = m_target_id;
		m_building_type = _building;
		for(int i =0; i<(int)m_building_amount; i++)
		{
			m_state_stack.push_back(State::COLLECT);
			m_state_stack.push_back(State::MOVE);
			m_state_stack.push_back(State::GET_WOOD);
			m_state_stack.push_back(State::MOVE);
			m_state_stack.push_back(State::BUILD);
		}
		m_state_stack.push_back(State::IDLE);
		std::cout<<"BUILD"<<std::endl;
	}
}

void Character::moveState()
{
	m_state_stack.push_back(State::MOVE);
	m_state_stack.push_back(State::IDLE);
	std::cout<<"MOVE"<<std::endl;
}

void Character::chopState()
{
	m_final_target_id = m_target_id;
	if(findNearestEmptyTile())
	{
		int wood_amount = 9;
		std::cout<<"TARGET 1: "<<m_target_id;
		for(int i = 0; i< wood_amount; i++)
		{
			m_state_stack.push_back(State::MOVE);
			m_state_stack.push_back(State::CHOP_WOOD);
			m_state_stack.push_back(State::MOVE);
			m_state_stack.push_back(State::STORE);
			m_state_stack.push_back(State::MOVE);
			if(i != (wood_amount - 1))
				m_state_stack.push_back(State::REPEAT);
		}
		std::cout<<"TREE"<<std::endl;
		m_state_stack.push_back(State::IDLE);
	}
}

void Character::fishState()
{
	if(findNearestFishingTile())
	{
		m_state_stack.push_back(State::MOVE);
		m_state_stack.push_back(State::FISH);
		m_state_stack.push_back(State::MOVE);
		m_state_stack.push_back(State::STORE);
		m_state_stack.push_back(State::MOVE);
		m_state_stack.push_back(State::IDLE);
	}
	std::cout<<"WATER"<<std::endl;
}

void Character::forageState()
{
	int berry_amount = m_forage_amount + Utility::randInt(-2, 2);
	for(int i=0; i<berry_amount; i++)
	{
		m_state_stack.push_back(State::MOVE);
		m_state_stack.push_back(State::FORAGE);
		m_state_stack.push_back(State::MOVE);
		m_state_stack.push_back(State::STORE);
		m_state_stack.push_back(State::MOVE);
		if (i != (berry_amount -1))
			m_state_stack.push_back(State::REPEAT);
	}
	std::cout<<"FORAGE"<<std::endl;
	m_state_stack.push_back(State::IDLE);
}

void Character::setIdleState()
{
  if (m_called == 0)
  {
    m_idle_target_id = m_target_id;
    m_speed /= 10;
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
					int wood_taken = m_grid->cutTileTrees(m_final_target_id, 1);
					if (wood_taken == 1)
					{
						m_inventory = CharInventory::WOOD;
						std::cout<<m_name<<" got a piece of wood!"<<std::endl;

						//when recieved piece of wood, remove the state from the stack
						m_state_stack.pop_front();
						m_timer.restart();

						//finds nearest storage house and sets as target for storing wood
						if(!findNearestStorage())
							//remove states from stack as well, as can't find/move to storage house
							m_state_stack.clear();
					}
					else
					{
						m_state_stack.clear();
						std::cout<<"no more wood availble"<<std::endl;
					}
        }
        break;
      }

			case(State::STORE):
      {
        if(m_timer.elapsed() >= 1000)
        {
					if (m_inventory == CharInventory::WOOD)
					{
						m_world_inventory->addWood(1);
						std::cout<<m_name<<" deposited wood"<<std::endl;
					}
					else if (m_inventory == CharInventory::FISH)
					{
						m_world_inventory->addFish(1);
						std::cout<<m_name<<" deposited fish"<<std::endl;
					}
					else if (m_inventory == CharInventory::BERRY)
					{
						m_world_inventory->addBerries(1);
						std::cout<<m_name<<" deposited berries"<<std::endl;
					}

					m_inventory = CharInventory::NONE;
          m_state_stack.pop_front();
          m_timer.restart();

					std::cout<<"storage wood inventory: "<<m_world_inventory->getWoodInventory()<<std::endl;
					std::cout<<"storage fish inventory: "<<m_world_inventory->getFishInventory()<<std::endl;
					std::cout<<"storage berry inventory: "<<m_world_inventory->getBerryInventory()<<std::endl;

          //if made it to storage house, should be able to find empty tile
          findNearestEmptyTile();
          m_timer.restart();
        }
        break;
      }

      case(State::FISH):
      {
        //when fishing speed reached, gain piece of fish
				if(m_timer.elapsed() >= 1000 * m_fishing_speed)
        {
          ngl::Random *rand = ngl::Random::instance();
          int fish_rand = rand->randomPositiveNumber(10);
          if(fish_rand % m_fishing_catch == 0)
          {
						m_inventory=CharInventory::FISH;
            std::cout<<m_name<<" got a fish!"<<std::endl;
            //when recieved fish, remove state from stack
            m_state_stack.pop_front();
            m_timer.restart();
            //find nearest storage house and sets as target
						if(!findNearestStorage())
						{
							std::cout<<"cant reach storage house"<<std::endl;
              //remove states from stack, can't find/move to storage house
              m_state_stack.clear();
						}
          }
          else
          {
            std::cout<<m_name<<" didnt catch a fish"<<std::endl;
            m_state_stack.clear();
          }
        }
        break;
      }

		case(State::FORAGE):
		{
			if(m_timer.elapsed() >= 1000)
			{
				m_inventory = CharInventory::BERRY;
				std::cout<<m_name<<" got berries"<<std::endl;

				m_state_stack.pop_front();
				m_timer.restart();

				if(!findNearestStorage())
					m_state_stack.clear();
			}
			break;
		}

    case(State::GET_WOOD):
    {
      if(m_world_inventory->getWoodInventory() >= 1)
      {
        if(m_timer.elapsed() >= 1000)
        {
          std::cout<<m_name<<" colected wood"<<std::endl;
					m_inventory = CharInventory::WOOD;
          m_world_inventory->takeWood(1);
          m_state_stack.pop_front();
          m_target_id = m_final_target_id;
          m_path = findPath(m_target_id);
          m_timer.restart();
        }
      }
      else
      {
        std::cout<<"There's not enough wood"<<std::endl;
        m_state_stack.clear();
      }
      break;
    }

    case(State::COLLECT):
    {
      if(!findNearestStorage())
      {
        std::cout<<"There's no nearby storage house"<<std::endl;
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
					m_inventory = CharInventory::NONE;
					float percentage = 1.0/m_building_amount;
					m_grid->setBuildState(m_final_target_id, percentage, m_building_type);
          ///Change grid tile from NONE type to HOUSE type
          m_state_stack.pop_front();
          m_timer.restart();
        }
        break;
      }

		case(State::REPEAT):
		{
			m_target_id = m_final_target_id;
			findNearestEmptyTile();
			m_state_stack.pop_front();
			m_timer.restart();
			break;
		}

      case(State::IDLE):
      {
        if(m_timer.elapsed() >= 3000)
        {
					std::cout<<"IN IDLE "<<std::endl;
          m_state_stack.pop_front();
          m_timer.restart();
        }
        break;
      }
    }
  }
  else if (m_active == false)
  {
    //setIdleState();
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
      return false;
    }
    else
    {
      return true;
    }
  }
  else
  {
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
      //if the current grid tile is a storehouse, add to vector
      if (m_grid->getTileType(i, j) == TileType::STOREHOUSE)
      {
        storage_houses.push_back(ngl::Vec2(i, j));
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

  if(m_grid->getTileType(target[0]+1, target[1]) == TileType::NONE)
    neighbours.push_back(ngl::Vec2(target[0]-1, target[1]));

  if(m_grid->getTileType(target[0]-1, target[1]) == TileType::NONE)
    neighbours.push_back(ngl::Vec2(target[0]-1, target[1]));

  if(m_grid->getTileType(target[0]-1, target[1]) == TileType::NONE)
    neighbours.push_back(ngl::Vec2(target[0]-1, target[1]));

  if(m_grid->getTileType(target[0]-1, target[1]) == TileType::NONE)
    neighbours.push_back(ngl::Vec2(target[0]-1, target[1]));

  return findNearest(neighbours);
}

bool Character::findNearestFishingTile()
{
  ngl::Vec2 selection = m_grid->idToCoord(m_target_id);

  std::set<int> edge_tile_ids;
  std::set<int> water_tile_ids;

  floodfill(selection, edge_tile_ids, water_tile_ids);

  std::vector<ngl::Vec2> edge_vector;
  for (auto edge: edge_tile_ids)
  {
    ngl::Vec2 coord = m_grid->idToCoord(edge);
    edge_vector.push_back(coord);
  }

	return findNearest(edge_vector);
}

void Character::floodfill(ngl::Vec2 _coord, std::set<int> &_edges, std::set<int> &_water)
{
  //algorithm created with help of Quentin
  //if out of map exit
  if(_coord.m_x >= m_grid->getW() ||
     _coord.m_x <= 0 ||
     _coord.m_y >= m_grid->getH() ||
     _coord.m_y <= 0)
  {
    return;
  }

  int id = m_grid->getTileId(_coord.m_x, _coord.m_y);

  // if tile in edge or water sets exit
  if(_edges.count(id) || _water.count(id))
  {
    return;
  }

  _water.insert(id);

  //Check neighbouring tiles, call flood if water tile
  bool is_edge = false;
  if (m_grid->getTileType(_coord.m_x, _coord.m_y+1) == TileType::WATER)
    floodfill(ngl::Vec2(_coord.m_x, _coord.m_y+1), _edges, _water);
  else
    is_edge = true;

  if (m_grid->getTileType(_coord.m_x, _coord.m_y-1) == TileType::WATER)
    floodfill(ngl::Vec2(_coord.m_x, _coord.m_y-1), _edges, _water);
  else
    is_edge = true;

  if (m_grid->getTileType(_coord.m_x-1, _coord.m_y) == TileType::WATER)
    floodfill(ngl::Vec2(_coord.m_x-1, _coord.m_y), _edges, _water);
  else
    is_edge = true;

  if(m_grid->getTileType(_coord.m_x+1, _coord.m_y) == TileType::WATER)
    floodfill(ngl::Vec2(_coord.m_x+1, _coord.m_y), _edges, _water);
  else
    is_edge = true;


  if(is_edge)
  {
    _edges.insert(id);
    return;
  }
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

ngl::Vec3 Character::getPos()
{
  //get grid height at character's position
  float height = m_grid->getInterpolatedHeight(m_pos[0], m_pos[1]);
  return ngl::Vec3(m_pos[0], height, m_pos[1]);
}
