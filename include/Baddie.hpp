#ifndef __BADDIE_HPP__
#define __BADDIE_HPP__

#include "AI.hpp"

#include "ngl/Vec2.h"
#include "ngl/Vec3.h"
#include "Grid.hpp"
#include "TerrainHeightTracer.hpp"
#include "vector"

class Character;

class Baddie : public AI
{
public:
  ///
  /// \brief Baddie constructor to create baddie at edge of map
  /// \param _grid grid to use
  ///
	Baddie(TerrainHeightTracer *_height_tracer, Grid *_grid, std::vector<Character> *_characters);
  ///
  /// \brief default destructor
  ///
  ~Baddie() = default;
  ///
  /// \brief update calculate behaviours for baddie
  ///
	void update();
	///
	/// \brief trackingState, baddie intiates fight with character and follows it
	///
	void trackingState();
	///
	/// \brief invadedState, character intiates fight with baddie
	///
	void invadedState(Character *_target);
	///
	/// \brief setInvade, stop character looking for an enemy
	///
	void stopSearching() {m_search = false;}
	///
	/// \brief fight, baddies actions when fighting
	///
	void fight();
	///
	/// \brief idleState, sets baddie to not be in a fithing state
	///
	void idleState() {m_combat = false;}
	///
	/// \brief findNearestTarget, finds character nearest to baddie
	///
	bool findNearestTarget();
private:
	///
	/// \brief m_combat, if the character is in combat
	///
	bool m_combat;
	///
	/// \brief m_invaded, if the character is being attacked, wont look for other enemies
	///
	bool m_search;
	///
	/// \brief m_track, if the character is following character
	///
	bool m_track;
	///
	/// \brief m_characters, vector of characters
	///
	std::vector<Character> *m_characters;
	///
	/// \brief m_target_character, current target character
	///
	Character *m_target_character;
	///
	/// \brief m_agro_range, range a character needs to be in to target
	///
	float m_agro_range;
};

#endif//__BADDIE_HPP__
