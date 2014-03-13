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

#include "SphereVBO.h"

#include <fah/viewer/GL.h>

#include <cbang/SmartPointer.h>
#include <cbang/log/Logger.h>

using namespace cb;
using namespace FAH;


SphereVBO::SphereVBO(const Vector3D &center, float radius, int slices,
                     bool textured) : VBO(textured), slices(slices) {
  int size = (slices + 1) * slices;
  int posSize = size * 3;
  int texSize = size * 2;
  int normSize = size * 3;

  if (radius < 0) radius = -radius;
  if (slices < 0) slices = -slices;
  if (slices < 4 || radius <= 0) return;

  SmartPointer<float>::Array posData = new float[posSize];
  SmartPointer<float>::Array texData = new float[texSize];
  SmartPointer<float>::Array normData = new float[normSize];

  for (int j = 0; j < slices / 2; j++) {
    float theta1 = j * M_PI * 2.0 / slices - M_PI / 2.0;
    float theta2 = (j + 1) * M_PI * 2.0 / slices - M_PI / 2.0;

    for (int i = 0; i <= slices; i++) {
      float theta3 = i * M_PI * 2 / slices;

      int index = j * slices + i;
      int posI = index * 6;
      int texI = index * 4;
      int normI = index * 6;

      float x = cos(theta2) * cos(theta3);
      float y = sin(theta2);
      float z = cos(theta2) * sin(theta3);

      posData[posI++] = center.x() + radius * x;
      posData[posI++] = center.y() + radius * y;
      posData[posI++] = center.z() + radius * z;
      texData[texI++] = i / (float)slices;
      texData[texI++] = 2 * (j + 1) / (float)slices;
      normData[normI++] = x;
      normData[normI++] = y;
      normData[normI++] = z;

      x = cos(theta1) * cos(theta3);
      y = sin(theta1);
      z = cos(theta1) * sin(theta3);

      posData[posI++] = center.x() + radius * x;
      posData[posI++] = center.y() + radius * y;
      posData[posI++] = center.z() + radius * z;
      texData[texI++] = i / (float)slices;
      texData[texI++] = 2 * j / (float)slices;
      normData[normI++] = x;
      normData[normI++] = y;
      normData[normI++] = z;
    }
  }

  loadArrays(size, posData, texData, normData);
}


void SphereVBO::draw() {
  glDrawArrays(GL_QUAD_STRIP, 0, slices * slices);
}
