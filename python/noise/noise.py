import random as rand
import math

class fractalNoise:
  def __init__(self, _seed):
    #set random engine seed
    rand.seed(_seed)

    #permutation table
    self.p = [x for x in range(255)]
    rand.shuffle(self.p)
    #gradient table
    self.grad3 = [[1,1,0],[-1,1,0],[1,-1,0],[-1,-1,0],
                 [1,0,1],[-1,0,1],[1,0,-1],[-1,0,-1],
                 [0,1,1],[0,-1,1],[0,1,-1],[0,-1,-1]]

  def dot(self, a, x, y):
    """
    custom dot product function used by the simplexNoise function
    a:        a gradient vector
    x:        x component of second vector
    y:        y component of second vector
    return:   the dot product of a and (x, y)
    """
    return a[0] * x + a[1] * y

  def simplex(self, x_in, y_in):
    """
    computes values of 2d simplex noise for given positions
    x_in:     the x component of the position
    y_in:     the y component of the position
    return:   the value of the simplex noise at (x_in, y_in)
    """
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

    gi0 = self.p[(ii + self.p[jj%255])%255] % 12
    gi1 = self.p[(ii + i1 + self.p[(jj + j1)%255])%255] % 12
    gi2 = self.p[(ii + 1 + self.p[(jj + 1)%255])%255] % 12

    t0 = 0.5 - x0 * x0 - y0 * y0
    if t0 < 0:
      n0 = 0.0
    else:
      t0 *= t0
      n0 = t0 * t0 * self.dot(self.grad3[gi0], x0, y0)

    t1 = 0.5 - x1 * x1 - y1 * y1
    if t1 < 0:
      n1 = 0.0
    else:
      t1 *= t1
      n1 = t1 * t1 * self.dot(self.grad3[gi1], x1, y1)

    t2 = 0.5 - x2 * x2 - y2 * y2
    if t2 < 0:
      n2 = 0.0
    else:
      t2 *= t2
      n2 = t2 * t2 * self.dot(self.grad3[gi2], x2, y2)

    return 70.0 * (n0 + n1 + n2)

  def fractal(self, x, y, octaves, persistence, scale, low, high):
    """
    computes 2d fractal noise values using layered simplex noise
    x:          x component of position
    y:          y component of position
    octaves:    number of layers of noise
    persistence:how much the weight of each layer is reduced by
    scale:      initial frequency
    low:        minimum value of output
    high:       maximum value of output
    return:     fractal noise value for the position (x, y) scaled to the range low-high
    """
    max_amp = 0
    amp = 1
    freq = scale
    noise = 0

    for i in range(octaves):
      noise += self.simplex(x * freq, y * freq) * amp
      max_amp += amp
      amp *= persistence
      freq *= 2

    noise /= max_amp
    return noise * (high - low)/2 + (high + low)/2
