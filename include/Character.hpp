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
/// for pathfinding. It is responsible for updating itself.

enum State
{
	CHOP_WOOD,
	STORE,
	FISH,
	FORAGE,
	CHECK_WOOD,
	CHECK_BERRIES,
	CHECK_FISH,
	GET_WOOD,
	GET_BERRIES,
	GET_FISH,
	BUILD,
	SLEEP,
	EAT_BERRIES,
	EAT_FISH,
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
	/// @brief ctor, sets reference to grid and initialised values
	/// @param [in] _grid, pointer to the grid to reference for pathfinding
	///
	Character(Grid *_grid, Inventory *_world_inventory, std::string _name);
	///
	/// \brief destructor
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
	/// \brief eatBerries, character eats berries and restores stamina
	///
	void eatBerriesState();
	///
	/// \brief eatFish, character eats fish and restores stamina
	///
	void eatFishState();
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
	/// \return true if gets to target
	///
	bool move();
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
	/// \brief getState get character's current state
	/// \return character's current state
	///
	State getState();
	///
	/// \brief getStamina get character's stamina
	/// \return character's stamina value
	///
	float getStamina() {return m_stamina;}
	///
	/// \brief gethealth, get character's health
	/// \return character's health value
	///
	float getHealth() {return m_health;}
	///
	/// \brief takeHealth, take health away from a character
	/// \param m_amount, amount of health to take away
	///
	void takeHealth(float m_amount) {m_health -= m_amount;}
	///
	/// \brief getPos, get character's position
	/// \return m_pos, character's position
	///
	ngl::Vec3 getPos();
	///
	/// \brief updateRot update m_rot, eg if direction has changed
	///
	void updateRot();
	///
	/// \brief getRot return rotation of character
	/// \return m_rot
	///
	float getRot() {return m_rot;}
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
	/// \return m_sleeping, the boolean stored in the character determining if it is sleeping or not
	///
	bool isSleeping() {return m_sleeping;}
	///
	/// \brief isIdle, returns whether the character is idle
	/// \return m_idle, the boolean stored in the character determining if it is idle or not
	///
	bool isIdle() {return m_idle;}

private:
	///
	/// \brief m_id_counter, counts how many characters have been created
	///
	static int m_id_counter;
	///
	/// \brief m_id, current character's id
	///
	int m_id;
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
	/// \brief m_health, how much health the character has: 1 = full health, 0 = no health/dead
	///
	float m_health;
	///
	/// \brief m_hunger, how hungry the character is, 1 = full, 0 = hungry
	///
	float m_hunger;
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
	/// \brief m_rot character's rotation to face current direction (degrees)
	///
	float m_rot;
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
	/// \brief m_action_timer, timer for characters actions such as chopping wood and building
	///
	QTime m_action_timer;
	QTime m_hunger_timer;
	QTime m_health_timer;
	QTime m_stamina_timer;
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
	///
	/// \brief m_idle, set when character is idle, for checking if idle
	///
	bool m_idle;
	///
	/// \brief m_building_amount, how many times it takes to build a house or storage house
	///
	int m_building_amount;
	///
	/// \brief m_building_type, the type of building the character is building
	///
	TileType m_building_type;
	///
	/// \brief findNearestStorage, finds a storage house close to the character and sets it as the target
	/// \return a boolean determining whether a storage house was found
	///
	bool findNearestStorage();
	///
	/// \brief findNearestEmptyTile, finds nearest neighbouring empty grid tile
	/// \return a boolean determing whether a neighbouring empty tile was found
	///
	bool findNearestEmptyTile();
	///
	/// \brief findNearestFishingTile, finds a tile on the edge of water for the character to fish from
	/// \return a boolean determining if a tile to fish from was found
	///
	bool findNearestFishingTile();
	///
	/// \brief findNearestTree, find a tree close to the character for foraging
	/// \return a boolean determining whether a tree tile was found
	///
	bool findNearestTree();
	///
	/// \brief waterFloodfill, uses a flood fill algorithm to find the edges of a body of water
	/// \param _coord, the current tile's coordinate
	/// \param _edges, a vector of tile id's that are the edge of the water
	/// \param _water, a vector of tile id's that are water tiles
	///
	void waterFloodfill(ngl::Vec2 _coord, std::set<int> &_edges, std::set<int> &_water);
	///
	/// \brief treeFloodfill, finds a nearby tree using a flood fill algorithm
	/// \param _coord, the current tile's coordinate
	/// \param _found, when a tree is found then the flood fill is exited
	///
	void treeFloodfill(ngl::Vec2 _coord, bool &_found);
	///
	/// \brief distanceSort, sorts a vector based on the squared distance from a character
	/// \param io_left, the index of the first element in the sub-section of the vector
	/// \param io_right, the index of the last element in the sub-section of the vector
	/// \param _vector, vector of tile positions
	///
	void distanceSort(int io_left, int io_right, std::vector<ngl::Vec2> &_vector);
	///
	/// \brief findNearest, finds the shortest distance for a character in a vector of given coordinates
	/// \param _coord_data, vector containing vec2's of coordinates to sort through
	/// \return a boolean determining if the shortest distance has been found
	///
	bool findNearest(std::vector<ngl::Vec2> _coord_data);
	///
	/// \brief findFirstPath, finds path to tiles, if a path is find then the function is exited
	/// \param _vector, vector of tile coordinates
	/// \return
	///
	bool findFirstPath(std::vector<ngl::Vec2> _vector);
	///
	/// \brief resetCharacter, clears a characters stack and resets their speed, used when changing from
	/// a character's idle state where the character's speed is reduced
	///
	void resetCharacter();
	///
	/// \brief completedAction, when a state has been completed it is removed from the stack and the
	/// internal timer is reset
	///
	void completedAction() {m_state_stack.pop_front(); m_action_timer.restart();}
	///
	/// \brief staminaMessage, generic message that is used when a character doesnt have enough stamina to
	/// complete a task
	///
	void staminaMessage();
	///
	/// \brief generalMessage, for sending a message to the UI
	/// \param _print, message being printed by the UI
	/// \param _id, tile id of where the camera will move to
	///
	void generalMessage(std::string _print, int _id);
	///
	/// \brief generalMessage, for sending a message to the UI
	/// \param _print, message being printed by the UI
	/// \param _coord, coordinate of where the camera will move to
	///
	void generalMessage(std::string _print, ngl::Vec2 _coord);
};

#endif//__CHARACTER_HPP__
