#ifndef __CHARACTER_HPP__
#define __CHARACTER_HPP__

#include "Grid.hpp"
#include "ngl/Vec2.h"
#include <vector>

/// @file Character.hpp
/// @brief The character refers to the grid for pathfinding and keeps track of a target
/// for pathfinding. It is responsible for updating and drawing itself.

/// @class Character
/// @brief Information for ingame characters, containing position, states and targets
class Character
{
public:
  /// @brief ctor sets reference to grid and initialised values
  /// @param [in] _grid pointer to the grid to reference for pathfinding
	Character(Grid *_grid);
  ~Character() = default;

  /// @brief updates character, including movement
  void update();

  /// @brief draw character with openGL
  void draw();

  /// @brief pathfinding function to get nodes for pathfinding
  void findPath();

  /// @brief calculate vector towards next point
  ngl::Vec2 calcAimVec(float *dist_squared);

  /// @brief set a new target position based on a position
  /// @param [in] _target_pos is the position to pathfind to
  void setTarget(ngl::Vec2 _target_pos);

  /// @brief set a new target based on the grid tile id
  /// @brief [in] _tile_id is the tile id to pathfind to
  void setTarget(int _tile_id);

	////////////////
	void printID();
	int getID() {return m_id;}
	///////////////

private:
	/// @brief counts how many objects have been created
	static int m_id_counter;
	/// @brief current object's id
	const int m_id;
  /// @brief grid to reference for pathfinding
  Grid *m_grid;
  /// @brief character position
  ngl::Vec2 m_pos;
  /// @brief id of target tile on grid
  int m_target_id;
  /// @brief max speed of character
  float m_speed;
  /// @brief list of target positions for movement
  std::vector<ngl::Vec2> m_path;
};

#endif//__CHARACTER_HPP__
