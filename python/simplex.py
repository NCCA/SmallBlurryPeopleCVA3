from PIL import Image
import math

perm = [(i&255) for i in range(512)]
print perm

grad3 = [[1,1,0],[-1,1,0],[1,-1,0],[-1,-1,0],
 		[1,0,1],[-1,0,1],[1,0,-1],[-1,0,-1],
 		[0,1,1],[0,-1,1],[0,1,-1],[0,-1,-1]]

def dot(a, x, y):
	return a[0] * x + a[1] * y


def simplexNoise(x_in, y_in):
	#vairables to store noise contributions from the three corners
	n0 = .0;
	n1 = .0;
	n2 = .0;

	#skew the input coordinates
	f2 = 0.5 * (math.sqrt(3) - 1)
	s = (x_in + y_in) * f2
	i = math.floor(x_in + s)
	j = math.floor(y_in + s)

	g2 = (3.0 - math.sqrt(3))/6.0
	t = (i + j) * g2
	x0 = x_in - (i - t)
	y0 = y_in - (j - t)

	#determine which simplex we are in
	i1 = 0
	j1 = 0

	if(x0 > y0):
		i1 = 1
	else:
		j1 = 1

	x1 = x0 - i1 + g2
	y1 = y0 - j1 + g2
	x2 = x0 - 1.0 + 2 * g2
	y2 = y0 - 1.0 + 2 * g2

	ii = int(i) & 255
	jj = int(j) & 255

	gi0 = perm[ii + perm[jj]] % 12
	gi1 = perm[ii + i1 + perm[jj + j1]] % 12
	gi2 = perm[ii + 1 + perm[jj + 1]] % 12

	t0 = 0.5 - x0 * x0 - y0 * y0
	if t0 < 0:
		n0 = 0.0
	else:
		t0 *= t0
		n0 = t0 * t0 * dot(grad3[gi0], x0, y0)

	t1 = 0.5 - x1 * x1 - y1 * y1
	if t1 < 0:
		n1 = 0.0
	else:
		t1 *= t1
		n1 = t1 * t1 * dot(grad3[gi1], x1, y1)

	t2 = 0.5 - x2 * x2 - y2 * y2
	if t2 < 0:
		n2 = 0.0
	else:
		t2 *= t2
		n2 = t2 * t2 * dot(grad3[gi2], x2, y2)

	return 70.0 * (n0 + n1 + n2)


width = 1080
height = 1080

freq = 0.005


im = Image.new("RGB", (width, height), "grey")
#im.getpixel((0, 0))
#im.putpixel((0, 0), (255, 0, 0))

for x in range(width):
	for y in range(height):
		noise = (1 + simplexNoise(x * freq, y * freq))/2
		#print noise
		im.putpixel((x, y), (int(noise * 255), 0, 0))





im.show()