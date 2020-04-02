/******************************************************************************\

                       This file is part of the FAHViewer.

            The FAHViewer displays 3D views of Folding@home proteins.
                    Copyright (c) 2016-2020, foldingathome.org
                   Copyright (c) 2003-2016, Stanford University
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

#include "VBO.h"

#include <fah/viewer/GL.h>

#include <string.h>

using namespace cb;
using namespace FAH;


VBO::~VBO() {
  if (vert) glDeleteBuffers(1, &vert);
  if (text) glDeleteBuffers(1, &text);
  if (norm) glDeleteBuffers(1, &norm);
}


void VBO::bind() {
  // Vertex
  if (glBindBuffer) glBindBuffer(GL_ARRAY_BUFFER, vert);
  glVertexPointer(3, GL_FLOAT, 0, vertData.get());
  glEnableClientState(GL_VERTEX_ARRAY);

  // Texture
  if (textured) {
    if (glBindBuffer) glBindBuffer(GL_ARRAY_BUFFER, text);
    glTexCoordPointer(2, GL_FLOAT, 0, textData.get());
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  }

  // Normal
  if (glBindBuffer) glBindBuffer(GL_ARRAY_BUFFER, norm);
  glNormalPointer(GL_FLOAT, 0, normData.get());
  glEnableClientState(GL_NORMAL_ARRAY);
}


void VBO::unbind() {
  glDisableClientState(GL_VERTEX_ARRAY);
  if (textured) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

  if (glBindBuffer) glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void VBO::loadArrays(unsigned size,
                     const SmartPointer<float>::Array &vertData,
                     const SmartPointer<float>::Array &textData,
                     const SmartPointer<float>::Array &normData) {
  this->size = size;

  if (glGenBuffers) {
    size *= sizeof(float);

    // Vertex
    glGenBuffers(1, &vert);
    glBindBuffer(GL_ARRAY_BUFFER, vert);
    glBufferData(GL_ARRAY_BUFFER, 3 * size, vertData.get(), GL_STATIC_DRAW);

    // Texture
    if (textured) {
      glGenBuffers(1, &text);
      glBindBuffer(GL_ARRAY_BUFFER, text);
      glBufferData(GL_ARRAY_BUFFER, 2 * size, textData.get(), GL_STATIC_DRAW);
    }

    // Normal
    glGenBuffers(1, &norm);
    glBindBuffer(GL_ARRAY_BUFFER, norm);
    glBufferData(GL_ARRAY_BUFFER, 3 * size, normData.get(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

  } else {
    // OpenGL prior to 1.5 does not have glGenBuffers
    this->vertData = vertData;
    if (textured) this->textData = textData;
    this->normData = normData;
  }
}
