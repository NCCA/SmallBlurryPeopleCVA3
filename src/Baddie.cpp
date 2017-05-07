#include "Baddie.hpp"
#include "Character.hpp"

#include "NodeNetwork.hpp"
#include "Prefs.hpp"
#include "ngl/NGLStream.h"
#include "Utility.hpp"

Baddie::Baddie(TerrainHeightTracer *_height_tracer, Grid *_grid, std::vector<Character> *_characters) :
	AI(_height_tracer, _grid),
	m_characters(_characters),
	m_target_character(nullptr),
	m_combat(false),
	m_track(false),
	m_search(true),
	//** should probably be set in preferences **//
	m_agro_range(5.0)
{
  m_target_id = m_grid->coordToId(m_pos);
  Prefs* prefs = Prefs::instance();
	m_speed = prefs->getFloatPref("CHARACTER_SPEED") * 0.1;

	//** should be in set preferences **//
	m_attack_power = 0.1;

	setTarget(ngl::Vec2(10, 10));
}

void Baddie::update()
{
	//following enemy to attack
	if(m_track)
		trackingState();

	//if enemy and baddie are on same tile, attack
	else if(m_combat)
		fight();

	else
	{
		if(m_search)
			findNearestTarget();
		if (move())
		{
			setTarget(ngl::Vec2(Utility::randInt(0,m_grid->getW()), Utility::randInt(0, m_grid->getH())));
		}
	}
}

void Baddie::invadedState(Character *_target)
{
	//baddie is being attacked
	//** to be filled with stuff later **/
	//** might want a count of how many enemies? **//
	//** should target be vector? **/

	m_target_character = _target;
	m_combat = true;

}

void Baddie::trackingState()
{
	//** Should have check on range so baddie doesn't follow character everywhere **/

	ngl::Vec2 target = ngl::Vec2(m_target_character->getPos()[0], m_target_character->getPos()[2]);
	int charID = m_grid->coordToId(target);
	if(m_grid->coordToId(m_pos) == charID)
	{
		//if reached enemy character
		m_target_character->invadedState(this);
		m_track = false;
		m_combat = true;
	}
	else
		setTarget(charID);
		move();
}

void Baddie::fight()
{
	if(m_action_timer.elapsed() >= 1000)
	{
		m_target_character->takeHealth(m_attack_power);

		if(m_target_character->getHealth() <= 0.0)
		{
			m_combat = false;
			m_search = true;
		}
		m_action_timer.restart();
	}
}

bool Baddie::findNearestTarget()
{
	bool found = false;
	float shortest_dist = m_agro_range;
	for(auto &character : *m_characters)
	{
		//find squared distance between baddie and characters
		ngl::Vec2 char_pos = ngl::Vec2(character.getPos()[0], character.getPos()[2]);
		float distance = Utility::sqrDistance(m_pos, char_pos);
		//find closest character within agro range
		if (distance < shortest_dist)
		{
			found = true;
			shortest_dist = distance;
			//set found character as target
			m_target_character = &character;
		}
	}

	if (found)
	{
		//baddie is set to follow enemy
		m_track = true;
		//baddie speeds up to get to enemy
		m_speed = 0.1;
		//set baddie's target to character enemy
		ngl::Vec2 target = ngl::Vec2(m_target_character->getPos()[0], m_target_character->getPos()[2]);
		setTarget(target);
	}
	return found;
}

