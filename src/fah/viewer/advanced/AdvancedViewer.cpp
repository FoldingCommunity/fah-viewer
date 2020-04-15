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

// Adapted from code by Maurice Ribble
// 3D Application Research Group
// (C) ATI Research, Inc.2006 All rights reserved.

#include "AdvancedViewer.h"

#include <cbang/Exception.h>
#include <cbang/Zap.h>
#include <cbang/SmartPointer.h>
#include <cbang/Math.h>
#include <cbang/log/Logger.h>

#include <fah/viewer/GL.h>
#include <fah/viewer/View.h>

using namespace std;
using namespace cb;
using namespace FAH;

static const float lightPosition0[] = {20.0, 25.0, 30.0, 0.0};

static const float fullScreenQuad[] = {
  -1, -1,  1,  1,
  -1,  1,  1,  1,
   1,  1,  1,  1,
   1, -1,  1,  1,
};


AdvancedViewer::AdvancedViewer() {
  for (unsigned i = 0; i < 16; i++) {
    cameraProjectionMatrix[i] = 0;
    cameraViewMatrix[i] = 0;
    lightProjectionMatrix[i] = 0;
    lightViewMatrix[i] = 0;
  }
}


void AdvancedViewer::resetDraw(const View &view) {
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glUseProgram(0);

  BasicViewer::resetDraw(view);
}


void AdvancedViewer::setupShadow(const Vector3D &coord,
                                 const AxisAngleD &angle) {
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(lightViewMatrix);
  glTranslatef(coord.x(), coord.y(), coord.z());

  // Put the biased and scaled projective coordinates in matrix for texture0
  glActiveTexture(GL_TEXTURE0);
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glTranslatef(0.5, 0.5, 0.5);
  glScalef(0.5, 0.5, 0.5);
  glMultMatrixf(lightProjectionMatrix);
  glMultMatrixf(lightViewMatrix);

  // Put the matrix to convert to world coordinates in matrix for texture1
  glActiveTexture(GL_TEXTURE1);
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glTranslatef(coord.x(), coord.y(), coord.z());
  glRotatef(angle.angle(), angle.x(), angle.y(), angle.x());

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  CHECK_GL_ERROR("");
}


/// See http://en.wikipedia.org/wiki/Xorshift
static uint32_t rand_x;
static uint32_t rand_y;
static uint32_t rand_z;
static uint32_t rand_w;

static void xorshift_reset() {
  rand_x = 123456789;
  rand_y = 362436069;
  rand_z = 521288629;
  rand_w = 88675123;
}


static uint32_t xorshift_rand() {
  uint32_t t;

  t = rand_x ^ (rand_x << 11);
  rand_x = rand_y; rand_y = rand_z; rand_z = rand_w;
  return rand_w = rand_w ^ (rand_w >> 19) ^ (t ^ (t >> 8));
}


void AdvancedViewer::drawAtom(const Atom &atom, const Vector3D &position) {
  glPushMatrix();

  // Apply a random, but repeatable, rotation for the noise texture
  uint32_t rand = xorshift_rand();
  float angle = (rand & 512) * 360 / 512;
  float ax = !(rand & (1 << 10));
  float ay = !(rand & (1 << 11));
  float az = !(rand & (1 << 12));

  glRotatef(angle, ax, ay, az);

  // We need to update the texture 0 and texture 1 matrices with the correct
  // data on a per atom basis for shadow transforms
  setupShadow(position, AxisAngleD(angle, ax, ay, az));

  BasicViewer::drawAtom(atom, position);

  glPopMatrix();
  CHECK_GL_ERROR("");
}


void AdvancedViewer::updatePerspective(float radius, const View &view) {
  // Save matrices
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  // Copy camera projection and view matrices from BasicViewer
  setupPerspective(view, radius);
  glGetFloatv(GL_PROJECTION_MATRIX, cameraProjectionMatrix);
  glGetFloatv(GL_MODELVIEW_MATRIX, cameraViewMatrix);

  // Setup light projection matrix
  glLoadIdentity();
  gluPerspective(40, 1, 1, 1000);

  glGetFloatv(GL_MODELVIEW_MATRIX, lightProjectionMatrix);

  // Setup light view matrix
  glLoadIdentity();
  gluLookAt(lightPosition0[0], lightPosition0[1], lightPosition0[2],
            0, 0, 0, 0, 1, 0);
  double rotation[4];
  view.getRotation().toAxisAngle().toGLRotation(rotation);
  glRotatef(rotation[0], rotation[1], rotation[2], rotation[3]);
  glGetFloatv(GL_MODELVIEW_MATRIX, lightViewMatrix);

  // Cleanup
  glPopMatrix();
  glPopMatrix();

  CHECK_GL_ERROR("");
}


void AdvancedViewer::drawProtein(const Protein &protein) {
  xorshift_reset();
  drawAtoms(protein);
  drawBonds(protein);
}


void AdvancedViewer::drawBackground(const View &view) {
  if (!view.getBGTexture().isNull()) {
    scene.useProgram("attenuateTexture");
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, view.getBGTexture()->getID());

    glVertexPointer(4, GL_FLOAT, 0, fullScreenQuad);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
  }
}


void AdvancedViewer::applyBlur(const View &view) {
  unsigned width = view.getWidth();
  unsigned height = view.getHeight();

  // Copy the scene into a texture
  // Don't render directly to a texture because that skips zmask / hiz
  scene.bindTexture("sharpTex", width, height);
  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);

  // Pass 1: The first blur pass
  scene.useProgram("blur");
  scene.bindFBO("blurFbo1", width, height); // Render to this FBO
  glClear(GL_COLOR_BUFFER_BIT);

  // This texture contains the fully rendered scene
  scene.bindTexture("sharpTex", width, height);

  glVertexPointer(4, GL_FLOAT, 0, fullScreenQuad);
  glEnableClientState(GL_VERTEX_ARRAY);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  glDisableClientState(GL_VERTEX_ARRAY);

  // Pass 2: The second blur pass
  scene.useProgram("blur2");
  scene.bindFBO("blurFbo2", width, height); // Render to this FBO
  glClear(GL_COLOR_BUFFER_BIT);

  // This texture contains the blurred scene
  scene.bindTexture("blurFbo1", width, height);

  glVertexPointer(4, GL_FLOAT, 0, fullScreenQuad);
  glEnableClientState(GL_VERTEX_ARRAY);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  glDisableClientState(GL_VERTEX_ARRAY);

  // Pass 3: Combines the previous passes
  // Force rendering to the normal back buffer
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  scene.useProgram("combine");
  scene.bindTexture("sharpTex", width, height);
  scene.bindTexture("blurFbo2", width, height);
  scene.bindTexture("shadowMapFbo", SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
  glActiveTexture(GL_TEXTURE0);

  glVertexPointer(4, GL_FLOAT, 0, fullScreenQuad);
  glEnableClientState(GL_VERTEX_ARRAY);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  glDisableClientState(GL_VERTEX_ARRAY);
}


void AdvancedViewer::drawShadows(const Protein &protein) {
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(lightProjectionMatrix);
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(lightViewMatrix);

  scene.bindFBO("shadowMapFbo", SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
  glColorMask(false, false, false, false);

  scene.useProgram("genShadowMap");

  glPushAttrib(GL_VIEWPORT_BIT | GL_SCISSOR_BIT);
  glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
  glScissor(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
  glEnable(GL_SCISSOR_TEST);
  glEnable(GL_DEPTH_TEST);
  glClear(GL_DEPTH_BUFFER_BIT);

  drawProtein(protein);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_SCISSOR_TEST);
  glPopAttrib();
  glColorMask(true, true, true, true);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}


void AdvancedViewer::drawRealScene(const Protein &protein) {
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(cameraProjectionMatrix);
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(cameraViewMatrix);

  scene.useProgram("lighting");

  float toon =
    mode == MODE_TOON_SPACE_FILLED || mode == MODE_TOON_BALL_AND_STICK;
  scene.updateUniform("toon", &toon);

  scene.bindTexture("NormalMap"); // Atom texture
  scene.bindTexture("shadowMapFbo", SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

  glEnable(GL_DEPTH_TEST);

  drawProtein(protein);

  glDisable(GL_DEPTH_TEST);
}


void AdvancedViewer::drawScene(const Protein *protein, const View &view) {
  glFrontFace(GL_CW);

  if (protein) {
    updatePerspective(protein->getRadius(), view);
    drawShadows(*protein);
  }

  drawBackground(view);

  if (protein) {
    drawRealScene(*protein);

    if (view.getBlur())
      switch (view.getMode()) {
      case MODE_ADV_SPACE_FILLED: case MODE_ADV_BALL_AND_STICK:
      case MODE_ADV_STICK: applyBlur(view); break;
      default: break;
      }
  }

  resetDraw(view);
  CHECK_GL_ERROR("");
}


void AdvancedViewer::init(ViewMode mode) {
  BasicViewer::init(mode);

  // Turn off state that might have gotten turned on in the other modes
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_POINT_SMOOTH);
  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHT1);
  glDisable(GL_NORMALIZE);
  glDrawBuffer(GL_BACK);
  glDisable(GL_DEPTH_TEST);

  // Lights
  const float ambient0[] = {0.1, 0.1, 0.1, 1.0};
  const float diffuse0[] = {1.0, 1.0, 1.0, 1.0};
  const float lmodelAmbient[] = {0.5, 0.5, 0.5, 1.0};
  const float lmodelTwoSide[] = {GL_FALSE};

  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodelAmbient);
  glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodelTwoSide);

  // Load scene
  scene.loadData("SceneData.txt");

  initialized = true;

  CHECK_GL_ERROR("");
}


void AdvancedViewer::release() {
  if (!initialized) return;

  BasicViewer::release();
  scene.freeResources();

  CHECK_GL_ERROR("");
}


void AdvancedViewer::draw(const SimulationInfo &info, const Protein *protein,
                     const View &view) {
  // Draw main scene
  drawScene(protein, view);

  // Draw the rest
  BasicViewer::drawRest(info, view);
}
