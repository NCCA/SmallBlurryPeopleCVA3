"""
  A test script for python scripting the map generation, which has
  access to the following variables:

  map_data:       A one dimensional array that mirrors the std::vector in the c++ program
                  The data from c++ is coppied to the script at the start, and then coppied back at the end of the script
  map_width:      The width of the map, set in the main program.
                  This is set from the values in the main program and not passed back
  map_height:     The height of the map, set in the main program.
                  This is set from the values in the main program and not passed back
  tile_error
  tile_empty:     Value to be given to an empty tile.
  tile_forest:    Value to be given to a forest tile
  tile_building:  Value to be given to a building tile
"""

import random as rand

max_rad = 5
num_circles = 20
seed = 14


def set(x, y, state):
  if (0 <= x < map_width) and (0 <= y < map_height):
    map_data[x + map_width * y] = state

choices = [tile_empty, tile_forest, tile_building]

rand.seed(seed)


for i in range(num_circles):
  x_rand = rand.randint(0, map_width)
  y_rand = rand.randint(0, map_height)
  r_rand = rand.randint(0, max_rad)
  t_rand = rand.choice(choices)
  for y in range(-y_rand, y_rand):
    for x in range(-x_rand, x_rand):
      if x*x + y*y < r_rand*r_rand:
        x_out = x + x_rand
        y_out = y + y_rand
        set(x_out, y_out, t_rand)


