from PIL import Image

class mapViewer:
	def __init__(self, _map, _dict, _w, _h):
		self.map = _map
		self.h = _h
		self.w = _w
		self.dict = _dict
		self.setColours()

	def setColours(
		    self,
			default = (50, 200, 50),
			tree = (30, 100, 30),
			mountain = (100, 100, 100),
			peak = (200, 200, 200),
			water = (80, 80, 200)
		):
		self.default = default
		self.tree = tree
		self.mountain = mountain
		self.peak = peak;
		self.water = water;

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