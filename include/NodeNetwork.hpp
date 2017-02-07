#ifndef __NODENETWORK_HPP__
#define __NODENETWORK_HPP__

#include "Node.hpp"
#include "Grid.hpp"
#include <vector>
#include "ngl/Vec2.h"
///
/// \file NodeNetwork.hpp
/// \brief The NodeNetwork can be created as a temporary helper object for finding a path.
/// all pathfinding logic is contained within it
///
///
/// \class NodeNetwork
/// \brief Wraps up a Node vector and uses it to find a path on the Grid object given in its constructor
///
class NodeNetwork
{
public:
  ///
  /// \brief NodeNetwork Constructor requires a grid and a start and end point for pathfinding
  /// \param _grid the grid used to check tile traversability
  /// \param _pos initial position of character
  /// \param _target_pos target position to find a path to
  ///
  NodeNetwork(Grid *_grid, ngl::Vec2 _pos, ngl::Vec2 _target_pos);
  ///
  /// \brief findPath returns a vector of 2d points that can be traversed
  /// \return vector of points in a path to target, or 0 length vector if no path is found
  ///
  std::vector<ngl::Vec2> findPath();
  ///
  /// \brief printNetwork used for debugging node network
  ///
  void printNetwork();
  ///
  /// \brief createFoundPath go through parent nodes and return list of positions of valid path
  /// \return vector of points in path
  ///
  std::vector<ngl::Vec2> createFoundPath(Node _end_node);
private:
  ///
  /// \brief m_grid grid which network can read for pathfinding
  ///
  Grid *m_grid;
  ///
  /// \brief m_nodes list of nodes used for pathfinding
  ///
  std::vector<Node> m_nodes;
  ///
  /// \brief m_char_pos initial position of character
  ///
  ngl::Vec2 m_char_pos;
  ///
  /// \brief m_target_pos target position of character
  ///
  ngl::Vec2 m_target_pos;
};

#endif//__NODENETWORK_HPP__
