"""
  A test script for python scripting the map generation, which has
  access to the following variables:

  map_data:       An array of data that represents the map, it stores lists where the
                  first value is the ammount of trees and the second value is the
                  height of the tile
  map_width:      The width of the map, set in the main program.
                  This is set from the values in the main program and not passed back
  map_height:     The height of the map, set in the main program.
                  This is set from the values in the main program and not passed back
"""

map_width = 50
map_height = 50
map_data = [[0, 0] for i in range(map_width * map_height)]


