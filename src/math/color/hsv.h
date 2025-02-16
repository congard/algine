// Copyright (c) 2014, Jan Winkler <winkler@cs.uni-bremen.de>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Universität Bremen nor the names of its
//       contributors may be used to endorse or promote products derived from
//       this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

/* Author: Jan Winkler */

// RGB <=> HSV converter
// https://gist.github.com/fairlight1337/4935ae72bcbcc1ba5c72

#ifndef ALGINE_COLOR_HSV_H
#define ALGINE_COLOR_HSV_H

namespace algine::internal {
/*! \brief Convert RGB to HSV color space

  Converts a given set of RGB values `r', `g', `b' into HSV
  coordinates. The input RGB values are in the range [0, 1], and the
  output HSV values are in the ranges h = [0, 360], and s, v = [0, 1], respectively.

  \param r Red component, used as input, range: [0, 1]
  \param g Green component, used as input, range: [0, 1]
  \param b Blue component, used as input, range: [0, 1]
  \param h Hue component, used as output, range: [0, 360]
  \param s Hue component, used as output, range: [0, 1]
  \param v Hue component, used as output, range: [0, 1]

*/
void rgb2hsv(float r, float g, float b, float &h, float &s, float &v);

/*! \brief Convert HSV to RGB color space

  Converts a given set of HSV values `h', `s', `v' into RGB
  coordinates. The output RGB values are in the range [0, 1], and
  the input HSV values are in the ranges h = [0, 360], and s, v =
  [0, 1], respectively.

  \param r Red component, used as output, range: [0, 1]
  \param g Green component, used as output, range: [0, 1]
  \param b Blue component, used as output, range: [0, 1]
  \param h Hue component, used as input, range: [0, 360]
  \param s Hue component, used as input, range: [0, 1]
  \param v Hue component, used as input, range: [0, 1]

*/
void hsv2rgb(float &r, float &g, float &b, float h, float s, float v);
}

#endif //ALGINE_COLOR_HSV_H
