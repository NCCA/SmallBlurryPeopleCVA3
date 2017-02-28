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

from PIL import Image
import os
cwd = os.getcwd()+ "/python/"
import sys
sys.path.append(cwd)
import noise.noise



#current map images:
#spiral.png 50x50     green and blue
#cross.png  500x500   green only
#lines.png  50x50     green only
#omaga.png  50x50     green only
#ring.png   50x50     green only

print "none: ", tileTypes["NONE"]
print "trees: ", tileTypes["TREES"]
print "water: ", tileTypes["WATER"]
print "mountains: ", tileTypes["MOUNTAINS"]
print "house: ", tileTypes["HOUSE"]
print "storehouse: ", tileTypes["STOREHOUSE"]

img_filename = "maps/spiral.png"
im = Image.open(img_filename)
map_width = im.size[0]
map_height = im.size[1]

print map_width, map_height


map_data = [tileTypes["NONE"] for i in range(map_width * map_height)]


for y in range(map_height):
  for x in range(map_width):
    xy = (x, y)
    rgb = im.getpixel(xy)
    r = rgb[0]
    g = rgb[1]
    b = rgb[2]
    if g == 255:
      map_data[x + map_width * y] = tileTypes["TREES"]

map_data[map_width/2 + map_width * map_height/2] = tileTypes["STOREHOUSE"]


