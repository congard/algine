import os

if not os.getcwd().endswith("contrib"):
    print("The script must be launched directly from the contrib directory")
    exit(1)

from urllib.request import urlretrieve
from os.path import exists, join

# src file, dst folder
libs = [
    ["https://github.com/nlohmann/json/releases/download/v3.9.1/json.hpp", "nlohmann"],
    ["https://raw.githubusercontent.com/nothings/stb/master/stb_image.h", "stb"],
    ["https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h", "stb"]
]

for lib in libs:
    dstFolder = lib[1]
    url = lib[0]

    if not exists(dstFolder):
        os.makedirs(dstFolder)

    filename = url.split("/")[-1]  # file name from url
    filepath = join(dstFolder, filename)

    if exists(filepath):
        print("File " + filepath + " will be reinstalled")

        os.remove(filepath)

    print("Installing " + filename + " to " + filepath)

    # download file
    urlretrieve(url, filepath)
