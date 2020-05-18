#!/usr/bin/python3

import csv
import sys
from os import listdir
from os.path import isfile, join, abspath
from os import path

filePath = sys.argv[1]
if not path.exists(filePath):
    print("The Data directory was not found at: " + filePath)
    sys.exit(-1)

print("Reading in the files in the directory")
print(filePath + "\n")
for file in listdir(filePath):
    if file[-4:] != ".csv":
        continue
    fullpath = filePath + "/" + file
    print(file + "\n")
    with open(fullpath, "r") as dataFiles:
        spamreader = csv.reader(dataFiles, delimiter=',')
        for row in spamreader:
            print(', '.join(row))

