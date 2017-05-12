import math
import random as rand

## @package helperFunctions
#  This module contains functions that I found useful when writing 
#  the maps that ship with the game and I thought would be useful 
#  to other people writing their own custom maps

## roundToNearest rounds the value x to the nearest multiple of base
#  It is used to produce the tiered look seen in the maps
#
#  @param x is the value to be rounded
#  @param base is the value that x is rounded to a multiple of
#  @return the rounded value
def roundToNearest(x, base):
  return int(base * round(float(x)/base))

## len returns the length of a 2d vector
#  @param x is the x component of the vector
#  @param y is the y component of the vector
def len(x, y):
  return math.sqrt(x * x + y * y)

## isClearArea indicates if the tiles within a give radius of a 
#  tile are all empty. This is used to find an empty area on a map
#  to designate as the starting area
#
#  @param map_data is the list of tiles that have been set by the script
#  @param map_width is the width of the map
#  @param x is the x coordinate of the center tile
#  @param y is the y coordinate of the center tile
#  @param radius is the search radius for the desured clear area
#  @param empty_type is a value that indicated if a tile is empty
#  @return is true if the area around x, y is clear, fals otherwise
def isClearArea(map_data, map_width, x, y, radius, empty_type):
  for i in range(x - radius, x + radius + 1):
    for j in range(y - radius, y + radius + 1):
      print i, j
      if map_data[i + map_width * j][0] != empty_type:
        return False
  return True

## getRandomSpawn produces a random spawn point to be passed to the
#  c++ program. The spawn point is checked to ensure it is clear in
#  a 
def getRandomSpawn(map_data, width, height, empty_type):
  found = False
  x = 0
  y = 0
  while not found:
    x = rand.randrange(width)
    y = rand.randrange(height)
    if isClearArea(map_data, width, x, y, 1, empty_type):
      found = True
  print "found ", x, y
  return (x, y)
