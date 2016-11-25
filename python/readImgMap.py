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


#for tile in map_data:
#  map_data[tile] = tile_empty

import os
from PIL import Image


img_filename = "maps/cross2.png"
im = Image.open(img_filename)
map_width = im.size[0]
map_height = im.size[1]

print map_width, map_height


map_data = [tile_empty for i in range(map_width * map_height)]


for y in range(map_height):
  for x in range(map_width):
    xy = (x, y)
    rgb = im.getpixel(xy)
    if rgb == (0, 255, 0):
      map_data[x + map_width * y] = tile_forest

