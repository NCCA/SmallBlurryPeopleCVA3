#include "AI.hpp"

#include "Node.hpp"
#include "NodeNetwork.hpp"
#include "Utility.hpp"


AI::AI(TerrainHeightTracer *_height_tracer, Grid *_grid):
	m_grid(_grid),
	m_height_tracer(_height_tracer),
	m_idle(true),
	m_health(1.0),
	m_speed(0.0),
	m_rot(0.0)
{
	m_action_timer.start();
	m_pos = ngl::Vec2(0.0,0.0);
}

bool AI::move()
{
	// check whether the next point is necessary or if character has line of sight to the second point in list
	if(m_path.size() > 1 && NodeNetwork::raytrace(m_grid, m_pos, m_path.end()[-2]))
	{
		// if character can see point after next, remove the next point so character heads directly to the furtherst one they can see
		// done on a one by one basis so it does all happen in the same frame as pathfinding, wouldn't be super slow that way but marginally better this way
		m_path.pop_back();
	}

	// move by distance up to speed
	float dist_moved = 0;
	while(m_path.size() > 0 && dist_moved < m_speed)
	{
		float dist = 1;
		ngl::Vec2 aim_vec = calcAimVec(&dist);
		if(dist < m_speed)
		{
			aim_vec *= dist;
			m_path.pop_back();
		}
		else
		{
			aim_vec *= m_speed;
		}
		dist_moved += aim_vec.length();
		m_pos += aim_vec;
	}
	updateRot();
	if(m_path.size() <= 0)
	{
		return true;
	}
	else return false;
}

void AI::findPath(ngl::Vec2 _target)
{
	NodeNetwork network(m_grid, m_pos, _target);
	m_path = network.findPath();
}

void AI::findPath(int _target_id)
{
	NodeNetwork network(m_grid, m_pos, m_grid->idToCoord(_target_id));
	m_path = network.findPath();
}

std::vector<ngl::Vec2> AI::getPath(int _target_id)
{
	NodeNetwork network(m_grid, m_pos, m_grid->idToCoord(_target_id));
	return network.findPath();
}

ngl::Vec2 AI::calcAimVec(float *dist)
{
	ngl::Vec2 aim_vec(0,0);
	if(m_path.size() > 0)
	{
	 aim_vec = m_path.back() - m_pos;
	}
	if(dist)
	{
		*dist = aim_vec.length();
	}
	if(aim_vec != ngl::Vec2(0,0))
	{
		aim_vec.normalize();
	}
	return aim_vec;
}

bool AI::setTarget(ngl::Vec2 _target_pos)
{
	setTarget(m_grid->coordToId(_target_pos));
}

bool AI::setTarget(int _tile_id)
{
	//if the chosen tile isnt equal to the target and isnt equal to the character's pos

	if(m_target_id != _tile_id && _tile_id != m_grid->coordToId(m_pos))
	{
		m_target_id = _tile_id;
		findPath(_tile_id);
	}
	//if no path was found, return false

	if(m_path.size() <= 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

ngl::Vec3 AI::getPos()
{
	//get grid height at AI's position
	float height = m_height_tracer->getHeight(m_pos[0], m_pos[1]);
	return ngl::Vec3(m_pos[0], height, m_pos[1]);
}

void AI::updateRot()
{
	if(m_path.size() > 0)
	{
		ngl::Vec2 dir = m_path.back() - m_pos;
		m_rot = Utility::degrees(atan2(dir.m_x, dir.m_y));
	}
}
