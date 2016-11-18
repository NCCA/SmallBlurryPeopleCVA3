import random as rand
r = rand.randrange(10)
print "generating random number: ", r
i += r

#grid = [0 for i in range(map_width * map_height)]

for index, val in enumerate(grid):
  grid[index] = index

for y in range(map_height):
  for x in range(map_width):
    print grid[x + map_width * y],
  print

