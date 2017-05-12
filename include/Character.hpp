#ifndef __CHARACTER_HPP__
#define __CHARACTER_HPP__

#include "AI.hpp"

#include "Grid.hpp"
#include "Inventory.hpp"
#include "TerrainHeightTracer.hpp"
#include "ngl/Vec2.h"

#include <QTime>
#include <vector>
#include <stack>
#include <set>
#include <SDL.h>

/// @file Character.hpp
/// @brief Has multiple states for actions, responsible for updating itself

/// @enum State
/// @brief The State enum, used for stack of states that are handled internally in a switch statment

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
	TRACK,
	FIGHT,
	REPEAT,
	IDLE
};

/// @enum CharInventory
/// @brief The CharInventory enum, used to define what the character is holding
///
enum class CharInventory
{
	WOOD,
	FISH,
	BERRIES,
	NONE
};

class Baddie;

/// @class Character
/// @brief Information for ingame characters, containing position, states and targets
class Character : public AI
{
public:
	///
	/// @brief ctor, sets reference to grid and initialised values
	/// @param [in] _height_tracer, pointer to height tracer, used for getting height at positions
	/// @param [in] _grid, pointer to the grid to reference for pathfinding
	/// @param [in] _world_inventory, pointer to global inventory
	/// @param [in] _name, name for character
	/// @param [in] _baddies, pointer to vector of baddies in the game
	///
	Character(TerrainHeightTracer *_height_tracer, Grid *_grid, Inventory *_world_inventory, std::string _name, std::vector<Baddie> *_baddies);
	///
	/// @brief destructor
	///
	~Character() = default;
	///
	/// @brief setState, creates state stack for the character to execute
	///
	void setState(int _target_id);
	///
	/// @brief setForageState, sets character for foraging instead of chopping
	///
	void setForageState() {m_forage = true; softResetCharacter();}
	///
	/// @brief isBaddie, checks if an empty square has been selected or a enemy
	///
	void isBaddie();
	///
	/// @brief buildState, tell character to start building on current square
	/// @param [in] _building type of building to build
	///
	void buildState(TileType _building);
	///
	/// @brief moveState, character moves to its target
	///
	void moveState();
	///
	/// @brief attackState, character intiates fight with enemy
	///
	void attackState();
	///
	/// @brief invadedState, enemy intiates fight with character
	///
	void invadedState( Baddie *_target);
	///
	/// @brief chopState, character collects wood from a tree
	///
	void chopState();
	///
	/// @brief fishState, character tries to catch a fish
	///
	void fishState();
	///
	/// @brief forageState, character finds tree to get berries
	///
	void forageState();
	///
	/// @brief storeState, character stores whatever is in its inventory
	///
	void storeState();
	///
	/// @brief sleepState, recovers character stamina and makes them inactive
	///
	void sleepState();
	///
	/// @brief eatBerries, character eats berries and restores stamina
	///
	void eatBerriesState();
	///
	/// @brief eatFish, character eats fish and restores stamina
	///
	void eatFishState();
	///
	/// @brief idleState, randomly moves character while not active
	///
	void idleState();
	///
	/// @brief clearState, removes any actions in the state stack
	///
	void clearState() {m_state_stack.clear();}
	///
	/// @brief update, updates character based on its current state
	///
	void update();
	///
	/// @brief getID, get the unique character id
	/// @return m_id, character's id
	///
	int getID() {return m_id;}
	///
	/// @brief getName, get character's name
	/// @return m_name, character's name
	///
	std::string getName() {return m_name;}
	///
	/// @brief getColour, get character's colour
	/// @return m_colour, character's colour
	///
	ngl::Vec3 getColour() {return m_colour;}
	///
	/// @brief getState get character's current state
	/// @return character's current state
	///
	State getState();
	///
	/// @brief getStamina get character's stamina
	/// @return character's stamina value 0-1
	///
	float getStamina() {return m_stamina;}
	///
	/// @brief getHunger get character's hunger
	/// @return character's hunger value 0-1
	///
	float getHunger() {return m_hunger;}
	///
	/// @brief getAttributes, gets character attributes such as chopping speed
	/// @return a vector of m_chopping_speed, m_building_speed, m_fishing_catch, m_forage_amount, m_attack_power
	///
	std::vector<float> getAttributes();
	///
	/// @brief setActive, set's whether the character is active
	/// @param [in] _selection, a boolean determing whether the character is active or not
	///
	void setActive(bool _selection) {m_active = _selection;}
	///
	/// @brief isActive, returns whether the  character is active
	/// @return m_active, the boolean stored in the character determining if it is active or not
	///
	bool isActive() {return m_active;}
	///
  /// @brief isInside, returns whether the character is inside a house or storehouse
  /// @return boolean OR operation between storing and sleeping so that if it is in a house or storehouse it returns true
  ///
  bool isInside() {return (m_sleeping || m_storing);}
  ///
  /// @brief isSleeping, returns whether the character is sleeping
  /// @return m_sleeping, the boolean stored in the character determining if it is sleeping in a house
  ///
  bool isSleeping() {return m_sleeping;}

private:
	///
	/// @brief m_id_counter, counts how many characters have been created
	///
	static int m_id_counter;
	///
	/// @brief m_id, current character's id
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
	/// @brief m_stamina, how much stamina the character has
	///
	float m_stamina;
	///
	/// @brief m_hunger, how hungry the character is, 1 = full, 0 = hungry
	///
	float m_hunger;
	///
	/// @brief m_active, sets if the current character is selected
	///
	bool m_active;
  ///
  /// @brief m_sleeping, checks if the character is inside a house
  ///
  bool m_sleeping;
	///
  /// @brief m_storing, checks if the character is inside a storehouse
	///
  bool m_storing;
	///
	/// @brief m_forage, checks if character is wanting to forage
	///
	bool m_forage;
	///
	/// @brief m_world_inventory, inventory in store houses around the map
	///
	Inventory *m_world_inventory;
	///
	/// @brief m_baddies, vector of baddies in world
	///
	std::vector<Baddie> *m_baddies;
	///
	/// @brief m_target_baddie, current enemy
	///
	Baddie *m_target_baddie;
	///
	/// @brief m_dest_target_id, id of character's final destination or reoccuring destination
	///
	int m_dest_target_id;
	///
	/// @brief m_building_tile, tile to build on
	///
	int m_building_tile;
	///
	/// @brief m_hunger_timer, timer for how fast hunger depletes and regenerates
	///
	QTime m_hunger_timer;
	///
	/// @brief m_health_timer, timer for how fast health depletes and regenerates
	///
	QTime m_health_timer;
	///
	/// @brief m_stamina_timer, timer for how fast stamina depletes and regenerates
	///
	QTime m_stamina_timer;
	///
	/// @brief m_state_stack, stack containing sequence of states to reach an end goal, such as chopping wood
	///
	std::deque <State> m_state_stack;
	///
	/// @brief m_inventory, character's current inventory
	///
	CharInventory m_inventory;
	///
	/// @brief m_chopping_speed, how long it takes the character to chop wood
	///
	int m_chopping_speed;
	///
	/// @brief m_building_speed, how long it takes the character to build a house
	///
	int m_building_speed;
	///
	/// @brief m_fishing_catch, likeyhood of catching a fish
	///
	int m_fishing_catch;
	///
	/// @brief m_forage_amount, how many berries they find
	///
	int m_forage_amount;
	///
	/// @brief m_attack_power, how much the character attacks for
	///
	int m_attack_power;
	///
	/// @brief allows for checking if this is the first time the character has been set idle between actions
	///
	int m_called;
	///
	/// @brief m_idle_target_id, grid id that the character moves around while idle
	///
	int m_idle_target_id;
	///
	/// @brief m_building_amount, how many times it takes to build a house or storage house
	///
	int m_building_amount;
	///
	/// @brief m_building_type, the type of building the character is building
	///
	TileType m_building_type;
	///
	/// @brief randomIdlePos, find random position within a radius
	/// @param [in] _idle_pos, central position for idle walking
	/// @param [in] _radius, radius to find position in
	/// @return whether position was found
	///
	bool randomIdlePos(ngl::Vec2 _idle_pos, int _radius);
	///
	/// @brief findNearestStorage, finds a storage house close to the character and sets it as the target
	/// @return a boolean determining whether a storage house was found
	///
	bool findNearestStorage();
	///
	/// @brief findNearestEmptyTile, finds nearest neighbouring empty grid tile
	/// @return a boolean determing whether a neighbouring empty tile was found
	///
	bool findNearestEmptyTile();
	///
	/// @brief findNearestFishingTile, finds a tile on the edge of water for the character to fish from
	/// @return a boolean determining if a tile to fish from was found
	///
	bool findNearestFishingTile();
	///
	/// @brief waterFloodfill, uses a flood fill algorithm to find the edges of a body of water
	/// @param [in] _coord, the current tile's coordinate
	/// @param [in,out] _edges, a vector of tile id's that are the edge of the water
	/// @param [in,out] _water, a vector of tile id's that are water tiles
	///
	void waterFloodfill(ngl::Vec2 _coord, std::set<int> &_edges, std::set<int> &_water);
	///
	/// @brief distanceSort, sorts a vector based on the squared distance from a character
	/// @param [in] io_left, the index of the first element in the sub-section of the vector
	/// @param [in] io_right, the index of the last element in the sub-section of the vector
	/// @param [in,out] _vector, vector of tile positions
	///
	void distanceSort(int io_left, int io_right, std::vector<ngl::Vec2> &_vector);
	///
	/// @brief findNearest, finds the shortest distance for a character in a vector of given coordinates
	/// @param _coord_data, vector containing vec2's of coordinates to sort through
	/// @return a boolean determining if the shortest distance has been found
	///
	bool findNearest(std::vector<ngl::Vec2> _coord_data);
	///
	/// @brief findFirstPath, finds path to tiles, if a path is find then the function is exited
	/// @param [in] _vector, vector of tile coordinates
	/// @return a boolean determining if a path was found to a coordinate in the vector
	///
	bool findFirstPath(std::vector<ngl::Vec2> _vector);
	///
	/// @brief softResetCharacter, clears a characters stack and resets their speed, used when changing from
	/// a character's idle state where the character's speed is reduced
	///
	void softResetCharacter();
	///
	/// @brief hardResetCharacter, does the same as the softResetCharacter except it also turns off the foraging flag
	///
	void hardResetCharacter();
	///
	/// @brief completedAction, when a state has been completed it is removed from the stack and the
	/// internal timer is reset
	///
	void completedAction() {m_state_stack.pop_front(); m_action_timer.restart();}
	///
	/// @brief staminaMessage, generic message that is used when a character doesnt have enough stamina to
	/// complete a task
	///
	void staminaMessage();
	///
	/// @brief generalMessage, for sending a message to the UI
	/// @param [in] _print, message being printed by the UI
	/// @param [in] _id, tile id of where the camera will move to
	///
	void generalMessage(std::string _print, int _id);
	///
	/// @brief generalMessage, for sending a message to the UI
	/// @param [in] _print, message being printed by the UI
	/// @param [in] _coord, coordinate of where the camera will move to
	///
	void generalMessage(std::string _print, ngl::Vec2 _coord);
};

#endif//__CHARACTER_HPP__
