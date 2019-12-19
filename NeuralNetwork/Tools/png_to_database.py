 
# This scripts converts png images to a database file that can be used by the
# neural networks. Usage:
# python png_to_database.py source_folder destination_file [nb_labels]
# The labels must be in the files names, separated from the rest of the file
# name by an underscore (ex: 9_05493.png is a 9)

import os
import sys
import cv2

# Gets the program arguments
if len(sys.argv) < 3 or len(sys.argv) > 4:
    raise Exception("Usage: python png_to_database.py source_folder destination_file [nb_labels (calculated by default)]")
source_folder = sys.argv[1]
destination_file = sys.argv[2]
nb_labels = -1
if len(sys.argv) == 4: nb_labels = sys.argv[3]

# Gets the name of all the files in the source folder
print("Fiding png files...")
fileNames = []
for f in os.listdir(source_folder):
    if(os.path.isfile(source_folder + os.sep + f)):
        fileNames.append(f)
print("Found " + str(len(fileNames)) + " images")

# Gets the possible labels
print("Finding the labels...")
labels = []
for f in fileNames:
    label = f[:f.index('_')]
    if label not in labels:
        labels.append(label)
labels.sort()

# Gets the number of labels if not given
if(nb_labels == -1):
    nb_labels = len(labels)
    print("nb_labels = " + str(nb_labels))

# Gets the number of inputs
print("Calculating the number of inputs...")
sampleimg = cv2.imread(source_folder + os.sep + fileNames[0])
nb_inputs = sampleimg.shape[0]*sampleimg.shape[1]
print("nb_inputs = " + str(nb_inputs))

# Creates the file and writes the first line
print("Creating destination file...")
file = open(destination_file, "w+")
print("Writing the first line...")
file.write(str(len(fileNames)) + " " + str(nb_inputs) + " " + str(nb_labels) + "\n")

# This function serializes the given file
def toDatabaseLine(fileName):
    line = ""
    image = cv2.imread(source_folder + os.sep + fileName)
    image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    for y in range(image.shape[0]):
        for x in range(image.shape[1]):
            line += "1 " if image[y,x]<127 else "0 "
    label = labels.index(fileName[:fileName.index('_')])
    for i in range(nb_labels):
        line += "1 " if i == label else "0 "
    return line

# Writes all the sample lines
print("Writing sample lines...")
for i in range(len(fileNames)):
    if(i > 0 and (i+1) % 1000 == 0):
        print(str(i+1) + " images processed")
    file.write(toDatabaseLine(fileNames[i]) + "\n")

print("Done !")
