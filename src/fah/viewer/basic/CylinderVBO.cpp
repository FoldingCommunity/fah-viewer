/******************************************************************************\

                     This file is part of the FAHViewer.

           The FAHViewer displays 3D views of Folding@home proteins.
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

#include "CylinderVBO.h"

#include <fah/viewer/GL.h>

#include <cbang/geom/Vector.h>
#include <cbang/SmartPointer.h>

#include <math.h>

using namespace cb;
using namespace FAH;


CylinderVBO::CylinderVBO(float baseRadius, float topRadius, float length,
                         int slices, int stacks, bool textured) :
  VBO(textured), stacks(stacks), slices(slices) {
  float radiusStep = (topRadius - baseRadius) / (float)stacks;
  float stepSizeSlice = M_PI * 2.0 / (float)slices;
 
  Vector3D vertex[4];
  Vector3D normal[4];
  Vector2D texture[4];
 
  float ds = 1.0 / (float)slices;
  float dt = 1.0 / (float)stacks;
  float dz = length / (float)stacks;

  float zNormal = baseRadius - topRadius; // Rise over run
  if (-0.00001 < zNormal && zNormal < 0.00001) zNormal = 0;

  int size = stacks * (1 + slices) * 6;
  SmartPointer<float>::Array vertData = new float[size * 3];
  SmartPointer<float>::Array textData = new float[size * 2];
  SmartPointer<float>::Array normData = new float[size * 3];
  int vertI = 0;
  int textI = 0;
  int normI = 0;

  for (int i = 0; i < stacks; i++) {
    float t = i * dt;
    float tNext = (i == stacks - 1) ? 0 : t + dt;
    float currentRadius = baseRadius + radiusStep * i;
    float nextRadius = currentRadius + radiusStep;
    float currentZ = i * dz;
    float nextZ = currentZ + dz;

    for (int j = 0; j < slices; j++) {
      float s = j * ds;
      float sNext = (j == slices - 1) ? 0 : s + ds;
      float theta = stepSizeSlice * j;
      float thetaNext = (j == slices - 1) ? 0 : theta + stepSizeSlice;
                 
      // Inner First
      vertex[1].x() = cos(theta) * currentRadius;
      vertex[1].y() = sin(theta) * currentRadius;
      vertex[1].z() = currentZ;

      normal[1].x() = vertex[1].x();
      normal[1].y() = vertex[1].y();
      normal[1].z() = zNormal;
      normal[1] = normal[1].normalize();

      texture[1].x() = s;
      texture[1].y() = t;

      // Outer First
      vertex[0].x() = cos(theta) * nextRadius;
      vertex[0].y() = sin(theta) * nextRadius;
      vertex[0].z() = nextZ;

      if (nextRadius < -0.00001 || 0.00001 < nextRadius) {
        normal[0].x() = vertex[0].x();
        normal[0].y() = vertex[0].y();
        normal[0].z() = zNormal;
        normal[0] = normal[0].normalize();

      } else normal[0] = normal[1];

      texture[0].x() = s;
      texture[0].y() = tNext;

      // Inner second
      vertex[3].x() = cos(thetaNext) * currentRadius;
      vertex[3].y() = sin(thetaNext) * currentRadius;
      vertex[3].z() = currentZ;

      normal[3].x() = vertex[3].x();
      normal[3].y() = vertex[3].y();
      normal[3].z() = zNormal;
      normal[3] = normal[3].normalize();

      texture[3].x() = sNext;
      texture[3].y() = t;

      // Outer second
      vertex[2].x() = cos(thetaNext) * nextRadius;
      vertex[2].y() = sin(thetaNext) * nextRadius;
      vertex[2].z() = nextZ;

      if (nextRadius < -0.00001 || 0.00001 < nextRadius) {
        normal[2].x() = vertex[2].x();
        normal[2].y() = vertex[2].y();
        normal[2].z() = zNormal;
        normal[2] = normal[2].normalize();

      } else normal[2] = normal[3];

      texture[2].x() = sNext;
      texture[2].y() = tNext;

      // Add triangles
      for (int m = 0; m < 2; m++) {
        for (int k = 0; k < 3; k++)
          for (int l = 0; l < 3; l++) {
            vertData[vertI++] = vertex[k][l];
            if (l < 2) textData[textI++] = texture[k][l];
            normData[normI++] = normal[k][l];
          }

        vertex[0] = vertex[3];
        normal[0] = normal[3];
        texture[0] = texture[3];
      }
    }
  }

  loadArrays(size, vertData, textData, normData);
}


void CylinderVBO::draw() {
  glDrawArrays(GL_TRIANGLES, 0, stacks * slices * 6);
}
