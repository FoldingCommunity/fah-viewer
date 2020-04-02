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

#include "Texture.h"

#include <cbang/util/Resource.h>
#include <cbang/log/Logger.h>

#include <fah/viewer/GL.h>
#include <fah/viewer/PPM.h>

#include <string.h>

using namespace std;
using namespace cb;
using namespace FAH;

namespace FAH {
  namespace Viewer {
    extern const DirectoryResource resource0;
  }
}


static SmartPointer<PPM> loadPPMResource(const string &name) {
  if (name.substr(0, 7) == "file://") return new PPM(name.substr(7));

  const Resource *ppmData = FAH::Viewer::resource0.find(name + ".ppm");
  if (!ppmData) THROW("Failed to load texture: " << name);

  return new PPM((uint8_t *)ppmData->getData(), ppmData->getLength());
}


Texture::Texture(const string &name, int width, int height, float alpha) :
  name(name), width(width), height(height), alpha(alpha), id(0), loaded(false) {
}


void Texture::load() {
  if (loaded) return;

  SmartPointer<PPM> rgbPPM = loadPPMResource(name);

  const uint8_t *data;
  
  SmartPointer<PPM> alphaPPM;
  SmartPointer<uint8_t>::Array buffer;
  if (alpha) {
    try {
      alphaPPM = loadPPMResource(name + "_alpha");
    } catch (const Exception &e) {} // Ignore

    if (!alphaPPM.isNull() && rgbPPM->getSize() != alphaPPM->getSize())
      THROW("Alpha layer size does not match RGB: " << name);

    buffer = new uint8_t[rgbPPM->getSize() * 4];
    
    for (unsigned i = 0; i < rgbPPM->getSize(); i++) {
      memcpy(&buffer[i * 4], &rgbPPM->getRaster()[i * 3], 3);
      buffer[i * 4 + 3] = (uint8_t)(alphaPPM.isNull() ? 255 * alpha :
                                    alpha * alphaPPM->getRaster()[i * 3]);
    }
    
    data = buffer.get();

  } else data = rgbPPM->getRaster();

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  unsigned w = rgbPPM->getWidth();
  unsigned h = rgbPPM->getHeight();

  if (GL_MAX_TEXTURE_SIZE < w || GL_MAX_TEXTURE_SIZE < h) {
    LOG_ERROR("OpenGL implementation has a max texture size of "
              << GL_MAX_TEXTURE_SIZE << "x" << GL_MAX_TEXTURE_SIZE
              << " texture " << name << " is " << width << "x" << height);

    if (GL_MAX_TEXTURE_SIZE < w) w = GL_MAX_TEXTURE_SIZE;
    if (GL_MAX_TEXTURE_SIZE < h) h = GL_MAX_TEXTURE_SIZE;
  }
  
  glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, w, h, 0,
                 alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

  if (!width) width = w;
  if (!height) height = h;

  loaded = true;
}


void Texture::release() {
  if (!loaded) return;

  glDeleteTextures(1, &id);

  loaded = false;
}


void Texture::draw(float x, float y, float w, float h) const {
  glPushAttrib(GL_COLOR_BUFFER_BIT);
  glEnable(GL_BLEND);

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D, id);

  if (!w) w = width;
  if (!h) h = height;

  glBegin(GL_POLYGON);
  glTexCoord2f(0, 1); glVertex2f(x, y);
  glTexCoord2f(1, 1); glVertex2f(x + w, y);
  glTexCoord2f(1, 0); glVertex2f(x + w, y + h);
  glTexCoord2f(0, 0); glVertex2f(x, y + h);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  glPopAttrib();

  CHECK_GL_ERROR("");
}
