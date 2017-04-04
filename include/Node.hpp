#ifndef __NODE_HPP__
#define __NODE_HPP__

#include "Grid.hpp"
///
/// @file Node.hpp
/// @brief Utility class used for pathfinding, holds information
/// position, cost, parent node etc
///
///
/// @enum An enum to identify node neighbours
///
enum Neighbour{
								UP,
								DOWN,
								LEFT,
								RIGHT
							 };
///
/// @class Node
/// @brief Pathfinding node class
///
class Node
{
public:
  ///
  /// \brief Node constructor used to set up position, target position and parent_id, and pointers to grid and vector of nodes
  /// \param _grid used to reference traversability and size
  /// \param nodes for referencing other nodes in the vector
  /// \param _pos the position of the node being constructed
  /// \param _target_pos the target position for calculating manhatten distance for h cost
  /// \param _parent_id the position of the parent node in the vector
  ///
  Node(Grid *_grid, std::vector<Node> *nodes, ngl::Vec2 _pos, ngl::Vec2 _target_pos, int _parent_id);
  ///
  /// \brief default destructor
  ///
  ~Node() = default;

  ///
  /// \brief close turns the node "off", after it has been fully explored
  ///
  void close();
  ///
  /// \brief manhattanDist get the x + y distance to given target
  /// \param _target_pos the target to check position against
  /// \return x dist + y dist
  ///
  float manhattanDist(ngl::Vec2 _target_pos);
  ///
  /// \brief calcNewGCost calculate a g_cost using a potential parent
  /// \param _parent_id potential parent to check against
  /// \return distance jumping node to node until reaching original position
  ///
  float calcNewGCost(int _parent_id);
  ///
  /// \brief getFCost get the node's current f cost
  /// \return node's f cost
  ///
  float getFCost();
  ///
  /// \brief isOpen check if node is still open or if it is closed
  /// \return true for open, false for closed
  ///
  bool isOpen();
  ///
  /// \brief getID get node's position in vector
  /// \return m_id, which is the position in the vector
  ///
  int getID();
  ///
  /// \brief getPos get the position of the node
  /// \return the position of the node
  ///
  ngl::Vec2 getPos();
  ///
  /// \brief getGCost get the current g cost of the node
  /// \return the current g cost of the node
  ///
  float getGCost();
  ///
  /// \brief setParent change parent of node
  /// \param _parent_id position of new parent in vector
  ///
  void setParent(int _parent_id);
  ///
  /// \brief getParentID get the parent position in vector
  /// \return parent position in vector
  ///
  int getParentID();
  ///
  /// \brief getNeighbours return list of neighbour positions in vector
  /// \return array of 4 potential neighbour positions, with -1 if not found
  ///
  std::array<int, 4> getNeighbours();

private:
  ///
  /// \brief m_grid used to reference size and traversability of grid
  ///
  Grid *m_grid;
  ///
  /// \brief m_node_vector vector of nodes which contains this one
  ///
  std::vector<Node> *m_node_vector;
  ///
  /// \brief m_id position in node vector
  ///
  int m_id;
  ///
  /// \brief m_pos node's position
  ///
  ngl::Vec2 m_pos;
  ///
  /// \brief m_parent_id position of parent in node vector
  ///
  int m_parent_id;
  ///
  /// \brief m_open if the node is fully explored or cannot be traversed, this is set to false. otherwise it is open
  ///
  bool m_open;
  ///
  /// \brief m_g_cost the cost moving up through the node's parents
  ///
  float m_g_cost;
  ///
  /// \brief m_h_cost the manhatten distance to the target position
  ///
  float m_h_cost;
};

#endif//__NODE_HPP__
