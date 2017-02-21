from PIL import Image
import random as rand

import noise.noise as n

gen = n.fractalNoise(4)



width = 50
height = 50

freq = 0.02


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
    noise = int(gen.fractal(x, y, 8, 0.4, 0.03, 0, 255))

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
