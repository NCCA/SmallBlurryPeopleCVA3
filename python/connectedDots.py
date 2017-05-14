from PIL import Image
import random as rand
import os
cwd = os.getcwd()+ "/python/"
import sys
sys.path.append(cwd)
import gameUtils.noise as n
import gameUtils.helperFunctions as hf

#parameters from game
map_width = width_in
map_height = height_in
map_seed = seed_in
tileTypes = tileTypes_in

img_filename = "resources/textures/maps/connectedDots.png"
im = Image.open(img_filename)
im = im.resize((map_width, map_height), Image.ANTIALIAS)

#seed for the noise generator
gen = n.fractalNoise(map_seed)


map_data = [[tileTypes["NONE"], 0] for i in range(map_width * map_height)]

#noise function values
octaves = 8
persistence = 0.4
freq = 0.04
min = 0
max = 200

#limits to divide the heightfield into terrain types
mountain_height = 170
tree_band = (70, 150)
tree_density_scale = 0.5
water_height = 40


height_divider = 20

half_width = map_width/2
half_height = map_height/2

max_len = hf.len(half_width, half_height)

#setting terrain types
for x in range(map_width):
  for y in range(map_height):
    noise = int(gen.fractal(x, y, octaves, persistence, freq, min, max))
    rgb = im.getpixel((x,y))
    noise *= 1 - (rgb[0]/255.0)
    noise = hf.roundToNearest(noise, 30)
    map_data[x + map_width * y][1] = noise

    #setting terrain types
    if noise > mountain_height:
      map_data[x + map_width * y][0] = tileTypes["MOUNTAINS"]
      map_data[x + map_width * y][1] = int(gen.fractal(x, y, octaves, persistence, freq, 200, 400))
    elif noise < water_height:
      map_data[x + map_width * y][0] = tileTypes["WATER"]

    #adding trees
    if int(noise) in range(tree_band[0], tree_band[1]):
      td = gen.fractal(x+100, y+100, octaves, persistence, 0.1, 0, 1)
      if 0 < (td):
        map_data[x + map_width * y][0] = tileTypes["TREES"]

#dividing heights to be passes to game
px, py = hf.getRandomSpawn(map_data, map_width, map_height, tileTypes["NONE"])
map_data[px + map_width * py][0] = tileTypes["STOREHOUSE"]
map_data[px+1 + map_width * py][0] = tileTypes["HOUSE"]
spawn_point = (px, py+2)
water_height /= height_divider
mountain_height /= height_divider

for x in range(map_width):
  for y in range(map_height):
    map_data[x + map_width * y][1]/= height_divider
