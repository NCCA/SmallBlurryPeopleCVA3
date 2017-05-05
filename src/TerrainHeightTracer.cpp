#include <iostream>
#include "ngl/NGLStream.h"
#include "ngl/Util.h"
#include "TerrainHeightTracer.hpp"

TerrainHeightTracer::TerrainHeightTracer(std::vector<ngl::Vec4> _trimesh):
  m_trimesh(_trimesh)
{
  m_normesh.clear();
  m_d.clear();
  for (size_t i = 0; i < m_trimesh.size(); i += 3)
  {
    ngl::Vec3 v0(m_trimesh[i].m_x, m_trimesh[i].m_y, m_trimesh[i].m_z);
    ngl::Vec3 v1(m_trimesh[i+1].m_x, m_trimesh[i+1].m_y, m_trimesh[i+1].m_z);
    ngl::Vec3 v2(m_trimesh[i+2].m_x, m_trimesh[i+2].m_y, m_trimesh[i+2].m_z);


    ngl::Vec3 n = (ngl::calcNormal(v0, v1, v2) * -1);
    n.normalize();
    float d = (n.dot(v0)) * -1;


    m_normesh.push_back(n);
    m_d.push_back(d);
  }

}







float TerrainHeightTracer::getHeight(double _x, double _y)
{
  //construct ray
  ngl::Vec3 origin(_x, 0, _y);
  ngl::Vec3 dir(0, 1.0, 0);

  for (size_t i = 0; i < m_trimesh.size(); i+= 3)
  {
    //raytrace between ray and triangle
    float denominator = m_normesh[i/3].dot(dir);


    float numerator = m_d[i/3] + m_normesh[i/3].dot(origin);
    float t = -numerator/denominator;

    ngl::Vec3 p(_x, t, _y);

    float u0 = p.m_z - m_trimesh[i].m_z;
    float v0 = p.m_x - m_trimesh[i].m_x;

    float u1 = m_trimesh[i+1].m_z - m_trimesh[i].m_z;
    float v1 = m_trimesh[i+1].m_x - m_trimesh[i].m_x;

    float u2 = m_trimesh[i+2].m_z - m_trimesh[i].m_z;
    float v2 = m_trimesh[i+2].m_x - m_trimesh[i].m_x;

    float alpha = -1;
    float beta = -1;

    if (u1 == 0)
    {
      beta = u0/u2;
      if ((beta >= 0) & (beta <= 1))
      {
        alpha = (v0 - beta * v2)/v1;
      }
    }
    else
    {
      beta = (v0 * u1 - u0 * v1) / (v2 * u1 - u2 * v1);
      if ((beta >= 0) & (beta <= 1))
      {
        alpha = (u0 - beta * u2)/u1;
      }
    }
    if ((alpha >= 0) & (beta >= 0) & ((alpha + beta) <= 1))
    {
      return t;
    }

  }
  return 0;
}



