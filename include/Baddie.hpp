#ifndef __BADDIE_HPP__
#define __BADDIE_HPP__

#include "ngl/Vec2.h"
#include "ngl/Vec3.h"
#include "Grid.hpp"
#include <vector>
#include "Character.hpp"

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
  void update(const std::vector<Character> &m_characters);
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
	///
	/// \brief findPath find path to given position
	/// \param _target position to aim for
	///
	void findPath(ngl::Vec2 _target);
	///
	/// \brief getPos return current pos
	/// \return m_pos as a vec3 using m_pos.m_x, height and m_pos.m_y
	///
	ngl::Vec3 getPos();
	///
	/// \brief updateRot update character's rotations
	///
	void updateRot();
	///
	/// \brief getRot get character's rotation
	/// \return m_rot (degrees)
	///
	float getRot() {return m_rot;}
	///
	/// \brief targetBest set target to the character it can find a path to
	/// \param _characters vector of characters
	/// \return true if path found
	///
	bool targetBest(const std::vector<Character> &_characters);
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
  ///
  /// \brief m_rot rotation in degrees of baddie
  ///
  float m_rot;
};

#endif//__BADDIE_HPP__
