#ifndef __NODE_HPP__
#define __NODE_HPP__

#include "Grid.hpp"

/// @file Node.hpp
/// @brief Utility class used for pathfinding, holds information
/// position, cost, parent node etc

/// @enum An enum to identify node neighbours
enum Neighbour{
                UP,
                DOWN,
                LEFT,
                RIGHT
               };

/// @class Node
/// @brief Pathfinding node class
class Node
{
  public:
    Node(Grid *_grid, std::vector<Node> *nodes, int _tile_id, int _target_id, int _parent_id);
    ~Node() = default;

    void close();
    float manhattanDist(int _target_id);
    float calcGCost(std::vector<Node> *_nodes, int _parent_id);
    float getFCost();

    bool isOpen();

    int getID();

    int getTileID();
    ngl::Vec2 getPos();
    float getGCost();

    void setParent(std::vector<Node> *_nodes, int _parent_id);

    int getParentID();

    std::array<int, 4> getNeighbours(std::vector<Node> *_nodes);

  private:
    Grid *m_grid;


    int m_id;

    int m_tile_id;
    int m_target_id;
    ngl::Vec2 m_pos;

    int m_parent_id;
    bool m_open;

    float m_g_cost;
    float m_h_cost;
};

#endif//__NODE_HPP__
