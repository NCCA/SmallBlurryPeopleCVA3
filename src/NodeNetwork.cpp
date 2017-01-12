#include "NodeNetwork.hpp"

NodeNetwork::NodeNetwork(Grid *_grid, ngl::Vec2 _pos, ngl::Vec2 _target_pos) :
  m_grid(_grid),
  m_char_pos(_pos),
  m_target_pos(_target_pos)
{

}

std::vector<ngl::Vec2> NodeNetwork::findPath()
{
  std::vector<Node> nodes;
  bool path_found = false;
  while(!path_found)
  {

    /// FUNCTION: GET LOWEST COST
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
      std::cout << "no path found!" << std::endl;
      std::vector<ngl::Vec2> no_path;
      return no_path;
    }

    ///FUNCTION END

    ///FUNCTION GET NEIGHBOURS AND UPDATE COSTS

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
          nodes.push_back(Node(m_grid, &nodes, tile_id, m_grid->coordToId(m_target_pos), nodes[lowest_f_node_id].getID()));
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

    /// CLOSE NODE
    // close current node
    nodes[lowest_f_node_id].close();


    // if current is target node, path is found
    if(nodes[lowest_f_node_id].getTileID() == m_grid->coordToId(m_target_pos))
    {
      ///FUNCTION CREATE PATH
      std::vector<ngl::Vec2> path;
      path_found = true;
      // create path with nodes
      //std::cout << "character position" << m_pos << std::endl;
      //std::cout << "character target" << m_grid->idToCoord(m_target_id) << std::endl;
      //std::cout << "path found!" << std::endl;
      Node *current_node = &(nodes[lowest_f_node_id]);
      while(true)
      {
        path.push_back(current_node->getPos());
        if(current_node->getParentID() == -1)
        {
          break;
        }
        current_node = &(nodes[current_node->getParentID()]);
      }
      return path;
    }
  }
}
