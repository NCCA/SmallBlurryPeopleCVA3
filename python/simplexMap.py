from PIL import Image
import random as rand
import os
cwd = os.getcwd()+ "/python/"
import sys
sys.path.append(cwd)
import noise.noise as n

gen = n.fractalNoise(4)




map_width = 50
map_height = 50


octaves = 8
persistence = 0.4
freq = 0.03
min = 0
max = 255


peak_height = 190
mountain_height = 170
tree_band = (90, 150)
tree_density_scale = 0.5
water_height = 90


map_data = [tileTypes["NONE"] for i in range(map_width * map_height)]


for x in range(map_width):
  for y in range(map_height):
    noise = int(gen.fractal(x, y, octaves, persistence, freq, min, max))

    col = (0, 0, 0)
    if noise > peak_height:
      map_data[x + map_width * y] = tileTypes["MOUNTAINS"]
    elif noise > mountain_height:
      map_data[x + map_width * y] = tileTypes["MOUNTAINS"]
    elif noise < water_height:
      map_data[x + map_width * y] = tileTypes["WATER"]
    else:
      col = (50, 50, 200)

    if noise in range(tree_band[0], tree_band[1]):

      r = rand.random()
      bw = tree_band[1] - tree_band[0]
      bp = noise - tree_band[0]
      td = (1 - bp/float(bw)) * float(tree_density_scale)

      if r < td:
        map_data[x + map_width * y] = tileTypes["TREES"]




print "finished map gen"
