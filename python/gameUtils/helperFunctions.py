import math
import random as rand

def roundToNearest(x, base):
	return int(base * round(float(x)/base))

def len(x, y):
	return math.sqrt(x * x + y * y)

def isClearArea(map_data, map_width, x, y, radius, empty_type):
	for i in range(x - radius, x + radius + 1):
		for j in range(y - radius, y + radius + 1):
			print i, j
			if map_data[i + map_width * j][0] != empty_type:
				return False
	return True

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
