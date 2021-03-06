#include "NodeNetwork.hpp"
#include "ngl/NGLStream.h"

NodeNetwork::NodeNetwork(Grid *_grid, ngl::Vec2 _pos, ngl::Vec2 _target_pos) :
  m_grid(_grid),
  m_char_pos((int)_pos.m_x, (int)_pos.m_y),
  m_target_pos(_target_pos)
{
  m_nodes.push_back(Node(_grid, &m_nodes, m_char_pos, m_target_pos, -1));
}

std::vector<ngl::Vec2> NodeNetwork::findPath()
{
  bool path_found = false;
  while(!path_found)
  {

    // GET LOWEST COST
    // used as pointer to lowest f cost node, but since vector might be reallocated and addresses will change, id is used
    int lowest_f_node_id = -1;

    // global open flag = false
    bool open_nodes_available = false;
    // for each node in nodes:
    for(Node &node : m_nodes)
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
        else if (node.getFCost() < m_nodes[lowest_f_node_id].getFCost())
        {
          // update lowest cost node
          lowest_f_node_id = node.getID();
        }
      }
    }

    // if no nodes are open
    if(!open_nodes_available)
    {
      break;
    }
    // GET NEIGHBOURS AND UPDATE COSTS

    // create neighbour list, (4)
    std::array<int, 4> neighbour_ids = m_nodes[lowest_f_node_id].getNeighbours();
    // check for neighbours
    // place neighbours that are not yet in the vector
    ngl::Vec2 current_pos = m_nodes[lowest_f_node_id].getPos();

    for(size_t i = 0; i < neighbour_ids.size(); i++)
    {
      // if neighbour is not found
      if(neighbour_ids[i] == -1)
      {
        ngl::Vec2 tile_pos(current_pos);
        switch(i)
        {
          case Neighbour::UP:
            if(current_pos[1] < m_grid->getH() - 1)
            {
              tile_pos += ngl::Vec2(0, 1);
            }
            break;
          case Neighbour::DOWN:
            if(current_pos[1] > 0)
            {
              tile_pos += ngl::Vec2(0, -1);
            }
            break;
          case Neighbour::LEFT:
            if(current_pos[0] > 0)
            {
              tile_pos += ngl::Vec2(-1, 0);
            }
            break;
          case Neighbour::RIGHT:
            if(current_pos[0] < m_grid->getW() - 1)
            {
              tile_pos += ngl::Vec2(1, 0);
            }
            break;
          default:
            std::cerr << "tile id not found" << std::endl;
        }
        if(tile_pos != current_pos)
        {
          neighbour_ids[i] = m_nodes.size();
          m_nodes.push_back(Node(m_grid, &m_nodes, tile_pos, m_target_pos, m_nodes[lowest_f_node_id].getID()));
        }
      }
    }

    // for each neighbour:
    for(int neighbour_id : neighbour_ids)
    {
      if(neighbour_id != -1)
      {
        Node *neighbour = &(m_nodes[neighbour_id]);
        // if open:
        if(neighbour->isOpen())
        {
          // check G cost
          float potential_g_cost = neighbour->calcNewGCost(m_nodes[lowest_f_node_id].getID());
          if(potential_g_cost < neighbour->getGCost())
          {
            // if lower, update parent to current node
            neighbour->setParent(m_nodes[lowest_f_node_id].getID());
          }
        }
      }
    }

    // CLOSE NODE
    // close current node
    m_nodes[lowest_f_node_id].close();

    // if current is target node, path is found
    if(m_nodes[lowest_f_node_id].getPos() == m_target_pos)
    {
      path_found = true;
      return createFoundPath(m_nodes[lowest_f_node_id]);
    }
  }
  // no path found if loop exits
  std::vector<ngl::Vec2> no_path;
  return no_path;
}

void NodeNetwork::printNetwork()
{
  // march through positions and if a valid node exists, print x or o, if closed or open
  for(int y=0; y<m_grid->getH(); y++)
  {
    for(int x=0; x<m_grid->getW(); x++)
    {
      Node *matching_node = nullptr;
      for(Node &node : m_nodes)
      {
        if(node.getPos().m_x == x && node.getPos().m_y == y)
        {
          matching_node = &node;
          break;
        }
      }
      if(matching_node)
      {
        if(matching_node->isOpen())
        {
          std::cout << "o ";
        }
        else
        {
          std::cout << "x ";
        }
      }
      else
      {
        std::cout << "  ";
      }
    }
    std::cout << '\n';
  }
}

std::vector<ngl::Vec2> NodeNetwork::createFoundPath(Node _end_node)
{
  std::vector<ngl::Vec2> path;
  // create path with nodes
  Node *current_node = &_end_node;
  while(true)
  {
    // stop when parent is reached
    if(current_node->getParentID() == -1)
    {
      break;
    }
    // add current position to vector and then get parent
    path.push_back(current_node->getPos() + ngl::Vec2(0.5, 0.5));
    current_node = &(m_nodes[current_node->getParentID()]);
  }
  //printPath(path);
  return path;
}

void NodeNetwork::printPath(std::vector<ngl::Vec2> &_path)
{
  // similar to print Network, go through positions and print valid path positions
  for(int y=0; y<m_grid->getH(); y++)
  {
    for(int x=0; x<m_grid->getW(); x++)
    {
      bool on_path = false;
      for(ngl::Vec2 &pos : _path)
      {
        if(pos.m_x == x && pos.m_y == y)
        {
          on_path = true;
          break;
        }
      }
      if(on_path)
      {
        std::cout << "o ";
      }
      else
      {
        if(m_grid->isTileTraversable(x, y))
        {
          std::cout << "  ";
        }
        else
        {
          std::cout << ". ";
        }
      }
    }
    std::cout << '\n';
  }
}

bool NodeNetwork::raytrace(Grid *_grid, ngl::Vec2 _start_pos, ngl::Vec2 _end_pos)
{
  _start_pos.m_x = (int)_start_pos.m_x;
  _start_pos.m_y = (int)_start_pos.m_y;
  _end_pos.m_x = (int)_end_pos.m_x;
  _end_pos.m_y = (int)_end_pos.m_y;

  int dx = abs(_end_pos.m_x - _start_pos.m_x);
  int dy = abs(_end_pos.m_y - _start_pos.m_y);
  int x = _start_pos.m_x;
  int y = _start_pos.m_y;
  int n = 1 + dx + dy;
  int x_inc = (_end_pos.m_x > _start_pos.m_x) ? 1 : -1;
  int y_inc = (_end_pos.m_y > _start_pos.m_y) ? 1 : -1;
  int error = dx - dy;
  dx *= 2;
  dy *= 2;

  for (; n > 0; --n)
  {
      if(!_grid->isTileTraversable(x, y)) return false;

      if (error > 0)
      {
          x += x_inc;
          error -= dy;
      }
      else
      {
          y += y_inc;
          error += dx;
      }
  }
  return true;
}
