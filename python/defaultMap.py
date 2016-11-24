"""
  A test script for python scripting the map generation, which has
  access to the following variables:

  map_data:       A one dimensional array that mirrors the std::vector in the c++ program
                  The data from c++ is coppied to the script at the start, and then coppied back at the end of the script
  map_width:      The width of the map, set in the main program.
                  This is set from the values in the main program and not passed back
  map_width:      The width of the map, set in the main program.
                  This is set from the values in the main program and not passed back
  tile_error
  tile_empty:     Value to be given to an empty tile.
  tile_forest:    Value to be given to a forest tile
  tile_building:  Value to be given to a building tile
"""

for index, val in enumerate(map_data):
  map_data[index] = tile_empty



