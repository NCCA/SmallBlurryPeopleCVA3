import os
cwd = os.getcwd()+ "/python/"
import sys
sys.path.append(cwd)

import gameUtils.mapViewer.mapViewer as mVis
import gameUtils.defaults.defaults as defaults

tileTypes = defaults.tileTypes

gameMap = [(tileTypes["TREES"], 0) for i in range(100 * 100)]
v = mVis.mapViewer(gameMap, tileTypes, 100, 100)
v.display()

