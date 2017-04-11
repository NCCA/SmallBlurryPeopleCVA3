#ifndef __BADDIE_HPP__
#define __BADDIE_HPP__

#include "ngl/Vec2.h"
#include "ngl/Vec3.h"
#include "Grid.hpp"
#include "vector"

class Baddie
{
public:
  ///
  /// \brief Baddie constructor to create baddie at edge of map
  /// \param _grid grid to use
  ///
  Baddie(Grid *_grid);
  ///
  /// \brief default destructor
  ///
  ~Baddie() = default;
  ///
  /// \brief update calculate behaviours for baddie
  ///
  void update(ngl::Vec3 closest_Target);
  ///
  /// \brief move, moves baddie along its path
  /// \return true if baddie reaches target
  ///
  bool move();
  ///
  /// \brief calcAimVec, calculate vector towards next point
  /// \return vector to target
  ///
  ngl::Vec2 calcAimVec(float *dist_squared);

	///
	/// \brief setTarget, set a new target position based on a position
	/// \param _target_pos, the position to pathfind to
	///
	bool setTarget(ngl::Vec2 _target_pos);

	void findPath(ngl::Vec2 _target);

	ngl::Vec3 getPos();
private:
  ///
  /// \brief m_pos position of baddie
  ///
  ngl::Vec2 m_pos;
  ///
  /// \brief m_grid pointer to grid baddie is on
  ///
  Grid *m_grid;
  ///
  /// \brief m_health float between 0..1 for health value
  ///
  float m_health;
  ///
  /// \brief m_speed speed of character
  ///
  float m_speed;
  ///
  /// \brief m_path, vector of target positions for movement
  ///
  std::vector<ngl::Vec2> m_path;
  ///
  /// \brief m_target_id target tile id
  ///
  int m_target_id;
};

#endif//__BADDIE_HPP__
