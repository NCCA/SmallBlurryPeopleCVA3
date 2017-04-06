#ifndef __CHARACTER_HPP__
#define __CHARACTER_HPP__

#include "Grid.hpp"
#include "Inventory.hpp"
#include "ngl/Vec2.h"

#include <QTime>
#include <vector>
#include <stack>
#include <set>
#include <SDL.h>

/// \file Character.hpp
/// \brief The character refers to the grid for pathfinding and keeps track of a target
/// for pathfinding. It is responsible for updating and drawing itself.

enum class State
{
	CHOP_WOOD,
	STORE,
	FISH,
	FORAGE,
	COLLECT,
	GET_WOOD,
	BUILD,
	SLEEP,
	MOVE,
	REPEAT,
	IDLE
};

enum class CharInventory
{
	WOOD,
	FISH,
	BERRIES,
	NONE
};

/// \class Character
/// \brief Information for ingame characters, containing position, states and targets
class Character
{
public:
	///
	/// \brief ctor, sets reference to grid and initialised values
	/// \param [in] _grid, pointer to the grid to reference for pathfinding
	///
  Character(Grid *_grid);
	///
	/// @brief ctor, sets reference to grid and initialised values
	/// @param [in] _grid, pointer to the grid to reference for pathfinding
	///
	Character(Grid *_grid, Inventory *_world_inventory, std::string _name);
	///
	/// \brief default destructor
	///
  ~Character() = default;
	///
	/// \brief setState, creates state stack for the character to execute
	///
	void setState(int _target_id);
	///
	/// \brief buildState, tell character to start building on current square
	/// \param _building type of building to build
	///
	void buildState(TileType _building);
	///
	/// \brief moveState, character moves to its target
	///
	void moveState();
	///
	/// \brief chopState, character collects wood from a tree
	///
	void chopState();
	///
	/// \brief fishState, character tries to catch a fish
	///
	void fishState();
	///
	/// \brief forageState, character finds tree to get berries
	///
	void forageState();
	///
	/// \brief storeState, character stores whatever is in its inventory
	///
	void storeState();
	///
	/// \brief sleepState, recovers character stamina and makes them inactive
	///
	void sleepState();
	///
	/// \brief idleState, randomly moves character while not active
	///
	void idleState();
	///
	/// \brief clearState, removes any actions in the state stack
	///
	void clearState() {m_state_stack.clear();}
	///
	/// \brief update, updates character based on its current state
	///
  void update();
  ///
	/// \brief move, moves character along its path
  ///
	bool move();
  ///
	/// \brief draw, draws character with openGL
  ///
  void draw();
  ///
	/// \brief findPath, pathfinding function to get nodes for pathfinding
  ///
	std::vector<ngl::Vec2> findPath(int _target_id);
  ///
	/// \brief calcAimVec, calculate vector towards next point
	/// \return
  ///
  ngl::Vec2 calcAimVec(float *dist_squared);
  ///
	/// \brief setTarget, set a new target position based on a position
	/// \param _target_pos, the position to pathfind to
  ///
	bool setTarget(ngl::Vec2 _target_pos);
  ///
	/// \brief setTarget, set a new target based on the grid tile id
	/// \param _tile_id, the tile id to pathfind to
  ///
	bool setTarget(int _tile_id);
	///
	/// \brief getID, get the unique character id
	/// \return m_id, character's id
	///
	int getID() {return m_id;}
	///
	/// \brief getName, get character's name
	/// \return m_name, character's name
	///
	std::string getName() {return m_name;}
	///
	/// \brief getColour, get character's colour
	/// \return m_colour, character's colour
	///
	ngl::Vec3 getColour() {return m_colour;}
	///
	/// \brief getPos, get character's position
	/// \return m_pos, character's position
	///
	ngl::Vec3 getPos();
	///
	/// \brief setActive, set's whether the character is active
	/// \param _selection, a boolean determing whether the character is active or not
	///
	void setActive(bool _selection) {m_active = _selection;}
	///
	/// \brief isActive, returns whether the  character is active
	/// \return m_active, the boolean stored in the character determining if it is active or not
	///
	bool isActive() {return m_active;}
	///
	/// \brief isSleeping, returns whether the character is sleeping
	/// \return m_sleeping, the boolean stored in the character determining if it is active or not
	///
	bool isSleeping() {return m_sleeping;}
private:
	///
	/// \brief m_id_counter, counts how many objects have been created
	///
	static int m_id_counter;
	///
	/// \brief m_id, current object's id
	///
	const int m_id;
	///
	/// @brief m_name, current character's name
	///
	std::string m_name;
	///
	/// @brief m_colour, current character's shading colour
	///
	ngl::Vec3 m_colour;
	///
	/// \brief m_stamina, how much stamina the character has
	///
	float m_stamina;
	///
	/// @brief m_active, sets if the current character is selected
	///
	bool m_active;
	///
	/// \brief m_sleeping, checks if the character is sleeping and hence shouldn't be drawn
	///
	bool m_sleeping;
	///
	/// \brief m_grid, grid pointer to reference for pathfinding
	///
  Grid *m_grid;
	///
	/// \brief m_world_inventory, inventory in store houses around the map
	///
	Inventory *m_world_inventory;
  ///
	/// \brief m_pos, character position
  ///
	ngl::Vec2 m_pos;
  ///
	/// \brief m_target_id, id of target tile on grid
  ///
  int m_target_id;
	///
	/// \brief m_dest_target_id, id of character's final destination or reoccuring destination
	///
	int m_dest_target_id;
	///
	/// \brief m_building_tile, tile to build on
	///
	int m_building_tile;
  ///
	/// \brief m_speed, max speed of character
  ///
  float m_speed;
  ///
	/// \brief m_path, vector of target positions for movement
  ///
	std::vector<ngl::Vec2> m_path;
	///
	/// \brief m_timer, timer for characters actions such as chopping wood and building
	///
	QTime m_timer;
	///
	/// \brief m_state_stack, stack containing sequence of states to reach an end goal, such as chopping wood
	///
	std::deque <State> m_state_stack;
	///
	/// \brief m_inventory, character's current inventory
	///
	CharInventory m_inventory;
	///
	/// \brief m_chopping_speed, how long it takes the character to chop wood
	///
	int m_chopping_speed;
	///
	/// \brief m_building_speed, how long it takes the character to build a house
	///
	int m_building_speed;
	///
	/// \brief m_fishing_speed, how long it takes the character to get a fish
	///
	int m_fishing_speed;
	///
	/// \brief m_fishing_catch, likeyhood of catching a fish
	///
	int m_fishing_catch;
	///
	/// \brief m_forage_amount, how many berries they find
	///
	int m_forage_amount;
	///
	/// \brief allows for checking if this is the first time the character has been set idle between actions
	///
	int m_called;
	///
	/// \brief m_idle_target_id, grid id that the character moves around while idle
	///
	int m_idle_target_id;

	int m_building_amount;
	TileType m_building_type;

	///
	/// \brief findNearestStorage, finds the storage house closest to the character and sets it as the target
	/// \return a boolean determining whether a storage house was found
	///
	bool findNearestStorage();
	///
	/// \brief findNearestEmptyTile, finds nearest neighbouring empty grid tile
	/// \return a boolean determing whether a neighbouring empty tile was found
	///
	bool findNearestEmptyTile();
	bool findNearestFishingTile();
	void distanceSort(int io_left, int io_right, std::vector<ngl::Vec2> &_edges);

	void waterFloodfill(ngl::Vec2 _coord, std::set<int> &_edges, std::set<int> &_water);
	void treeFloodfill(ngl::Vec2 _coord, bool &_found);
	bool findNearestTree();
	///
	/// \brief findNearest, finds the shortest distance for a character in a vector of given coordinates
	/// \param _coord_data, vector containing vec2's of coordinates to sort through
	/// \return a boolean determining if the shortest distance has been found
	///
	bool findNearest(std::vector<ngl::Vec2> _coord_data);

	void resetCharacter();
	void completedAction() {m_state_stack.pop_front(); m_timer.restart();}
	void staminaMessage();
	void generalMessage(std::string _print, int _id);
	void generalMessage(std::string _print, ngl::Vec2 _coord);
};

#endif//__CHARACTER_HPP__
