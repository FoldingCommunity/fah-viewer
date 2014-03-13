/******************************************************************************\

                     This file is part of the FAHViewer.

           The FAHViewer displays 3D views of Folding@home proteins.
                 Copyright (c) 2003-2014, Stanford University
                             All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

                For information regarding this software email:
                               Joseph Coffland
                        joseph@cauldrondevelopment.com

\******************************************************************************/

// Author: Maurice Ribble
// 3D Application Research Group
// (C)Advanced Research, Inc.2006 All rights reserved.

#ifndef FAH_UNIFORM_H
#define FAH_UNIFORM_H

#include <string>

namespace FAH {
  typedef enum {
    SAMPLE_UNKNOWN    = 0x0,
    SAMPLE_FLOAT      = 0x1,
    SAMPLE_FLOAT_VEC2 = 0x2,
    SAMPLE_FLOAT_VEC3 = 0x3,
    SAMPLE_FLOAT_VEC4 = 0x4,
    SAMPLE_INT        = 0x5,
    SAMPLE_FLOAT_MAT4 = 0xd,
    SAMPLE_PROGRAM    = 0xe,
  } uniform_t;


  /// Contains the actual data for uniforms, textures, and attributes.
  class Uniform {
  public:
    std::string name; // Text name used for this uniform or attribute
    uniform_t type; // Type of data used to represent this uniform

    int location; // Locations in uniform / texture / attribute index
    float matrixData[4][4]; // Matrix data
    float floatData[4]; // Vector float data
    unsigned textureHandle; // GL texture id
    unsigned textureUnit; // GL texture unit number
    bool depthTex; // Keeps track of whether this is depth texture or not
    int width; // Texture width
    int height; // Texture height
    unsigned fboHandle; // GL fbo id
    unsigned vertShaderHandle; // GL vertex shader id
    unsigned fragShaderHandle; // GL fragment shader id
    int progHandle; // GL program id
    int attachedProgram; // The program this uniform was attached to

    Uniform(const std::string name, uniform_t type);
    ~Uniform();

    void setLocation(unsigned program);
    void update(float *vals = 0);
    void link(const std::string uniform);
    void bindTexture(int width, int height);
    void bindFBO(int width, int height);
    unsigned loadProgram(const std::string &vertShader,
                         const std::string &fragShader);

  protected:
    unsigned loadShader(const std::string &filename, unsigned type);
  };
};

#endif // FAH_UNIFORM_H
