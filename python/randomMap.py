"""
  A test script for python scripting the map generation, which has
  access to the following variables:

  map_data:       An array of data that represents the map, it stores lists where the
                  first value is the ammount of trees and the second value is the
                  geight of the tile
  map_width:      The width of the map, set in the main program.
                  This is set from the values in the main program and not passed back
  map_height:     The height of the map, set in the main program.
                  This is set from the values in the main program and not passed back
"""

import random as rand
import math

max_rad = 10
tree_circles = 10
height_circles = 10
seed = 14


map_width = 50
map_height = 50
map_data = [[0, 0] for i in range(map_width * map_height)]

def set(x, y, t, state):
  if (0 <= x < map_width) and (0 <= y < map_height):
    map_data[x + map_width * y][t] += state


rand.seed(seed)


for i in range(tree_circles):
  x_rand = rand.randint(0, map_width)
  y_rand = rand.randint(0, map_height)
  r_rand = rand.randint(0, max_rad)

  for y in range(-y_rand, y_rand):
    for x in range(-x_rand, x_rand):
      if x*x + y*y < r_rand*r_rand:
        x_out = x + x_rand
        y_out = y + y_rand
        set(x_out, y_out, 0 ,1)




for i in range(height_circles):
  x_rand = rand.randint(0, map_width)
  y_rand = rand.randint(0, map_height)
  r_rand = rand.randint(0, max_rad)

  for y in range(-y_rand, y_rand):
    for x in range(-x_rand, x_rand):
      if x*x + y*y < r_rand*r_rand:
        height = 255 - (math.sqrt(x*x + y*y)/r_rand) * 255.0
        x_out = x + x_rand
        y_out = y + y_rand
        set(x_out, y_out, 1, int(height))




