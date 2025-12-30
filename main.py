from logging import exception
from PIL import Image as img
import numpy as np
import os

width = 320
height = 240

before = 0
after = 0

fileCount = 0

codec = "latin"

area = height * width

folderName = input("Folder Name : ")
folderName = folderName.replace(" ", "").split(",")

for folder in folderName:
    try:
        path = os.listdir(folder)
        fileCount = len(path)
        for files in path:
            f = open(folder + "/" + files, 'r', encoding=codec, errors="ignore")
            f = f.read()
            pixels = [ord(i) for i in f]
            before = len(pixels)
            # print(f"length : {before}")
            if len(pixels) < area:
                for i in range(area - len(pixels)):
                    pixels.append(1)
            # print(pixels)
            after = len(pixels)

            arr = np.array(pixels, dtype=np.uint8)
            arr = arr.reshape(height, width)

            data = img.fromarray(arr, mode='L')
            data.save(folder + '/IMG_' + str(path.index(files)) + '.png')
            os.remove(folder + "/" + files)
            # print(arr)
            print(f"Progress : Folder {folder}  :  {int(path.index(files) + 1)} of {fileCount}   -> loss   : {((after - before) / after) * 100} %")
        os.rename(src=folder, dst=folder + "_built")
    except exception:
        print(exception)