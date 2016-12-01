#include "Character.hpp"

#include "Node.hpp"

#include "ngl/Random.h"
#include "ngl/NGLStream.h"

#include <iostream>
#include <vector>
#include <array>
#include <QTime>

Character::Character(Grid *_grid):
  m_grid(_grid)
{
  ngl::Random *rand = ngl::Random::instance();
  rand->setSeed(0);

  //m_pos = ngl::Vec2(1, 4);
  m_pos = ngl::Vec2(16, 47);
  m_target_id = m_grid->coordToId(m_pos);

  //setTarget(ngl::Vec2(9, 1));
  setTarget(ngl::Vec2(20, 20));

  std::cout << "character created" << std::endl;
  std::cout << "character target: " << m_target_id << std::endl;
  update();
}

void Character::update()
{
  std::cout << "character updated" << std::endl;
  draw();
}

void Character::draw()
{
  std::cout << "character position: " << m_pos << std::endl;
  std::cout << "character drawn" << std::endl;
}

void Character::findPath()
{
  QTime myTimer;
  myTimer.start();

  std::vector<Node> nodes;
  nodes.push_back(Node(m_grid, &nodes, m_grid->coordToId(m_pos), m_target_id, -1));

  bool path_found = false;
  while(!path_found)
  {
    // pointer to current node
    int lowest_f_node_id = -1;

    // global open flag = false
    bool open_nodes_available = false;
    // for each node in nodes:
    for(Node &node : nodes)
    {
      // if open:
      if(node.isOpen())
      {
        open_nodes_available = true;
        // compare node cost to current lowest
        if(lowest_f_node_id == -1)
        {
          lowest_f_node_id = node.getID();
        }
        else if (node.getFCost() < nodes[lowest_f_node_id].getFCost())
        {
          // update lowest cost node
          lowest_f_node_id = node.getID();
        }
      }
    }

    // if no nodes are open
    if(!open_nodes_available)
    {
      std::cout << "character position" << m_pos << std::endl;
      std::cout << "character target" << m_grid->idToCoord(m_target_id) << std::endl;
      std::cout << "no path found!" << std::endl;
      break;
    }

    // create neighbour list, (4)
    std::array<int, 4> neighbour_ids = nodes[lowest_f_node_id].getNeighbours(&nodes);
    // check for neighbours
    // place neighbours that are not yet in the vector
    int current_tile_id = nodes[lowest_f_node_id].getTileID();
    ngl::Vec2 current_pos = m_grid->idToCoord(current_tile_id);

    for(size_t i = 0; i < neighbour_ids.size(); i++)
    {
      // if neighbour is not found
      if(neighbour_ids[i] == -1)
      {
        int tile_id = -1;
        switch(i)
        {
          case Neighbour::UP:
            if(current_pos[1] > 0)
            {
              tile_id = current_tile_id - m_grid->getW();
            }
            break;
          case Neighbour::DOWN:
            if(current_pos[1] < m_grid->getH() - 1)
            {
              tile_id = current_tile_id + m_grid->getW();
            }
            break;
          case Neighbour::LEFT:
            if(current_pos[0] > 0)
            {
              tile_id = current_tile_id - 1;
            }
            break;
          case Neighbour::RIGHT:
            if(current_pos[0] < m_grid->getW() - 1)
            {
              tile_id = current_tile_id + 1;
            }
            break;
          default:
            std::cerr << "tile id not found" << std::endl;
        }
        if(tile_id != -1)
        {
          neighbour_ids[i] = nodes.size();
          nodes.push_back(Node(m_grid, &nodes, tile_id, m_target_id, nodes[lowest_f_node_id].getID()));
        }
      }
    }
    // for each neighbour:
    for(int neighbour_id : neighbour_ids)
    {
      if(neighbour_id != -1)
      {
        Node *neighbour = &(nodes[neighbour_id]);
        // if open:
        if(neighbour->isOpen())
        {
          // check G cost
          if(neighbour->calcGCost(&nodes, nodes[lowest_f_node_id].getID()) < neighbour->getGCost())
          {
            // if lower, update parent to current node
            neighbour->setParent(&nodes, nodes[lowest_f_node_id].getID());
            //std::cout << neighbour->getParentID() << std::endl;
          }
        }
      }
    }

    // close current node
    nodes[lowest_f_node_id].close();

    // if current is target node, path is found
    if(nodes[lowest_f_node_id].getTileID() == m_target_id)
    {
      path_found = true;
      // create path with nodes
      //std::cout << "character position" << m_pos << std::endl;
      //std::cout << "character target" << m_grid->idToCoord(m_target_id) << std::endl;
      //std::cout << "path found!" << std::endl;
      Node *current_node = &(nodes[lowest_f_node_id]);
      while(true)
      {
        m_path.push_back(current_node->getTileID());
        if(current_node->getParentID() == -1)
        {
          break;
        }
        current_node = &(nodes[current_node->getParentID()]);
      }

      int nMilliseconds = myTimer.elapsed();
      std::cout << "time taken" << nMilliseconds << std::endl;
    }
  }

  std::cout << "nodes" << std::endl;

  for(int y=0; y<m_grid->getH(); y++)
  {
    for(int x=0; x<m_grid->getW(); x++)
    {
      int node_id = -1;
      for(Node &node : nodes)
      {
        if(node.getPos()[0] == x && node.getPos()[1] == y)
        {
          node_id = node.getID();
        }
      }

      if(node_id == -1)
      {
        std::cout << "   ";
      }
      else
      {

        int node_tile = nodes[node_id].getTileID();
        if(node_tile == m_target_id || node_tile == m_grid->coordToId(m_pos))
        {
          if(nodes[node_id].isOpen()) std::cout << "!!";
          else std::cout << "@@";
          std::cout << "@";
        }
        else if(m_grid->read(node_tile) != Tile::EMPTY)
        {
          std::cout << "###";
        }
        else
        {
          if(nodes[node_id].getGCost() < 10)
          {
            std::cout << "_";
          }
          if(nodes[node_id].getGCost() < 100)
          {
            if(nodes[node_id].isOpen())
            {
              std::cout << "o";
            }
            else
            {
              std::cout << "x";
            }
          }
          std::cout << nodes[node_id].getGCost();
        }
        //std::cout << "_" << nodes[node_id].getParentID();
      }
    }
    std::cout << std::endl;
  }
  for(int y=0; y<m_grid->getH(); y++)
  {
    for(int x=0; x<m_grid->getW(); x++)
    {
      for(int id: m_path)
      {
        if(m_grid->coordToId(ngl::Vec2(x, y)) == id)
        {
          std::cout << "o";
        }
        else
        {
          std::cout << " ";
        }
      }
    }
    std::cout << std::endl;
  }

  m_grid->print2();

}

void Character::setTarget(ngl::Vec2 _target_pos)
{
  // convert to tile id
  setTarget(m_grid->coordToId(_target_pos));
}

void Character::setTarget(int _tile_id)
{
  if(_tile_id != m_target_id)
  {
    m_target_id = _tile_id;
    findPath();
  }
}
