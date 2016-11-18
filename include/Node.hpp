#ifndef __NODE_HPP__
#define __NODE_HPP__

#include "Grid.hpp"

/// @file Node.hpp
/// @brief Utility class used for pathfinding, holds information
/// position, cost, parent node etc

/// @class Node
/// @brief Pathfinding node class
class Node
{
  public:
    Node(Grid *_grid, int _tile_id, int _target_id, Node *_parent);
    ~Node() = default;

    void close();
    float manhattanDist(int _target_id);
    float calcGCost();
    float getFCost();

    bool isOpen();

    int getTileID();
    ngl::Vec2 getPos();
    float getGCost();

  private:
    Grid *m_grid;
    int m_tile_id;
    ngl::Vec2 m_pos;
    Node *m_parent;
    bool m_open;

    float m_g_cost;
    float m_h_cost;
};

#endif//__NODE_HPP__
