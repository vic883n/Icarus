#!/usr/bin/python3

import csv
import sys
import numpy as np
from os import listdir
from os.path import isfile, join, abspath
from os import path

#magnometor scalling factor
def scale(num):
    return (num/6842) * 100


experimentalDataFolder = sys.argv[1]
IGRFVectorFile = sys.argv[2]

if not path.exists(experimentalDataFolder):
    print("The Data directory was not found at: " + experimentalDataFolder)
    sys.exit(-1)

print("Reading in the files in the directory")
print(experimentalDataFolder + "\n")
for file in listdir(experimentalDataFolder):
    if file[-4:] != ".csv":
        continue
    fullpath = experimentalDataFolder + "/" + file
    print(file + "\n")
    with open(fullpath, "r") as dataFiles:
        reader = csv.reader(dataFiles, delimiter=',')
        next(reader)
        for datapoint in reader:
            print(scale(int(datapoint[4])))
            magX = scale(int(datapoint[4]))
            magY = scale(int(datapoint[5]))
            magZ = scale(int(datapoint[6]))

            vector = np.array([magX, magY, magZ])
            unitVector = (vector / np.linalg.norm(vector))


