from os import listdir, chdir, mkdir
from os.path import isfile, join, exists
import subprocess

# Must be launched directly from scripts directory!
# This script is ONLY for Linux with installed Assimp, GLM, GLEW and GLFW
# This script is designed to compile the engine with a single command
# which will be printed into the console
# Warning: this script may not be updated for a long time
# Use CMake instead
# To launch output executable use LD_LIBRARY_PATH=/usr/local/lib ./_algine exec_

includes = ["include", "contrib"]
command = "g++"
outputFolder = "out"
output = outputFolder + "/algine"
linkLibraries = ["glfw", "GLEW", "GLU", "GL", "assimp"]
flags = ["pthread"]

chdir("..")

if not exists(outputFolder):
    print("Creating output directory")
    mkdir(outputFolder)

src = "src/"
include = "include/algine/"
sources = [src + f for f in listdir(src) if isfile(join(src, f))]
headers = [include + f for f in listdir(include) if isfile(join(include, f))]

# include paths
command += " -I " + " -I ".join(includes)
# sources
command += " " + " ".join(sources)
# headers
command += " " + " ".join(headers)
# link
command += " -l " + " -l ".join(linkLibraries)
# flags
command += " -" + " -".join(flags)
# output
command += " -o " + output

print(command)

process = subprocess.Popen(command.split(), stdout=subprocess.PIPE)
process.communicate()
if process.returncode != 0:
    print("Something went wrong. Exit code " + str(process.returncode))
else:
    print("Done")
