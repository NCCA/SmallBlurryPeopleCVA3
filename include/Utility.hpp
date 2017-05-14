#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cmath>

#include <limits>
#include <random>
#include <vector>
#include <set>

#include <ngl/Vec2.h>
#include <ngl/Vec3.h>

#include <iostream>
#include <ngl/NGLStream.h>

namespace Utility
{
constexpr float floatInfinity = std::numeric_limits<float>::infinity();
constexpr float pi = atan(1.0) * 4.0;

//Given a vector of input points, computes the min and max bounds. Use it to make an AABB around a bunch of stuff
template<class T>
std::pair<ngl::Vec3, ngl::Vec3> enclose(const T &_pts)
{
    //Min = first, max = second
    std::pair<ngl::Vec3, ngl::Vec3> bounds;
    bounds.first = ngl::Vec3(
                floatInfinity,
                floatInfinity,
                floatInfinity
                );
    bounds.second = ngl::Vec3(
                -floatInfinity,
                -floatInfinity,
                -floatInfinity
                );

    for(auto &pt : _pts)
    {
        bounds.first.m_x = std::min(bounds.first.m_x, pt.m_x);
        bounds.first.m_y = std::min(bounds.first.m_y, pt.m_y);
        bounds.first.m_z = std::min(bounds.first.m_z, pt.m_z);

        bounds.second.m_x = std::max(bounds.second.m_x, pt.m_x);
        bounds.second.m_y = std::max(bounds.second.m_y, pt.m_y);
        bounds.second.m_z = std::max(bounds.second.m_z, pt.m_z);
    }
    return bounds;
}
///
/// \brief radians
/// \param _degrees
/// \return 
///
float radians(const float _degrees);
///
/// \brief degrees
/// \param _radians
/// \return 
///
float degrees(const float _radians);
///
/// \brief randFlt, returns a random float within the given range
/// \param _min, minimum range
/// \param _max, maximum range
/// \return the random float generated
///
float randFlt(const float _min, const float _max);
///
/// \brief randInt, return a random integer within the given range
/// \param _min, minimum range
/// \param _max, maximum range
/// \return  the random integer generated
///
int randInt(const int _min, const int _max);
///
/// \brief average
/// \param _values
/// \return 
///
float average(std::vector<float> _values);
float clamp(float _in, float _lo, float _hi);
///
/// \brief sqrDistance, finds the distance squared between two vectors
/// \param _a, the first vector 
/// \param _b, the second vector
/// \return the squared distance between the two vectors
///
float sqrDistance(ngl::Vec2 _a, ngl::Vec2 _b);
///
/// \brief remap01, remaps a value to be between 0 and 1
/// \param _in_value, the value to remap
/// \param _max_in_range, the old minimum range the value was created from
/// \param _min_in_range, the old maximum range the value was created from
/// \return the value remapped between 0 and 1
///
float remap01(float _in_value, float _max_in_range, float _min_in_range);

int findSetElement(std::set<int> _set, int index);

template<typename T>
T average(std::vector<T> _values)
{
    T total = std::accumulate( _values.begin(), _values.end(), T() );
    return total / static_cast<float>(_values.size());
}

template<typename T>
T clamp(T _in, T _lo, T _hi)
{
    if(_in < _lo)
        return _lo;
    if(_in > _hi)
        return _hi;
    return _in;
}

bool pointInBox(std::pair<ngl::Vec3, ngl::Vec3> _box, ngl::Vec3 _pt);

bool boxIntersectBox(std::pair<ngl::Vec3, ngl::Vec3> _a, std::pair<ngl::Vec3, ngl::Vec3> _b);

///
/// \brief getMousePos get the mouse position
/// \return vec2 with x,y position of mouse
///
ngl::Vec2 getMousePos();

template<typename T>
T Sqr(T _x)
{
    return _x * _x;
}
}

#endif
