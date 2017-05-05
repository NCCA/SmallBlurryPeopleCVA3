#ifndef __TERRAINHEIGHTTRACER_HPP__
#define __TERRAINHEIGHTTRACER_HPP__

#include <vector>
#include "ngl/Vec4.h"

class TerrainHeightTracer
{
public:
  TerrainHeightTracer() = default;
  TerrainHeightTracer(std::vector<ngl::Vec4> _trimesh);
  float getHeight(double _x, double _y);
private:
  std::vector<ngl::Vec4> m_trimesh;
  std::vector<ngl::Vec3> m_normesh;
  std::vector<float> m_d;
};


#endif//__TERRAINHEIGHTTRACER_HPP__
