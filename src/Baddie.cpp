#include "Baddie.hpp"
#include "Character.hpp"

#include "NodeNetwork.hpp"
#include "Prefs.hpp"
#include "Gui.hpp"
#include "Utility.hpp"

int Baddie::m_id_counter(1);

Baddie::Baddie(ngl::Vec2 _pos, TerrainHeightTracer *_height_tracer, Grid *_grid, std::vector<Character> *_characters) :
	AI(_height_tracer, _grid),
  m_id(m_id_counter++),
	m_characters(_characters),
	m_combat(false),
	m_search(true),
  m_track(false),
	m_scale(2.0)
{
  //setting attributes from preferences
  Prefs* prefs = Prefs::instance();
	m_speed = prefs->getFloatPref("CHARACTER_SPEED") * 0.1;
  m_attack_power = prefs->getFloatPref("BADDIE_ATTACK");
  m_tracking_distance = prefs->getFloatPref("BADDIE_TRACKING");
  m_agro_distance = prefs->getFloatPref("BADDIE_AGRO_DIST");

  //setting spawn point
	m_pos = _pos;
	m_target_id = m_grid->coordToId(m_pos);
}

void Baddie::update()
{
  //for if the prefernces have been changed
  Prefs* prefs = Prefs::instance();
  m_speed = prefs->getFloatPref("CHARACTER_SPEED") * 0.1;

  //precaution but shouldn't be called
	if(m_health <= 0.0)
		m_targets.clear();

	//following enemy to attack
	if(m_track)
		trackingState();

	//if enemy and baddie are on same tile, attack
	else if(m_combat)
		fight();

	else
	{
		//if not in combat, look for neabry characters
		if(m_search)
			findNearestTarget();
		if (move())
			{
				//if no characters found, make baddie move to random position
				setTarget(ngl::Vec2(Utility::randInt(0,m_grid->getW()), Utility::randInt(0, m_grid->getH())));
			}
	}
}

void Baddie::invadedState(Character *_target)
{
	//if a character attacks, add it to the target list
	m_targets.push_back(_target);
	//set the character to be ready to fight
	m_combat = true;
}

void Baddie::trackingState()
{
  //baddie speeds up to get to enemy
  m_speed *= 10;
	//get the target's position and move the baddie to it if they're not on the same grid tile
  ngl::Vec2 target = m_targets[0]->getPos2D();
	int charID = m_grid->coordToId(target);
	if(m_grid->coordToId(m_pos) == charID)
	{
		//if reached enemy character
    m_targets[0]->invadedState(m_id);
		//reset baddie
		m_track = false;
		m_combat = true;

		setTarget(ngl::Vec2(Utility::randInt(0,m_grid->getW()), Utility::randInt(0, m_grid->getH())));
	}
	else
		//go to character target
		setTarget(charID);
		move();
}

void Baddie::fight()
{

	float distance = m_agro_distance;
  size_t target = -1;
	bool chase = false;

	//go through every target
  for(size_t i=0; i<m_targets.size(); i++)
	{
    if(m_targets[i]->isSleeping())
			m_targets.erase(m_targets.begin() + i);

    ngl::Vec2 target_coord = m_targets[i]->getPos2D();
		//if the baddies tile position isnt the same as the targets grid position
		if(m_grid->coordToId(m_pos) != m_grid->coordToId(target_coord))
		{
			float char_distance = Utility::sqrDistance(m_pos, target_coord);
			//if character is outside baddie's range
			if(char_distance > m_agro_distance)
			{
				//remove character from vector of targets
				Gui::instance()->notify(m_targets[i]->getName() + " has gotten away", m_pos);
				m_targets.erase(m_targets.begin() + i);

			}
			//finds the closest target to baddie
			else if (char_distance < distance)
			{
				distance = char_distance;
				target = i;
				chase = true;
			}
		}
	}

	//delete all targets that arent the chase target
	if(m_targets.size() > 1 && chase)
	{
    for(size_t i=0; i<m_targets.size(); i++)
		{
      if(i != target)
			{
				m_targets.erase(m_targets.begin() + i);
			}
		}
	}

	//follows target
	if(chase)
	{
    ngl::Vec2 target_coord = m_targets[0]->getPos2D();
		setTarget(target_coord);
		move();
	}

	//if theres no targets left, reset baddie
	if(m_targets.size() == 0)
	{
		m_combat = false;
		m_search = true;
	}

	//if not chasing a target, attack targets
	else if(!chase)
	{
		int target_no = (int)m_targets.size();
		if(m_action_timer.elapsed() >= 1000)
		{
      for (size_t i=0; i<m_targets.size(); i++)
			{
				//amount of health taken scales with how many characters are attacking
				m_targets[i]->takeHealth(m_attack_power/target_no);
				//if a target dies, remove from vector
				if(m_targets[i]->getHealth() <= 0.0)
        {
					m_targets.erase(m_targets.begin() + i);
          //scale up when kill character
          addScale(0.5f);
        }
			}
			m_action_timer.restart();
		}
	}

}

bool Baddie::findNearestTarget()
{
	bool found = false;
	float shortest_dist = m_tracking_distance;
	for(auto &character : *m_characters)
	{
		//find squared distance between baddie and characters
    ngl::Vec2 char_pos = character.getPos2D();
		float distance = Utility::sqrDistance(m_pos, char_pos);
		//find closest character within agro range
    if (distance < shortest_dist && !character.isInside())
		{
			found = true;
			shortest_dist = distance;
			//set found character as target
			m_targets.push_back(&character);
		}
	}

	if (found)
	{
		//baddie is set to follow enemy
		m_track = true;
		//set baddie's target to character enemy
    ngl::Vec2 target = m_targets[0]->getPos2D();
		setTarget(target);
	}
	return found;
}

