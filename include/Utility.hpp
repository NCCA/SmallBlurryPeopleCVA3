#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cmath>

#include <limits>
#include <random>
#include <vector>

#include <ngl/Vec3.h>

#include <iostream>
#include <ngl/NGLStream.h>

namespace Utility
{
constexpr float floatInfinity = std::numeric_limits<float>::infinity();
constexpr float pi = atan(1.0) * 4.0;

//Given a vector of input points, computes the min and max bounds. Use it to make an AABB around a bunch of stuff.
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

float radians(const float _degrees);
float degrees(const float _radians);

float randFlt(const float _min, const float _max);
}

#endif
