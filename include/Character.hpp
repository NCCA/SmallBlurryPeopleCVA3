#ifndef __CHARACTER_HPP__
#define __CHARACTER_HPP__

#include "Grid.hpp"
#include "ngl/Vec2.h"
#include <QTime>
#include <vector>
#include <stack>
#include <SDL.h>

/// \file Character.hpp
/// \brief The character refers to the grid for pathfinding and keeps track of a target
/// for pathfinding. It is responsible for updating and drawing itself.

enum class State
{
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
  /// \brief ctor sets reference to grid and initialised values
  /// \param [in] _grid pointer to the grid to reference for pathfinding
  Character(Grid *_grid);
	/// @brief ctor sets reference to grid and initialised values
  /// @param [in] _grid pointer to the grid to reference for pathfinding
	Character(Grid *_grid, std::string _name);
	///
	/// \brief default destructor
	///
  ~Character() = default;
	///
	/// \brief sets state
	///
	void setState();
	///
	/// \brief update updates character, including calling move()
	///
  void update();
  ///
  /// \brief move moves character along its path
  ///
	bool move();
  ///
  /// \brief draw draws character with openGL
  ///
  void draw();
  ///
  /// \brief findPath pathfinding function to get nodes for pathfinding
  ///
	void findPath();
  ///
  /// \brief calcAimVec calculate vector towards next point
  ///
  ngl::Vec2 calcAimVec(float *dist_squared);
  ///
  /// \brief setTarget set a new target position based on a position
  /// \param _target_pos is the position to pathfind to
  ///
  void setTarget(ngl::Vec2 _target_pos);

  ///
  /// \brief setTarget set a new target based on the grid tile id
  /// \param _tile_id is the tile id to pathfind to
  ///
	void setTarget(int _tile_id);

	bool findNearestStorage();
	///
	/// \brief getID get the unique character id
	/// \return character's id
	///
	int getID() {return m_id;}

	void setActive(bool _selection) {m_active = _selection;}
	bool isActive() {return m_active;}

private:
	///
	/// \brief m_id_counter counts how many objects have been created
	///
	static int m_id_counter;
	///
	/// \brief m_id current object's id
	///
	const int m_id;
	///
	/// @brief current character's name
	///
	std::string m_name;
	///
	/// @brief sets if the current character is selected
	///
	bool m_active;
	///
	/// \brief m_grid grid pointer to reference for pathfinding
	///
  Grid *m_grid;
  ///
  /// \brief m_pos character position
  ///
  ngl::Vec2 m_pos;
  ///
  /// \brief m_target_id id of target tile on grid
  ///
  int m_target_id;
  ///
  /// \brief m_speed max speed of character
  ///
  float m_speed;
  ///
  /// \brief m_path vector of target positions for movement
  ///
	std::vector<ngl::Vec2> m_path;

	QTime m_timer;

	std::deque <State> m_state_stack;

	int m_wood_inventory;

	int m_chopping_speed;
	int m_building_speed;
};

#endif//__CHARACTER_HPP__
