#ifndef __AI_HPP__
#define __AI_HPP__

#include "Grid.hpp"
#include "TerrainHeightTracer.hpp"
#include "ngl/Vec2.h"

#include <QTime>
#include <vector>
#include <SDL.h>
#include<iostream>

/// @file AI.hpp
/// @brief The AI refers to the grid for pathfinding and keeps track of a target
/// for pathfinding.

/// @class AI
/// @brief Parent class for ingame characters and enemies, containing position, states and targets
class AI
{
public:
	///
	/// @brief ctor, sets reference to grid and initialised values
	/// @param [in] _grid, pointer to the grid to reference for pathfinding
	/// @param [in] _height_tracer, pointer to TerrainHeightTracer to get the height of the AI
	/// at its position
	///
	AI(TerrainHeightTracer *_height_tracer, Grid *_grid);
  ///
	/// @brief destructor
	///
	~AI() = default;
	///
	/// @brief idleState, randomly moves AI while not active
	///
	virtual void idleState() = 0;
	///
	/// @brief update, updates AI based on its current state
	///
	virtual void update() = 0;
	///
	/// @brief move, moves AI along its path
  /// @return a boolean determing if its reached its target
	///
	bool move();
	///
	/// @brief findPath, pathfinding function to get nodes for pathfinding
	/// @param [in] _target, coordinate to path find to
	///
	void findPath(ngl::Vec2 _target);
	///
	/// @brief findPath, pathfinding function to get nodes for pathfinding
	/// @param [in] _target_id, tile to path find to
	///
	void findPath(int _target_id);
	///
	/// @brief getPath, path finds to a tile and returns the path
	/// @param [in] _target_id, tile to path find to
	/// @return vector of vec2's that store the path to the target tile
	///
	std::vector<ngl::Vec2> getPath(int _target_id);
	///
	/// @brief calcAimVec, calculate vector towards next point
  /// @return vec2 of direction vector character is aiming towards
	///
	ngl::Vec2 calcAimVec(float *dist);
	///
	/// @brief setTarget, set a new target position based on a position
	/// @param [in] _target_pos, the position to pathfind to
	/// @return a boolean determining whether the target is viable
	///
	bool setTarget(ngl::Vec2 _target_pos);
	///
	/// @brief setTarget, set a new target based on the grid tile id
	/// @param [in] _tile_id, the tile id to pathfind to
	/// @return a boolean determining whether the target is viable
	///
	bool setTarget(int _tile_id);
	///
	/// @brief gethealth, get AI's health
	/// @return m_health, AI's health value
	///
	float getHealth() {return m_health;}
	///
	/// @brief takeHealth, take health away from a AI
	/// @param m_amount, amount of health to take away
	///
	void takeHealth(float m_amount) {m_health -= m_amount;}
	///
	/// @brief getPos, get AI's position
	/// @return m_pos, AI's position
	///
	ngl::Vec3 getPos();
	///
  /// @brief getPos2D, get 2D position
  /// @return m_pos, AI's position
	///
  ngl::Vec2 getPos2D();
	///
  /// @brief updateRot update m_rot, eg if direction has changed
	///
	void updateRot();
	///
	/// @brief getRot return rotation of AI
	/// @return m_rot, character's rotation
	///
	float getRot() {return m_rot;}
	///
	/// @brief isIdle, returns whether the AI is idle
	/// @return m_idle, the boolean stored in the AI determining if it is idle or not
	///
	bool isIdle() {return m_idle;}

protected:
  ///
  /// @brief m_grid, grid pointer to reference for pathfinding
  ///
  Grid *m_grid;
  ///
  /// @brief m_height_tracer, for grid height
  ///
  TerrainHeightTracer *m_height_tracer;
  ///
  /// @brief m_pos, AI's position
  /// ///
  ngl::Vec2 m_pos;
  ///
  /// @brief m_offset, offset AI on current tile, to avoid clipping between AIs
  ///
  float m_offset;
  ///
  /// @brief m_target_id, id of target tile on grid
  ///
  int m_target_id;
  ///
  /// @brief m_health, how much health the character has: 1 = full health, 0 = no health/dead
  ///
  float m_health;
  ///
  /// @brief m_rot character's rotation to face current direction (degrees)
  ///
  float m_rot;
  ///
  /// @brief m_speed, max speed of character
  ///
  float m_speed;
  ///
  /// @brief m_path, vector of target positions for movement
  ///
  std::vector<ngl::Vec2> m_path;
  ///
  /// @brief m_action_timer, timer for characters actions such as chopping wood and building
  ///
  QTime m_action_timer;
  ///
  /// @brief m_idle, set when character is idle, for checking if idle
  ///
  bool m_idle;
};

#endif//__CHARACTER_HPP__
