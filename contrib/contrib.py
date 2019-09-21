# this file will install all contrib libs
# must be launched directly from contrib directory!

import shutil
import urllib.request
import os
from zipfile import ZipFile


def index(x):
    return int(x) - 1


# src file, dst folder
libs = [
      ["https://github.com/g-truc/glm/releases/download/0.9.9.6/glm-0.9.9.6.zip", "glm"],
      ["https://github.com/nlohmann/json/releases/download/v3.7.0/json.hpp", "nlohmann"],
      ["https://raw.githubusercontent.com/nothings/stb/master/stb_image.h", "stb"],
      ["https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h", "stb"],
      ["https://github.com/assimp/assimp/archive/v.5.0.0.rc1.zip", "assimp"],
      ["https://github.com/nigels-com/glew/releases/download/glew-2.1.0/glew-2.1.0.zip", "glew"],
      ["https://github.com/glfw/glfw/releases/download/3.3/glfw-3.3.zip", "glfw"]
]

print("=== Contrib installer ===")
print("One header libs:")
print("1) glm\n"
      "2) nlohmann/json.hpp\n"
      "3) stb/stb_image.h\n"
      "4) stb/std_image_write.h")
print("Non one header libs:")
print("5) assimp\n"
      "6) glew\n"
      "7) glfw")
installLibs = input("What libs you want to install? Separate with spaces (default=all): ").split(" ")

# [''] if pressed enter (all)
if len(installLibs) == 1 and installLibs[0] == '':
    installLibs = [l for l in range(1, 8)]

print("Removing old libs (if exists)")
for lib in installLibs:
    dstFolder = libs[index(lib)][1]
    if os.path.exists(dstFolder):
        print("Removing " + dstFolder)
        shutil.rmtree(dstFolder)

# can't merge this loop with previous because of two "stb" dirs
print("Creating new lib folders")
for lib in installLibs:
    dstFolder = libs[index(lib)][1]
    if not os.path.exists(dstFolder):
        print("Creating " + dstFolder)
        os.makedirs(dstFolder)

for lib in installLibs:
    dstFolder = libs[index(lib)][1]
    url = libs[index(lib)][0]
    filename = url.split("/")[-1]

    print("Downloading " + url + " into " + dstFolder + " folder")
    urllib.request.urlretrieve(url, os.getcwd() + "/" + dstFolder + "/" + filename)

    if filename.endswith("zip"):
        print("Extracting files from " + filename + " to " + dstFolder + "/__tmp__")
        with ZipFile(dstFolder + "/" + filename, 'r') as zipObj:
            extractedTo = dstFolder + "/__tmp__/" + zipObj.namelist()[0] + "/"
            zipObj.extractall(dstFolder + "/__tmp__")

        print("Moving files to " + dstFolder)
        files = os.listdir(extractedTo)
        for file in files:
            os.rename(extractedTo + file, dstFolder + "/" + file)

        print("Clean up")
        shutil.rmtree(dstFolder + "/__tmp__")
        os.remove(dstFolder + "/" + filename)

print("=== Done ===")
