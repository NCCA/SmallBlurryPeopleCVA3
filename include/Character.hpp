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
	STORE_WOOD,
	FISH,
	STORE_FISH,
	COLLECT,
	GET_WOOD,
	BUILD,
	SLEEP,
	FORAGE,
	MOVE,
	IDLE
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
	void setState();
	///
	/// \brief clearState, removes any actions in the state stack
	///
	void clearState() {m_state_stack.clear();}
	///
	/// \brief setIdleState, randomly moves character while not active
	///
	void setIdleState();
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
	/// \brief findNearestStorage, finds the storage house closest to the character and sets it as the target
	/// \return a boolean determining whether a storage house was found
	///
	bool findNearestStorage();
	///
	/// \brief findNearestEmptyTile, finds nearest neighbouring empty grid tile
	/// \return a boolean determing whether a neighbouring empty tile was found
	///
	bool findNearestEmptyTile();

	void findNearestFishingTile();
	void floodfill(ngl::Vec2 _coord, std::set<int> &_edges, std::set<int> &_water);
	//void distanceSort(int io_left, int io_right, std::vector<int> _edges);
	//float findSortDist(std::vector<int> _vector, int index);
	///
	/// \brief findNearest, finds the shortest distance for a character in a vector of given coordinates
	/// \param _coord_data, vector containing vec2's of coordinates to sort through
	/// \return a boolean determining if the shortest distance has been found
	///
	bool findNearest(std::vector<ngl::Vec2> _coord_data);
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
	/// \brief build tell character to start building on current square
	/// \param _building type of building to build
	///
	void build(BuildingType _building);
	///
	/// \brief forage tell character to forage
	///
	void forage();

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
	/// @brief m_active, sets if the current character is selected
	///
	bool m_active;
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
	/// \brief m_final_target_id, id of target character ends on after sequence
	///
	int m_final_target_id;
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
	/// \brief m_wood_inventory, the amount of wood a character has
	///
	int m_wood_inventory;
	/// \brief m_fish_inventory, the amount of fish a character has
	///
	int m_fish_inventory;
	///
	/// \brief m_chopping_speed, how long it takes the character to chop wood
	///
	int m_chopping_speed;
	///
	/// \brief m_building_speed, how long it takes the character to build a house
	///
	int m_building_speed;
	///
	/// \brief m_fishing_catch, likeyhood of catching a fish
	///
	int m_fishing_catch;
	///
	/// \brief allows for checking if this is the first time the character has been set idle between actions
	///
	int m_called;
	///
	/// \brief m_idle_target_id, grid id that the character moves around while idle
	///
	int m_idle_target_id;
};

#endif//__CHARACTER_HPP__
