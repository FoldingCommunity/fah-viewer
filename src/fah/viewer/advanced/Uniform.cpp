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

#include "Uniform.h"

#include <cbang/Exception.h>

#include <cbang/util/Resource.h>
#include <cbang/log/Logger.h>

#include <fah/viewer/GL.h>

#include <string.h>

using namespace std;
using namespace cb;
using namespace FAH;

namespace FAH {
  namespace Viewer {
    extern const DirectoryResource resource0;
  }
}


Uniform::Uniform(const string name, uniform_t type) :
  name(name), type(type), location(-1), textureHandle(0), textureUnit(0),
  depthTex(false), width(0), height(0), fboHandle(0),
  vertShaderHandle(0), fragShaderHandle(0), progHandle(-1),
  attachedProgram(-1) {
  memset(matrixData, 0, sizeof(matrixData));
  memset(floatData, 0, sizeof(floatData));
}


Uniform::~Uniform() {
  if (vertShaderHandle) glDeleteShader(vertShaderHandle);
  if (fragShaderHandle) glDeleteShader(fragShaderHandle);
  if (progHandle != -1) glDeleteProgram(progHandle);
  if (fboHandle) glDeleteFramebuffersEXT(1, &fboHandle);
  else if (textureHandle) glDeleteTextures(1, &textureHandle);
}


void Uniform::setLocation(unsigned program) {
  attachedProgram = program;
  location = glGetUniformLocation(program, name.c_str());
  if (location == -1)
    THROW("Location " << name << " not found for program id " << program);
}


void Uniform::update(float *vals) {
  switch (type) {
  case SAMPLE_FLOAT:
    if (vals) memcpy(floatData, vals, sizeof(float) * 1);
    glUniform1f(location, floatData[0]);
    break;

  case SAMPLE_FLOAT_VEC2:
    if (vals) memcpy(floatData, vals, sizeof(float) * 2);
    glUniform2f(location, floatData[0], floatData[1]);
    break;

  case SAMPLE_FLOAT_VEC3:
    if (vals) memcpy(floatData, vals, sizeof(float) * 3);
    glUniform3f(location, floatData[0], floatData[1], floatData[2]);
    break;

  case SAMPLE_FLOAT_VEC4:
    if (vals) memcpy(floatData, vals, sizeof(float) * 4);
    glUniform4f(location, floatData[0], floatData[1], floatData[2],
                floatData[3]);
    break;

  case SAMPLE_FLOAT_MAT4:
    if (vals) memcpy(matrixData, vals, sizeof(float) * 16);
    glUniformMatrix4fv(location, 1, GL_FALSE, &matrixData[0][0]);
    break;

  case SAMPLE_INT:
    // Prevents error on nullTexture which has not yet been given a
    // location, done at bind time
    if (location != -1) glUniform1i(location, textureUnit);
    break;

  case SAMPLE_PROGRAM: break;

  default: THROW("Invalid Uniform type: " << type);
  }
}


void Uniform::link(const string uniform) {
  glGetIntegerv(GL_CURRENT_PROGRAM, (int *)&attachedProgram);
  location = glGetUniformLocation(attachedProgram, uniform.c_str());

  // NOTE: We ignore location == -1

  update();
}


void Uniform::bindTexture(int width, int height) {
  if (!textureHandle) THROW("Uniform " << name << " is not a texture");

  link(name);

  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(GL_TEXTURE_2D, textureHandle);

  // If these are 0 then we just want to keep the current size
  if (width && height && (this->width != width || this->height != height)) {
    this->width = width;
    this->height = height;

    if (depthTex)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0,
                   GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
    else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                      GL_UNSIGNED_BYTE, 0);
  }
}


void Uniform::bindFBO(int width, int height) {
  if (!textureHandle || !fboHandle)
    THROW("Uniform " << name << " is not an FBO");

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboHandle);

  if (this->width != width || this->height != height) {
    int curTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &curTexture);

    this->width = width;
    this->height = height;

    glBindTexture(GL_TEXTURE_2D, textureHandle);

    if (depthTex)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0,
                   GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
    else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                      GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, curTexture);

    if (depthTex) {
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                GL_TEXTURE_2D, textureHandle, 0);
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);

    } else
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                GL_TEXTURE_2D, textureHandle, 0);
  }
}


static string getShaderInfoLog(unsigned handle) {
  char log[1000];
  glGetShaderInfoLog(handle, 1000, 0, log);
  return log;
}


unsigned Uniform::loadProgram(const string &vertShader,
                              const string &fragShader) {
  // Create the program
  progHandle = glCreateProgram();

  // Create one shader of object of each type
  vertShaderHandle = loadShader(vertShader, GL_VERTEX_SHADER);
  fragShaderHandle = loadShader(fragShader, GL_FRAGMENT_SHADER);

  // Link the whole program together
  glLinkProgram(progHandle);

  // Check for link success
  int linkResult = 0;
  glGetProgramiv(progHandle, GL_LINK_STATUS, &linkResult);

  if (!linkResult) THROW("Failed to link program object: " << name
                          << ": " << getShaderInfoLog(progHandle));

  return progHandle;
}


/***
 * Loads shader objects
 *
 * @param filename is the name for the file where we get objects
 * @param type shader type
 *
 * @return The shader handle
 */
unsigned Uniform::loadShader(const string &filename, unsigned type) {
  const Resource *data = FAH::Viewer::resource0.find(filename);
  if (!data) THROW("Failed to find shader object: " << filename);

  const char *source = (char *)data->getData();

  unsigned handle = glCreateShader(type);

  // Pass our sources to OpenGL
  glShaderSource(handle, 1, &source, 0);

  // Compile object
  glCompileShader(handle);

  // Check for link success
  int compileResult = 0;
  glGetShaderiv(handle, GL_COMPILE_STATUS, &compileResult);
  if (!compileResult) THROW("Failed to compile shader: " << filename << ": "
                             << getShaderInfoLog(handle));

  // Attach to the program
  glAttachShader(progHandle, handle);

  return handle;
}
