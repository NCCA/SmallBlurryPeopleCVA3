#include "Baddie.hpp"
#include "NodeNetwork.hpp"
#include "Prefs.hpp"
#include "ngl/NGLStream.h"
#include "Utility.hpp"

Baddie::Baddie(TerrainHeightTracer *_height_tracer, Grid *_grid) :
	AI(_height_tracer, _grid),
	m_combat(false)
{
  m_target_id = m_grid->coordToId(m_pos);
  Prefs* prefs = Prefs::instance();
	m_speed = prefs->getFloatPref("CHARACTER_SPEED") * 0.1;

	setTarget(ngl::Vec2(10, 10));
}

void Baddie::update()
{
	if(m_combat == false)
	{
		if(move())
		{
				setTarget(ngl::Vec2(Utility::randInt(0, 50), Utility::randInt(0,50)));
			//newRandomTarget();
		}
	}
	else
	{
		setTarget(m_pos);
	}

  //setTarget(target);

  //std::cout << "-Baddie-" << std::endl;
  //std::cout << "pos:" << m_pos << std::endl;
}

void Baddie::fightState()
{
	//to be filled with stuff later
	m_combat = true;
}

