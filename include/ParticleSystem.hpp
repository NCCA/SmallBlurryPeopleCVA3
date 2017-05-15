#ifndef PARTICLESYSTEM_HPP
#define PARTICLESYSTEM_HPP

#include <vector>

#include <ngl/Vec3.h>

/// @file ParticleSystem.hpp
/// @brief An extemely barebones particle effect struct. We forgo creating a singular particle class, instead opting to have vectors
/// of attributes.

struct ParticleSystem
{
    /// @brief The position of each particle.
    std::vector<ngl::Vec3> m_pos;

    /// @brief The velocity of each particle.
    std::vector<ngl::Vec3> m_vel;

    /// @brief The scale of each particle.
    std::vector<float> m_scale;

    /// @brief The time this particle has been alive for. I use it to animate the cloud particles.
    std::vector<float> m_time;

    /// @brief The opacity of each particle.
    std::vector<float> m_alpha;

    /// @brief The size of the particle system.
    size_t size() const {return m_pos.size();}
};

#endif
