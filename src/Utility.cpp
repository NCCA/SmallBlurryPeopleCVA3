#include <random>

#include "Utility.hpp"

namespace Utility
{

std::default_random_engine randomDevice = std::default_random_engine ((std::random_device())());

float radians(const float _degrees)
{
    return _degrees / 180.0f * pi;
}

float degrees(const float _radians)
{
    return _radians / pi * 180.0f;
}

float randFlt(const float _min, const float _max)
{
		if( _min == _max )
				return _min;

		std::uniform_real_distribution< float > uni( _min, _max );
    return uni( randomDevice );
}

int randInt(const int _min, const int _max)
{
		if( _min == _max )
				return _min;

		std::uniform_int_distribution<int> uni( _min, _max );
		return uni( randomDevice );
}

float average(std::vector<float> _values)
{
    float total = std::accumulate( _values.begin(), _values.end(), 0.0f );
    return total / static_cast<float>(_values.size());
}

float clamp(float _in, float _lo, float _hi)
{
		if(_in < _lo)
				return _lo;
		else if(_in > _hi)
				return _hi;
		return _in;
}

bool pointInBox(std::pair<ngl::Vec3, ngl::Vec3> _box, ngl::Vec3 _pt)
{
    return _pt.m_x > _box.first.m_x and
            _pt.m_y > _box.first.m_y and
            _pt.m_z > _box.first.m_z and
            _pt.m_x < _box.second.m_x and
            _pt.m_y < _box.second.m_y and
            _pt.m_z < _box.second.m_z;
}

}
