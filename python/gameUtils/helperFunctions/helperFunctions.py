import math

def roundToNearest(x, base):
	return int(base * round(float(x)/base))

def len(x, y):
	return math.sqrt(x * x + y * y)