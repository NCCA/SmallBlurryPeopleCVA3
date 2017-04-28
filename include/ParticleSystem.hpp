#ifndef PARTICLESYSTEM_HPP
#define PARTICLESYSTEM_HPP

#include <vector>

#include <ngl/Vec3.h>

//We store arrays of positions, velocities and other attributes, rather than creating a particle class.
struct ParticleSystem
{
    std::vector<ngl::Vec3> m_pos;
    std::vector<ngl::Vec3> m_vel;
    std::vector<float> m_scale;
    std::vector<float> m_time;
    std::vector<float> m_alpha;

    size_t size() const {return m_pos.size();}
};

#endif
