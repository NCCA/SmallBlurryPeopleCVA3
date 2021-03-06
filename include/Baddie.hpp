#ifndef __BADDIE_HPP__
#define __BADDIE_HPP__

#include "AI.hpp"

#include "ngl/Vec2.h"
#include "Grid.hpp"
#include "TerrainHeightTracer.hpp"

#include <vector>

/// @file Baddie.hpp
/// @brief The enemy class that wanders around searching for characters, when one comes into range
///  it follows and attacks the character


class Character;

/// @class Baddie
/// @brief The Baddie class is the ingame enemy, with attacking and tracking states
///
class Baddie : public AI
{
public:
  ///
  /// \brief Baddie create baddie with a position on the grid and a references to the characters
  /// \param _pos spawn position
  /// \param _height_tracer height tracer to use
  /// \param _grid pointer to grid for pathfinding and positions
  /// \param _characters reference to character vector in scene
  ///
  Baddie(ngl::Vec2 _pos, TerrainHeightTracer *_height_tracer, Grid *_grid, std::vector<Character> *_characters);
  ///
  /// @brief update calculate behaviours for baddie
  ///
  void update();
  ///
  /// \brief trackingState, baddie intiates fight with character and follows it
  ///
  void trackingState();
  ///
  /// \brief invadedState, character intiates fight with baddie
  ///
  void invadedState(int _target);
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
  ///
  /// \brief getScale, returns baddie's scale for drawing
  /// \return m_scale, scale of baddie
  ///
  float getScale() {return m_scale;}
  ///
  /// \brief addScale, adds onto scale of baddie
  /// \param _scale, amount to add onto scale
  ///
  void addScale(float _scale) {m_scale += _scale;}
  ///
  /// \brief getID, return baddie ID
  /// \return m_id, baddies ID
  ///
  float getID() {return m_id;}
private:
  ///
  /// @brief m_id_counter, counts how many baddies have been created
  ///
  static int m_id_counter;
  ///
  /// \brief m_id, baddies ID
  ///
  int m_id;
  ///
  /// \brief m_characters, vector of characters
  ///
  std::vector<Character> *m_characters;
  ///
  /// \brief m_targets, current character target's
  ///
  std::vector<int> m_targets;
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
  /// \brief m_attack_power, how much the baddie attacks for
  ///
  float m_attack_power;
  ///
  /// \brief m_agro_distance, distance a character can move where the baddie will stop pursuing
  ///
  float m_agro_distance;
  ///
  /// \brief m_tracking_distance, range a character needs to be in to target
  ///
  float m_tracking_distance;
  ///
  /// \brief m_scale, baddie grows in size when it kills enemies
  ///
  float m_scale;
};

#endif//__BADDIE_HPP__
