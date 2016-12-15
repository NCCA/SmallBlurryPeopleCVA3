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

import os
from PIL import Image



#current map images:
#spiral.png 50x50     green and blue
#cross.png  500x500   green only
#lines.png  50x50     green only
#omaga.png  50x50     green only
#ring.png   50x50     green only
img_filename = "maps/spiral.png"
im = Image.open(img_filename)
map_width = im.size[0]
map_height = im.size[1]

print map_width, map_height


map_data = [[0, 0] for i in range(map_width * map_height)]


for y in range(map_height):
  for x in range(map_width):
    xy = (x, y)
    rgb = im.getpixel(xy)
    r = rgb[0]
    g = rgb[1]
    b = rgb[2]
    if g == 255:
      map_data[x + map_width * y][0] = 9;
    map_data[x + map_width * y][1] = b/255.0;

