import gameUtils.noise.noise as n
import gameUtils.mapViewer.mapViewer as mv
import gameUtils.helperFunctions.helperFunctions as hf
import random as rand

map_width = 100
map_height = 100
tileTypes = {"NONE": 0,
             "TREES": 1,
             "WATER": 2,
             "MOUNTAINS": 3,
             "STOREHOUSE": 4}
map_data = [[tileTypes["NONE"], 0] for i in range(map_width * map_height)]


octaves = 8
persistence = 0.33
freq = 0.03
min = 0
max = 255
gen = n.fractalNoise(4)

#setting some limits to divide the heightfield into terrain types
peak_height = 190
mountain_height = 170
tree_band = (90, 150)
tree_density_scale = 0.5
water_height = 90

#setting terrain types
for x in range(map_width):
  for y in range(map_height):
    #noise = gen.simplex(x * 1, y * 1)
    noise = int(gen.fractal(x, y, octaves, persistence, freq, min, max))
    map_data[x + map_width * y][1] = noise #hf.roundToNearest(noise, 25)
    #setting terrain types
    if noise > mountain_height:
      map_data[x + map_width * y][0] = tileTypes["MOUNTAINS"]
      map_data[x + map_width * y][1] = int(gen.fractal(x, y, octaves, 0.2, freq, mountain_height, 255))
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

if "from_game" not in globals():
  print "not from game"
  viewer = mv.mapViewer(map_data, tileTypes, map_width, map_height)
  viewer.display(displayType = "types")
