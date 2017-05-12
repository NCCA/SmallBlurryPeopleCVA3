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
                    STOREHOUSE
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

import random as rand
import os
cwd = os.getcwd()+ "/python/"
import sys
sys.path.append(cwd)
import gameUtils.noise.noise as n
import gameUtils.mapViewer.mapViewer as mv
import gameUtils.helperFunctions.helperFunctions as hf

#parameters from game
map_width = 100
map_height = 100
map_seed = 4
tileTypes = {"NONE": 0,
             "TREES": 1,
             "WATER": 2,
             "MOUNTAINS": 3,
             "STOREHOUSE": 4}

if "width_in" in globals():
  map_width = width_in

if "height_in" in globals():
  map_height = height_in

if "seed_in" in globals():
  map_seed = seed_in

if "tileTypes_in" in globals():
  tileTypes = tileTypes_in



#setting the seed for the noise generator
gen = n.fractalNoise(map_seed)
#setting up variables that will be passed to the game

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
    map_data[x + map_width * y][1] = hf.roundToNearest(noise, 25)/20.0
    #setting terrain types
    if noise > peak_height:
      map_data[x + map_width * y][0] = tileTypes["MOUNTAINS"]
      map_data[x + map_width * y][1] = int(gen.fractal(x, y, octaves, persistence, freq/2, min, max))/10
    elif noise > mountain_height:
      map_data[x + map_width * y][0] = tileTypes["MOUNTAINS"]
      map_data[x + map_width * y][1] = int(gen.fractal(x, y, octaves, persistence, freq/2, min, max))/12
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
px, py = hf.getRandomSpawn(map_data, map_width, map_height, tileTypes["NONE"])
map_data[px + map_width * py][0] = tileTypes["STOREHOUSE"]
map_data[px+1 + map_width * py][0] = tileTypes["HOUSE"]
spawn_point = (px, py+2)
water_height = 90/20
mountain_height = 170/20

if "from_game" not in globals():
  print "not from game"
  viewer = mv.mapViewer(map_data, tileTypes, map_width, map_height)
  viewer.display(displayType = "types")
