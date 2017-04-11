#include "Character.hpp"

#include "Node.hpp"
#include "NodeNetwork.hpp"
#include "Prefs.hpp"
#include "Gui.hpp"
#include "Utility.hpp"

#include "ngl/Random.h"
#include "ngl/NGLStream.h"

#include <iostream>
#include <memory>

int Character::m_id_counter(1);

Character::Character(Grid *_grid, Inventory *_world_inventory, std::string _name):
  m_id(m_id_counter++),
  m_name(_name),
  m_stamina(1.0),
  m_active(false),
  m_sleeping(false),
  m_grid(_grid),
  m_world_inventory(_world_inventory),
  m_speed(0),
  m_inventory(CharInventory::NONE),
  m_called(0),
  m_building_amount(0),
  m_building_type(TileType::NONE)
{
  //get values from preferences
  Prefs* prefs = Prefs::instance();
  m_speed = prefs->getFloatPref("CHARACTER_SPEED");
  m_building_amount = prefs->getIntPref("CHARACTER_BUILDING");

  //timer for actions
  m_timer.start();

  //create random times for abilities
  //wood chopping speed
  m_chopping_speed = Utility::randInt(5, 10);
  //building houses speed
  m_building_speed = Utility::randInt(5,10);
  //probability of catching a fish
  m_fishing_catch = Utility::randInt(1,3);
  //fishing speed
  m_fishing_speed = Utility::randInt(m_fishing_catch,5);
  //amount of berries collected
  m_forage_amount = Utility::randInt(1,5);

  //random colour
  float red = Utility::randFlt(0, 1);
  float blue = Utility::randFlt(0,1);
  float green = Utility::randFlt(0,1);

  m_colour = ngl::Vec3(red, blue, green);

  //random starting position
  bool valid = false;
  float x,y = 0;

  //find random starting position
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

void Character::setState(int _target_id)
{
  resetCharacter();
  //if the target is reachable or a water tile
  if(setTarget(_target_id) || m_grid->getTileType(_target_id)== TileType::WATER)
  {
    m_target_id = _target_id;

    m_called = 0;

    //create different state stacks depending on grid tile clicked on
    if (m_grid->getTileType(m_target_id) == TileType::TREES)
      chopState();
    else if (m_grid->getTileType(m_target_id) == TileType::WATER)
      fishState();
    else if (m_grid->getTileType(m_target_id) == TileType::NONE)
      moveState();
    else if (m_grid->getTileType(m_target_id) == TileType::HOUSE)
      sleepState();
    else if (m_grid->getTileType(m_target_id) == TileType::FOUNDATION_A ||
             m_grid->getTileType(m_target_id) == TileType::FOUNDATION_B)
      buildState(TileType::HOUSE);
    else if (m_grid->getTileType(m_target_id) == TileType::FOUNDATION_C ||
             m_grid->getTileType(m_target_id) == TileType::FOUNDATION_D)
      buildState(TileType::STOREHOUSE);
  }
  else
    //if the tile is unreachable
    generalMessage(" can't get there!", _target_id);
}

void Character::buildState(TileType _building)
{
  //working out how many iterations it would take to complete the building
  float amount = 1.0/m_building_amount;
  float completed = m_grid->getBuildState(m_target_id);
  int stages = ceil(completed/amount);
  int iterations = m_building_amount - stages;

  //check if character has enough stamina
  if(m_stamina >= 0.3)
  {
    resetCharacter();
    m_building_tile = m_target_id;
    //if character is holding something that isnt wood, store it first
    if(m_inventory != CharInventory::NONE && m_inventory != CharInventory::WOOD)
      storeState();
    //if the character has wood in its inventory or in storage
    if(m_inventory == CharInventory::WOOD || m_world_inventory->getWoodInventory() >= 1)
    {
      //Start building the building type at current position
      if(findNearestEmptyTile())
      {
        m_dest_target_id = m_building_tile;
        m_building_type = _building;
        //create cycle of states
        for(int i =0; i<iterations; i++)
        {
          m_state_stack.push_back(State::COLLECT_WOOD);
          m_state_stack.push_back(State::MOVE);
          m_state_stack.push_back(State::GET_WOOD);
          m_state_stack.push_back(State::MOVE);
          m_state_stack.push_back(State::BUILD);
        }
        m_state_stack.push_back(State::IDLE);
        generalMessage(" has started building", m_dest_target_id);
      }
    }
    else
      Gui::instance()->notify("There's not enough wood to build", m_pos);
  }
  else
    staminaMessage();
}

void Character::moveState()
{
    m_state_stack.push_back(State::MOVE);
    m_state_stack.push_back(State::IDLE);
}

void Character::chopState()
{
  //check if character has enough stamina
  if(m_stamina >= 0.1)
  {
    m_dest_target_id = m_target_id;
    if(findNearestEmptyTile())
    {
      //amount of wood a tree holds
      int wood_amount = 9;
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
      m_state_stack.push_back(State::IDLE);
      generalMessage(" has started chopping wood", m_dest_target_id);
    }
  }
  else
    staminaMessage();
}

void Character::fishState()
{
  //check if character has enough stamina
  if(m_stamina >= 0.3)
  {
    if(findNearestFishingTile())
    {
      m_dest_target_id = m_target_id;
      m_state_stack.push_back(State::MOVE);
      m_state_stack.push_back(State::FISH);
      m_state_stack.push_back(State::MOVE);
      m_state_stack.push_back(State::STORE);
      m_state_stack.push_back(State::MOVE);
      m_state_stack.push_back(State::IDLE);
    }
    generalMessage(" has started fishing", m_dest_target_id);
  }
  else
    staminaMessage();
}

void Character::forageState()
{
  //check if character has enough stamina
  if(m_stamina >= 0.1)
  {
    resetCharacter();
    //if (m_inventory != CharInventory::NONE)
    //	storeState();

    findNearestTree();
    if(findNearestEmptyTile())
    {
      int berry_amount = m_forage_amount + Utility::randInt(0, 2);
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
      m_state_stack.push_back(State::IDLE);
      generalMessage(" has started foraging", m_dest_target_id);
    }
  }
  else
    staminaMessage();
}

void Character::storeState()
{
  if(findNearestStorage())
  {
    m_state_stack.push_back(State::MOVE);
    m_state_stack.push_back(State::STORE);
    m_state_stack.push_back(State::MOVE);
  }
}

void Character::sleepState()
{
  generalMessage(" is going to sleep", m_target_id);
  m_state_stack.push_back(State::MOVE);
  m_state_stack.push_back(State::SLEEP);
  m_state_stack.push_back(State::MOVE);
  m_state_stack.push_back(State::IDLE);
}

void Character::eatBerriesState()
{
  resetCharacter();
  if(m_inventory != CharInventory::NONE && m_inventory != CharInventory::BERRIES)
    storeState();

  if (m_inventory == CharInventory::BERRIES)
  {
    generalMessage(" is eating berries", m_pos);
    m_state_stack.push_back(State::EAT_BERRIES);
  }
  else if(m_world_inventory->getBerryInventory() >= 5)
  {
    generalMessage(" is eating berries", m_pos);
    m_state_stack.push_back(State::COLLECT_BERRIES);
    m_state_stack.push_back(State::MOVE);
    m_state_stack.push_back(State::GET_BERRIES);
    m_state_stack.push_back(State::MOVE);
    m_state_stack.push_back(State::EAT_BERRIES);
  }
  else
    Gui::instance()->notify("There's not enough berries", m_pos);
}

void Character::eatFishState()
{
  resetCharacter();
  if(m_inventory != CharInventory::NONE && m_inventory != CharInventory::FISH)
    storeState();

  if(m_inventory == CharInventory::FISH)
  {
    generalMessage(" is eating fish", m_pos);
    m_state_stack.push_back(State::EAT_FISH);
  }
  else if (m_world_inventory->getFishInventory() >= 1)
  {
    generalMessage(" is eating fish", m_pos);
    m_state_stack.push_back(State::COLLECT_FISH);
    m_state_stack.push_back(State::MOVE);
    m_state_stack.push_back(State::GET_FISH);
    m_state_stack.push_back(State::MOVE);
    m_state_stack.push_back(State::EAT_FISH);
  }
  else
    Gui::instance()->notify("There's not enough fish", m_pos);
}

void Character::idleState()
{
  if (m_called == 0)
  {
    m_idle_target_id = m_target_id;
    m_speed /= 10;
  }

  ngl::Vec2 idle_pos = m_grid->idToCoord(m_idle_target_id);
  int dist = 5;
  int max_attempts = 10;
  bool valid = false;
  int attempt_number = 0;
  while(!valid && attempt_number<max_attempts)
  {
    attempt_number++;
    int x_min_range = Utility::clamp((idle_pos.m_x - dist), 0, m_grid->getW());
    int x_max_range = Utility::clamp((idle_pos.m_x + dist), 0, m_grid->getW());
    int y_min_range = Utility::clamp((idle_pos.m_y - dist), 0, m_grid->getH());
    int y_max_range = Utility::clamp((idle_pos.m_y + dist), 0, m_grid->getH());

    int x = Utility::randInt(x_min_range, x_max_range);
    int y = Utility::randInt(y_min_range, y_max_range);
    valid = setTarget(ngl::Vec2(x,y));
    // if target tile is not traversable (eg if target is tree)
    // this stops them from idly moving through trees and then getting stuck
    if(!m_grid->isTileTraversable(m_target_id))
    {
      valid = false;
      m_target_id = m_grid->coordToId(m_pos);
    }
  }

  if(m_stamina < 1.0)
    m_stamina += 0.001;
  m_state_stack.push_back(State::MOVE);
  m_called++;
}

void Character::update()
{
  std::string message;
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
          completedAction();
        }
        break;
      }

      case(State::CHOP_WOOD):
      {
        //when chopping speed has been reached, gain a piece of wood
        if(m_timer.elapsed() >= 1000 * m_chopping_speed)
        {
          int wood_taken = m_grid->cutTileTrees(m_dest_target_id, 1);
          if (wood_taken == 1)
          {
            //take stamina away
            m_stamina -= 0.1;
            m_inventory = CharInventory::WOOD;
            generalMessage(" got a piece of wood", m_pos);

            //remove the state from the stack
            completedAction();
            //finds nearest storage house and sets as target
            findNearestStorage();
          }
          else
          {
            //no wood left to chop
            Gui::instance()->notify("There's no tree left", m_grid->idToCoord(m_dest_target_id));
            m_state_stack.clear();
          }
        }
        break;
      }

      case(State::STORE):
      {
        if(m_timer.elapsed() >= 1000)
        {
          //if the character is holding wood, add to inventory
          if (m_inventory == CharInventory::WOOD)
          {
            m_world_inventory->addWood(1);
            generalMessage(" deposited wood", m_target_id);
          }
          //if the character is holding fish, add to inventory
          else if (m_inventory == CharInventory::FISH)
          {
            m_world_inventory->addFish(1);
            generalMessage(" deposited a fish", m_target_id);
          }
          //if the character is holding berries, add to inventory
          else if (m_inventory == CharInventory::BERRIES)
          {
            m_world_inventory->addBerries(5);
            generalMessage(" deposited berries", m_target_id);
          }

          //reset character inventory
          m_inventory = CharInventory::NONE;
          //find nearest empty tile to the storage house
          findNearestEmptyTile();
          completedAction();
        }
        break;
      }

      case(State::FISH):
      {
        //when fishing speed reached, gain piece of fish
        if(m_timer.elapsed() >= 1000 * m_fishing_speed)
        {
          //take away stamina
          m_stamina -= 0.3;
          //random chance of catching a fish
          ngl::Random *rand = ngl::Random::instance();
          int fish_rand = rand->randomPositiveNumber(10);
          if(fish_rand % m_fishing_catch == 0)
          {
            //character has fish in inventory
            m_inventory=CharInventory::FISH;
            generalMessage(" caught a fish", m_pos);
            //remove state from stack
            completedAction();
            //find nearest storage house and sets as target
            findNearestStorage();
          }
          else
          {
            //if no fish is caught
            generalMessage(" didn't catch a fish", m_pos);
            m_state_stack.clear();
          }
        }
        break;
      }

      case(State::FORAGE):
      {
        if(m_timer.elapsed() >= 1000)
        {
          //take away stamina
          m_stamina -= 0.1;
          //character has berries in inventory
          m_inventory = CharInventory::BERRIES;
          generalMessage(" got berries", m_pos);
          //remove state from stack
          completedAction();
          //find nearest storage house and sets as target
          findNearestStorage();
        }
        break;
      }

      case(State::COLLECT_WOOD):
      {
        //if the character isnt carrying wood, collect wood
        if (m_inventory != CharInventory::WOOD)
        {
          //if storage house found, remove state from stack
          if(findNearestStorage() && m_world_inventory->getWoodInventory() >= 1)
            completedAction();
          else
          {
            //no wood in storage house
            Gui::instance()->notify("There's not enough wood to build", m_pos);
            m_state_stack.clear();
          }
        }
        else
        {
          //remove collect, move and get_wood states if character holding wood
          m_state_stack.pop_front();
          m_state_stack.pop_front();
          m_state_stack.pop_front();
          m_timer.restart();
        }
        break;
      }

      case(State::COLLECT_BERRIES):
      {
        //if the character isnt carrying berries, collect berries
        if (m_inventory != CharInventory::BERRIES)
        {
          //if storage house found, remove state from stack
          if(findNearestStorage())
            completedAction();
        }
        else
        {
          //remove collect, move and get_berries states if character holding berries
          m_state_stack.pop_front();
          m_state_stack.pop_front();
          m_state_stack.pop_front();
          m_timer.restart();
        }
        break;
      }

      case(State::COLLECT_FISH):
      {
        //if the character isnt carrying a fish, collect fish
        if (m_inventory != CharInventory::FISH)
        {
          //if storage house found, remove state from stack
          if(findNearestStorage())
            completedAction();
        }
        else
        {
          //remove collect, move and get_fish states if character holding fish
          m_state_stack.pop_front();
          m_state_stack.pop_front();
          m_state_stack.pop_front();
          m_timer.restart();
        }
        break;
      }


      case(State::GET_WOOD):
      {
        if(m_timer.elapsed() >= 1000)
        {
          //character has wood in inventory
          m_inventory = CharInventory::WOOD;
          generalMessage(" collected wood", m_pos);

          //take wood from universal storage
          m_world_inventory->takeWood(1);
          //set final destination as the new target for the character
          m_target_id = m_dest_target_id;
          m_path = findPath(m_target_id);
          //setTarget(m_target_id);/////////// DOESNT WORK FOR SOME REASON??///////////////
          //remove state from stack
          completedAction();
        }
        break;
      }

      case(State::GET_BERRIES):
      {
        if(m_timer.elapsed() >= 1000)
        {
          //character has berries in inventory
          m_inventory = CharInventory::BERRIES;
          generalMessage(" collected berries", m_pos);

          //take berries from universal storage
          m_world_inventory->takeBerries(5);
          findNearestEmptyTile();
          //remove state from stack
          completedAction();
        }
        break;
      }

      case(State::GET_FISH):
      {
        if(m_timer.elapsed() >= 1000)
        {
          //character has fish in inventory
          m_inventory = CharInventory::FISH;
          generalMessage(" collected a fish", m_pos);

          //take fish from universal storage
          m_world_inventory->takeFish(1);
          findNearestEmptyTile();
          //remove state from stack
          completedAction();
        }
        break;
      }

      case(State::BUILD):
      {
        //check in case building has been finished by another character
        if(m_grid->getBuildState(m_building_tile) >= 1.0)
        {
          m_building_type = TileType::NONE;
          m_state_stack.clear();
          //put wood back that isnt used
          storeState();
        }

        if(m_timer.elapsed() >= 1000 * m_building_speed)
        {
          //take away stamina
          m_stamina -= 0.3;
          m_inventory = CharInventory::NONE;

          //amount of building complete
          float percentage = 1.0/m_building_amount;
          m_grid->setBuildState(m_building_tile, percentage, m_building_type);

          //if charaters has finished building
          if(m_grid->getBuildState(m_building_tile) >= 1.0)
          {
            Gui::instance()->notify("Building complete!", m_grid->idToCoord(m_building_tile));
            m_building_type = TileType::NONE;
          }

          completedAction();
          if (m_stamina < 0.3)
            //if not enough stamina to build again
            staminaMessage();
        }
        break;
      }

      case(State::EAT_BERRIES):
      {
        if(m_timer.elapsed() >= 3000)
        {
          //take berries away from inventory
          m_inventory = CharInventory::NONE;

          //add onto stamina
          m_stamina += 0.2;
          if (m_stamina >= 1.0)
            m_stamina = 1.0;

          generalMessage(" ate berries", m_pos);
          completedAction();
        }
        break;
      }

      case(State::EAT_FISH):
      {
        if(m_timer.elapsed() >= 3000)
        {
          //take fish from inventory
          m_inventory = CharInventory::NONE;

          //add onto stamina
          m_stamina += 0.5;
          if (m_stamina >= 1.0)
            m_stamina = 1.0;

          generalMessage(" ate a fish", m_pos);
          completedAction();
        }
        break;
      }

      case(State::REPEAT):
      {
        //repeat for foraging and chopping wood (have same stamina)
        if(m_stamina >= 0.1)
        {
          m_target_id = m_dest_target_id;
          //find nearest empty tile to the destination target
          findNearestEmptyTile();
          //remove state from stack
          completedAction();
        }
        else
          //if not enough stamina
          staminaMessage();
        break;
      }

      case(State::IDLE):
      {
        //character doesnt move after an action for 3 seconds
        if(m_timer.elapsed() >= 3000)
        {
          //remove state from stack
          completedAction();
        }
        break;
      }

      case(State::SLEEP):
      {
        //how long to sleep to recover stamina
        float recover = (1.0 - m_stamina);
        if(recover > 0)
        {
          //character isn't drawn when m_sleeping is true
          m_sleeping = true;
          //character is no longer the active character
          m_active = false;
          if(m_timer.elapsed() >= 1000 * (recover*100))
          {
            m_sleeping = false;
            m_stamina = 1.0;
            generalMessage(" has finished sleeping", m_pos);
            findNearestEmptyTile();
            //remove state from stack
            completedAction();
          }
        }
        else
        {
          //if stamina is full
          if(m_timer.elapsed() >= 1000)
          {
            generalMessage(" doesn't need to sleep", m_pos);
            findNearestEmptyTile();
            //remove state from stack
            completedAction();
          }
        }
      }

    }
  }
  else if (m_active == false)
  {
    //if a character isn't active they are idle
    idleState();
  }
}

bool Character::move()
{
  // check whether the next point is necessary or if character has line of sight to the second point in list
  if(m_path.size() > 1 && NodeNetwork::raytrace(m_grid, m_pos, m_path.end()[-2]))
  {
    // if character can see point after next, remove the next point so character heads directly to the furtherst one they can see
    // done on a one by one basis so it does all happen in the same frame as pathfinding, not super slow that way but marginally better this way
    m_path.pop_back();
  }

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
  int occupant_limit = Prefs::instance()->getIntPref("MAX_TILE_OCCUPANTS");
  int target_occupants = m_grid->getOccupants(_tile_id);

  int m_old_target = m_target_id;
  if (target_occupants < occupant_limit)
  {
    //if the chosen tile isnt equal to the target and isnt equal to the character's pos
    if(_tile_id != m_target_id && _tile_id != m_grid->coordToId(m_pos))
    {
      m_target_id = _tile_id;
      m_path = findPath(m_target_id);
      //if no path was found, return false
      if(m_path.size() <= 0)
      {
        return false;
      }
      else
      {
        m_grid->addOccupant(_tile_id);
        m_grid->removeOccupant(m_old_target);
        return true;
      }
    }
    else
    {
      return false;
    }
  }
  else
  {
    Gui::instance()->notify("There are too many people there :(", m_grid->idToCoord(_tile_id));
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

  //sort vector based on distance from character's position
  distanceSort(0, storage_houses.size()-1, storage_houses);

  //set the storehouse with the shortest path as the target
  if(	findNearest(storage_houses) == true)
    return true;
  else
  {
    generalMessage(" can't get to a store house!", m_pos);
    m_state_stack.clear();
    return false;
  }
}

bool Character::findNearestEmptyTile()
{
  //checks neighbouring tiles to see if they are empty,
  //if empty add to vector neighbours
  std::vector<ngl::Vec2> neighbours;
  ngl::Vec2 target = m_grid->idToCoord(m_target_id);

  if(m_grid->getTileType(target[0]+1, target[1]) == TileType::NONE)
    neighbours.push_back(ngl::Vec2(target[0]+1, target[1]));

  if(m_grid->getTileType(target[0]-1, target[1]) == TileType::NONE)
    neighbours.push_back(ngl::Vec2(target[0]-1, target[1]));

  if(m_grid->getTileType(target[0], target[1]+1) == TileType::NONE)
    neighbours.push_back(ngl::Vec2(target[0], target[1]+1));

  if(m_grid->getTileType(target[0], target[1]-1) == TileType::NONE)
    neighbours.push_back(ngl::Vec2(target[0], target[1]-1));

  //find tile with the shortest distance to the character's current position
  if(findNearest(neighbours) == true)
    return true;
  else
  {
    generalMessage(" can't get there!", m_target_id);
    m_state_stack.clear();
    return false;
  }
}

bool Character::findNearestFishingTile()
{
  ngl::Vec2 selection = m_grid->idToCoord(m_target_id);

  //set of tiles that make the edge of the body of water
  std::set<int> edge_tile_ids;
  //set of tiles that make up the body of water
  std::set<int> water_tile_ids;

  waterFloodfill(selection, edge_tile_ids, water_tile_ids);

  //create a vector of vec2's from the set
  std::vector<ngl::Vec2> edge_vector;
  for (auto edge: edge_tile_ids)
  {
    ngl::Vec2 coord = m_grid->idToCoord(edge);
    edge_vector.push_back(coord);
  }

  //sort vector based on squared distance to character
  distanceSort(0, edge_vector.size()-1, edge_vector);
  //go through vector until a viable path is find
  return findFirstPath(edge_vector);
}

bool Character::findNearestTree()
{
  bool found = false;
  treeFloodfill(m_pos, found);
  if (found == true)
  {
    //m_target_id = m_dest_target_id;
    //setTarget(m_target_id);
    return true;
  }
  else
    return false;
}

void Character::waterFloodfill(ngl::Vec2 _coord, std::set<int> &_edges, std::set<int> &_water)
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

  if (m_grid->getTileType(_coord.m_x, _coord.m_y) == TileType::WATER)
    waterFloodfill(ngl::Vec2(_coord.m_x-1, _coord.m_y), _edges, _water);
  else
    is_edge = true;

  if (m_grid->getTileType(_coord.m_x-1, _coord.m_y) == TileType::WATER)
    waterFloodfill(ngl::Vec2(_coord.m_x+1, _coord.m_y), _edges, _water);
  else
    is_edge = true;

  if(m_grid->getTileType(_coord.m_x, _coord.m_y-1) == TileType::WATER)
    waterFloodfill(ngl::Vec2(_coord.m_x, _coord.m_y-1), _edges, _water);
  else
    is_edge = true;

  if(m_grid->getTileType(_coord.m_x, _coord.m_y+1) == TileType::WATER)
    waterFloodfill(ngl::Vec2(_coord.m_x, _coord.m_y+1), _edges, _water);
  else
    is_edge = true;

  if(is_edge)
  {
    _edges.insert(id);
    return;
  }
}

void Character::treeFloodfill(ngl::Vec2 _coord, bool &_found)
{
  if (_found == true)
    return;

  if(_coord.m_x >= m_grid->getW() ||
     _coord.m_x <= 0 ||
     _coord.m_y >= m_grid->getH() ||
     _coord.m_y <= 0)
  {
    return;
  }

  int id = m_grid->getTileId(_coord.m_x, _coord.m_y);

  if(m_grid->getTileType(id) == TileType::TREES)
  {
    if(setTarget(id))
    {
      _found = true;
      m_dest_target_id = id;
    }
    return;
  }
  else
  {
    treeFloodfill(ngl::Vec2 (_coord.m_x-1, _coord.m_y), _found);
    treeFloodfill(ngl::Vec2 (_coord.m_x+1, _coord.m_y), _found);
    treeFloodfill(ngl::Vec2 (_coord.m_x, _coord.m_y-1), _found);
    treeFloodfill(ngl::Vec2 (_coord.m_x, _coord.m_y+1), _found);
  }
}

void Character::distanceSort(int io_left, int io_right, std::vector<ngl::Vec2> &_vector)
{
 int i = io_left, j = io_right;
 int pivot = (io_left + io_right) / 2;
 float pivot_dist = Utility::sqrDistance(_vector[pivot], m_pos);

 while (i <= j)
 {

     while (Utility::sqrDistance(_vector[i], m_pos) < pivot_dist) i++;
     while (Utility::sqrDistance(_vector[j], m_pos) > pivot_dist) j--;

     if (i <= j)
     {
         ngl::Vec2 tmpCoord = _vector[i];
         _vector[i] = _vector[j];
         _vector[j] = tmpCoord;
         i++;
         j--;
     }
 }

 if (io_left < j)
   distanceSort(io_left, j, _vector);
 if (i < io_right)
   distanceSort (i, io_right, _vector);
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
    generalMessage(" can't get there!", m_target_id);
    return false;
  }
}

bool Character::findFirstPath(std::vector<ngl::Vec2> _vector)
{
  for(auto element: _vector)
  {
    if(setTarget(element))
      return true;
  }
  //if no path found to any tile in the vector
  return false;
}

ngl::Vec3 Character::getPos()
{
  //get grid height at character's position
  float height = m_grid->getInterpolatedHeight(m_pos[0], m_pos[1]);
  return ngl::Vec3(m_pos[0], height, m_pos[1]);
}

void Character::resetCharacter()
{
  //clear states
  std::cout << "reset caled" << std::endl;

  m_state_stack.clear();
  //reset speed
  Prefs* prefs = Prefs::instance();
  m_speed = prefs->getFloatPref("CHARACTER_SPEED");
}

void Character::staminaMessage()
{
  //message printed when the character doesnt have enough stamina to perform an action
  std::string message = m_name + " doesn't have enough stamina";
  Gui::instance()->notify(message, m_pos);
  m_state_stack.clear();
}

void Character::generalMessage(std::string _print, int _id)
{
  generalMessage(_print, m_grid->idToCoord(_id));
}

void Character::generalMessage(std::string _print, ngl::Vec2 _coord)
{
  //general message using the character's name
  std::string message = m_name + _print;
  Gui::instance()->notify(message, _coord);
}

State Character::getState()
{
  // fix to return the current state
  // might need safeguards if state stack is empty i dunno
  return State::IDLE;
}
