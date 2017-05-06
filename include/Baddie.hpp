#ifndef __BADDIE_HPP__
#define __BADDIE_HPP__

#include "AI.hpp"

#include "ngl/Vec2.h"
#include "ngl/Vec3.h"
#include "Grid.hpp"
#include "TerrainHeightTracer.hpp"
#include "vector"

class Baddie : public AI
{
public:
  ///
  /// \brief Baddie constructor to create baddie at edge of map
  /// \param _grid grid to use
  ///
  Baddie(TerrainHeightTracer *_height_tracer, Grid *_grid);
  ///
  /// \brief default destructor
  ///
  ~Baddie() = default;
  ///
  /// \brief update calculate behaviours for baddie
  ///
	void update();
	///
	/// \brief setFightState, sets baddie into a fighting state
	///
	void fightState();
	///
	/// \brief idleState, sets baddie to not be in a fithing state
	///
	void idleState() {m_combat = false;}
private:
	///
	/// \brief m_combat, if the character is in combat
	///
	bool m_combat;
};

#endif//__BADDIE_HPP__
