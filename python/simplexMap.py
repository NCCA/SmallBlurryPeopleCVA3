"""
A user can write custom scripts for the game to use to generate maps. To do this they
must define the 3 following variables:

  map_data:       An array of integers that set the types of the map tiles in game.
                  To allow the setting of specific types, a dictionary is passed from
                  the c++ program that links strings with the tile type integers. The
                  dictionary is called tileTypes and stoes the following strings:
                    NONE
                    TREES
                    WATER
                    MOUNTAINS
                    HOUSE
                    STOEREHOUSE
                  To set one of these values the following syntax is used:
                    $map_data[x + map_width * y] = tileTypes["TREES"]

  map_width:      The width of the map, set in the main program.
                  This is set from the values in the main program and not passed back

  map_height:     The height of the map, set in the main program.
                  This is set from the values in the main program and not passed back


A python module for producing fractal simple noise is also provided and can be imported
using the following lines:
  $import os
  $cwd = os.getcwd()+ "/python/"
  $import sys
  $sys.path.append(cwd)
  $import noise.noise as n
The preamble is required because the module is stoeed as part of the game, which is not
on the PYTHONPATH variable
"""

#importing modules
from PIL import Image
import random as rand
import os
cwd = os.getcwd()+ "/python/"
import sys
sys.path.append(cwd)
import utils.noise.noise as n

#setting the seed for the noise generator
gen = n.fractalNoise(seed_in)
#setting up variables that will be passed to the game
map_width = width_in
map_height = height_in
map_data = [[tileTypes["NONE"], 0] for i in range(map_width * map_height)]
print map_data[0]
#setting up noise function values
octaves = 8
persistence = 0.4
freq = 0.03
min = 0
max = 255

#setting some limits to divide the heightfield into terrain types
peak_height = 190
mountain_height = 170
tree_band = (90, 150)
tree_density_scale = 0.5
water_height = 90

#setting terrain types
for x in range(map_width):
  for y in range(map_height):
    noise = int(gen.fractal(x, y, octaves, persistence, freq, min, max))
    map_data[x + map_width * y][1] = noise
    #setting terrain types
    if noise > peak_height:
      map_data[x + map_width * y][0] = tileTypes["MOUNTAINS"]
    elif noise > mountain_height:
      map_data[x + map_width * y][0] = tileTypes["MOUNTAINS"]
    elif noise < water_height:
      map_data[x + map_width * y][0] = tileTypes["WATER"]

    #adding trees
    if noise in range(tree_band[0], tree_band[1]):
      r = rand.random()
      bw = tree_band[1] - tree_band[0]
      bp = noise - tree_band[0]
      td = (1 - bp/float(bw)) * float(tree_density_scale)
      if r < td:
        map_data[x + map_width * y][0] = tileTypes["TREES"]

#scattering some random stoerehouses
for i in range(10):
  #wtf is going on here
  rand.seed(i)
  x_rand = rand.randint(0, map_width)
  y_rand = rand.randint(0, map_height)
  if (map_data[x_rand + map_width * y_rand][0] != tileTypes["WATER"]) and (map_data[x_rand + map_width * y_rand][0] != tileTypes["MOUNTAINS"]):
    map_data[x_rand + map_width * y_rand][0] = tileTypes["STOREHOUSE"]

print map_data[0]
