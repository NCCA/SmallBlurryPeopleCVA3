from PIL import Image

## @package mapViewer
#  implementation of the MapViewer class that can be used to
#  show generated maps as an image without having to run the
#  entire game. Used in early development of maps

## The map viewer class
class mapViewer:

  ## __init__ is the ctor  that reads in all the data and sets colours
  #  for the different types
  #
  #  @param self is the instance of the class
  #  @param _map is the map data to be read to produce the image
  #  @param _dict is the dictionary that maps key strigs to integers
  #  that indicate tile types
  #  @param _w is the width of the map
  #  @param _h is the height of the map
  def __init__(self, _map, _dict, _w, _h):
    self.map = _map
    self.h = _h
    self.w = _w
    self.dict = _dict
    self.setColours()

  ## setColours sets the colours that different map elements will be
  #  drawn in
  #
  #  @param self is the instance of the class
  #  @param default is the colour for empty tiles
  #  @param tree is the colour for tree tiles
  #  @param mountain is the colour for mountain tiles
  #  @param water is the colour for water tiles
  def setColours(self,
                 default = (50, 200, 50),
                 tree = (30, 100, 30),
                 mountain = (100, 100, 100),
                 water = (80, 80, 200)
                 ):
    self.default = default
    self.tree = tree
    self.mountain = mountain
    self.peak = peak;
    self.water = water;

  ## display shows the map as an image on the screen using the python
  #  image libraray where 1 tile = 1 pixel
  #
  #  @param self is the instance of the class
  #  @param displayType indicated what is to be drawn ("types", or "height")
  def display(self, displayType = "types"):
    im = Image.new("RGB", (self.w, self.h))
    pix = im.load()
    for x in range(self.w):
      for y in range(self.h):
        col = (0, 0, 0)

        if displayType == "types":
          tile_type = self.map[x + self.w * y][0]
          if tile_type == self.dict["NONE"]:
            col = self.default
          elif tile_type == self.dict["TREES"]:
            col = self.tree
          elif tile_type == self.dict["WATER"]:
            col = self.water
          elif tile_type == self.dict["MOUNTAINS"]:
            col = self.mountain
        elif displayType == "height":
          height = self.map[x + self.w * y][1]
          col = (height, 0, 0)
        pix[x, y] = col
    im.show()
