import random as rand
import os
cwd = os.getcwd()+ "/python/"
import sys
sys.path.append(cwd)
import gameUtils.noise as n
import gameUtils.helperFunctions as hf


map_width = width_in
map_height = height_in
map_seed = seed_in
tileTypes = tileTypes_in



#setting the seed for the noise generator
gen = n.fractalNoise(map_seed)
#setting up variables that will be passed to the game

map_data = [[tileTypes["NONE"], 0] for i in range(map_width * map_height)]
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

height_divider = 20

#setting terrain types
for x in range(map_width):
  for y in range(map_height):
    noise = int(gen.fractal(x, y, octaves, persistence, freq, min, max))
    map_data[x + map_width * y][1] = hf.roundToNearest(noise, 25)/height_divider
    #setting terrain types
    if noise > peak_height:
      map_data[x + map_width * y][0] = tileTypes["MOUNTAINS"]
      map_data[x + map_width * y][1] = int(gen.fractal(x, y, octaves, persistence, freq/2, min, max))/(height_divider/2)
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
water_height = 90/height_divider
mountain_height = 170/height_divider

