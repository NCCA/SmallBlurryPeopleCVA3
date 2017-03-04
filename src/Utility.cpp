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

float randFlt(const float _low, const float _high)
{
    if( _low == _high )
        return _low;

    std::uniform_real_distribution< float > uni( _low, _high );
    return uni( randomDevice );
}

float average(std::vector<float> _values)
{
    float total = std::accumulate( _values.begin(), _values.end(), 0.0f );
    return total / static_cast<float>(_values.size());
}

}
