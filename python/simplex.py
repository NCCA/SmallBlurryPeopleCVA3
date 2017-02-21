from PIL import Image
import random as rand
import math

#building the hash table
rand.seed(100)
p = [x for x in range(255)]
rand.shuffle(p)

#gradient table
grad3 = [[1,1,0],[-1,1,0],[1,-1,0],[-1,-1,0],
    [1,0,1],[-1,0,1],[1,0,-1],[-1,0,-1],
    [0,1,1],[0,-1,1],[0,1,-1],[0,-1,-1]]

#custom dot product function
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

  gi0 = p[(ii +  p[jj%255])%255] % 12
  gi1 = p[(ii + i1 + p[(jj + j1)%255])%255] % 12
  gi2 = p[(ii + 1 + p[(jj + 1)%255])%255] % 12

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

def fractalNoise(x, y, freq, octaves, freq_scale):
  noise_out = 0
  for i in range(octaves):
    noise_out += simplexNoise(x * freq, y * freq)
    freq /= freq_scale
  return noise_out

def fractalSimlex(x, y, octaves, persistence, scale, low, high):
  max_amp = 0
  amp = 1
  freq = scale
  noise = 0

  for i in range(octaves):
    noise += simplexNoise(x * freq, y * freq) * amp
    max_amp += amp
    amp *= persistence
    freq *= 2

  noise /= max_amp
  return noise * (high - low)/2 + (high + low)/2



width = 50
height = 50

freq = 0.02

print min(p)
print max(p)

im = Image.new("RGB", (width, height), "grey")
peak_height = 190
mountain_height = 170
tree_band = (90, 150)
tree_density_scale = 0.5
water_height = 90
#im.getpixel((0, 0))
#im.putpixel((0, 0), (255, 0, 0))


#map_data = [tileTypes["NONE"] for i in range(width * height)]


for x in range(width):
  for y in range(height):
    #noise = int(255 * (1 + simplexNoise(x * freq, y * freq)) * 0.5)
    noise = int(fractalSimlex(x, y, 8, 0.4, 0.03, 0, 255))

    col = (0, 0, 0)
    if noise > peak_height:
      col = (255, 255, 255)
    elif noise > mountain_height:
      col = (150, 150, 150)
    elif noise > water_height:
      col = (80, 200, 50)
    else:
      col = (50, 50, 200)

    if noise in range(tree_band[0], tree_band[1]):

      r = rand.random()
      bw = tree_band[1] - tree_band[0]
      bp = noise - tree_band[0]
      td = (1 - bp/float(bw)) * float(tree_density_scale)

      if r < td:
        col = (50, 120, 20)
    im.putpixel((x, y), col)







im.show()
