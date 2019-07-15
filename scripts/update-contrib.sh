#!/usr/bin/env bash

echo Updating contrib

# stb_image.h
echo stb/stb_image.h
curl -o contrib/stb/stb_image.h https://raw.githubusercontent.com/nothings/stb/master/stb_image.h

# stb_image_write.h
echo stb/stb_image_write.h
curl -o contrib/stb/stb_image_write.h https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h

# json.hpp
echo nlohmann/json.hpp
curl -o contrib/nlohmann/json.hpp https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp