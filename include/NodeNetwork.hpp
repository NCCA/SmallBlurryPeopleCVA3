#ifndef __NODENETWORK_HPP__
#define __NODENETWORK_HPP__

#include "Node.hpp"
#include "Grid.hpp"
#include <vector>
#include "ngl/Vec2.h"

/// @file NodeNetwork.hpp
/// @brief The NodeNetwork can be created as a temporary helper object for finding a path.
/// all pathfinding logic is contained within it

/// @class NodeNetwork
/// @brief Wraps up a Node vector and uses it to find a path on the Grid object given in its constructor

class NodeNetwork
{
public:
  /// @brief Constructor requires a grid and a start and end point for pathfinding
  /// @param [in] _grid the grid used to check tile traversability
  /// @param [in] _pos initial position of character
  /// @param [in] _target_pos target position to find a path to
  NodeNetwork(Grid *_grid, ngl::Vec2 _pos, ngl::Vec2 _target_pos);

  /// @brief findPath returns a vector of 2d points that can be traversed
  /// @return vector of points in a path to target, or 0 length vector if no path is found
  std::vector<ngl::Vec2> findPath();

  void printNetwork();
private:
  /// @brief grid which network can read for pathfinding
  Grid *m_grid;
  /// @brief list of nodes used for pathfinding
  std::vector<Node> m_nodes;
  /// @brief initial position of character
  ngl::Vec2 m_char_pos;
  /// @brief target position of character
  ngl::Vec2 m_target_pos;
};

#endif//__NODENETWORK_HPP__
