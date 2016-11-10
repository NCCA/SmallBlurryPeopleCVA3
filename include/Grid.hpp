#ifndef __GRID_HPP__
#define __GRID_HPP__

#include <vector>

enum class Tile{EMPTY, FOREST, BUILDING};

class Grid
{
public:
    Grid();
    Grid(int _w, int _h);
    ~Grid() = default;
    void printMap();
private:
    void init();
    int m_w;
    int m_h;
    std::vector<Tile> m_map;
};

#endif//__GRID_HPP__
