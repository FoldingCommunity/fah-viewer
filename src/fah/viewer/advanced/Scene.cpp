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

#include "Scene.h"

#include <cbang/Exception.h>

#include <cbang/util/Resource.h>

#include <fah/viewer/GL.h>
#include <fah/viewer/PPM.h>

#include <iostream>
#include <sstream>
#include <string>

#include <cstring>

using namespace std;
using namespace cb;
using namespace FAH;

namespace FAH {
  namespace Viewer {
    extern const DirectoryResource resource0;
  }
}


Scene::Scene() : recentProgramHandle(-1) {}


Scene::~Scene() {
  freeResources();
}


Uniform *Scene::findUniform(const string &name, uniform_t type) {
  for (uniforms_t::iterator it = uniforms.begin(); it != uniforms.end(); it++)
    if ((*it)->name == name && ((*it)->type == type || type == SAMPLE_UNKNOWN))
      return it->get();

  THROW("Uniform " << name << " not found");
}


/***
 * Loads textures, attributes, uniforms, shaders, etc.
 *
 * @param filename is the name for the file where we get the data
 */
void Scene::loadData(const string &filename) {
  recentProgramHandle = -1;

  const Resource *data = FAH::Viewer::resource0.find(filename);
  if (!data) THROW("Could not find resource: " << filename);

  string s(data->getData(), data->getLength());
  istringstream in(s);

  freeResources();

  while (!in.eof()) {
    SmartPointer<Uniform> uniform;
    string lineString;
    string item;
    string key;

    getline(in, lineString);
    stringstream line(lineString);

    line >> item;

    if (8 < item.length() && item.substr(0, 8) == "uniform_") {
      float val[16];
      line >> key;

      item = item.substr(8);
      int count;
      uniform_t type;
      if (item == "float") {type = SAMPLE_FLOAT; count = 1;}
      else if (item == "vec2") {type = SAMPLE_FLOAT_VEC2; count = 2;}
      else if (item == "vec3") {type = SAMPLE_FLOAT_VEC3; count = 3;}
      else if (item == "vec4") {type = SAMPLE_FLOAT_VEC4; count = 4;}
      else if (item == "mat4") {type = SAMPLE_FLOAT_MAT4; count = 16;}
      else THROW("Invalid uniform type " << item);

      for (int i = 0; i < count; i++) line >> val[i];

      uniform = new Uniform(key, type);
      uniform->setLocation(recentProgramHandle);
      uniform->update(val);

    } else if (item == "attribute") {
      int val;

      line >> key >> val;
      glBindAttribLocation(recentProgramHandle, val, key.c_str());

    } else if (item == "texture") {
      string val;
      int texUnit;

      line >> key >> texUnit >> val;

      const Resource *ppmData = FAH::Viewer::resource0.find(val);

      if (ppmData) {
        PPM ppm((uint8_t *)ppmData->getData(), ppmData->getLength());

        unsigned texId;
        glGenTextures(1, &texId);
        glActiveTexture(GL_TEXTURE0 + texUnit);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ppm.getWidth(),
                     ppm.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,
                     ppm.getRaster());

        uniform = new Uniform(key, SAMPLE_INT);
        uniform->textureHandle = texId;
        uniform->textureUnit = texUnit;

        if (recentProgramHandle != -1) {
          uniform->setLocation(recentProgramHandle);
          uniform->update(0);
        }

      } else THROW("Failed to load texture: " << val);

    } else if (item == "nullTexture" || item == "colorTexFbo" ||
               item == "depthTexFbo") {
      unsigned texId;
      int texUnit;
      line >> key >> texUnit;

      uniform = new Uniform(key, SAMPLE_INT);
      uniform->textureUnit = texUnit;
      glGenTextures(1, &texId);
      uniform->textureHandle = texId;

      if (item != "nullTexture") {
        unsigned fboId;
        glGenFramebuffersEXT(1, &fboId);
        uniform->fboHandle = fboId;
      }

      if (item == "depthTexFbo") uniform->depthTex = true;

      glActiveTexture(GL_TEXTURE0 + texUnit);
      glBindTexture(GL_TEXTURE_2D, texId);

      if (item == "depthTexFbo") {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      }

    } else if (item == "program") {
      string vertShader, fragShader;
      line >> key >> vertShader >> fragShader;

      uniform = new Uniform(key, SAMPLE_PROGRAM);
      recentProgramHandle = uniform->loadProgram(vertShader, fragShader);
      glUseProgram(recentProgramHandle);

    } else if (item.empty() || item[0] == '/' || item[0] == '#') {
      // Either an empty line or a comment

    } else THROW("Unsupported keyword in uniform file:" << item);

    CHECK_GL_ERROR("Uniform " << key << "caused ");

    if (!uniform.isNull()) uniforms.push_back(uniform);
  }
}


/***
 * Puts the named program in use
 *
 * @param name - The name we gave to the program
 *
 * @return The program handle
 */
unsigned Scene::getProgramHandle(const string &name) {
  return findUniform(name, SAMPLE_PROGRAM)->progHandle;
}


void Scene::useProgram(const string &name) {
  glUseProgram(getProgramHandle(name));
}


/***
 * Updates the value of a uniform
 *
 * @param name - The name we gave to the uniform
 * @param vals - An array of values we want to replace in the current uniform
 */
void Scene::updateUniform(const string &name, float *vals) {
  findUniform(name)->update(vals);
}


/***
 * Binds a texture
 *
 * @param name - The name we gave the texture
 * @param width - The width of the texture
 * @param height - The height of the texture
 */
void Scene::bindTexture(const string &name, int width, int height) {
  findUniform(name, SAMPLE_INT)->bindTexture(width, height);
}


/***
 * Links and binds a texture
 *
 * @param arg - The name of the uniform to bind to
 * @param name - The name we gave the texture
 * @param width - The width of the texture
 * @param height - The height of the texture
 */
void  Scene::linkAndBindTexture(const string &arg, const string &name,
                                int width, int height) {
  Uniform *uniform = findUniform(name, SAMPLE_INT);

  uniform->link(arg);
  uniform->bindTexture(width, height);
}


/***
 * Binds an FBO into GL
 *
 * @param name - The name we gave the fbo
 * @param width - The width of the fbo
 * @param height - The height of the fbo
 */
void Scene::bindFBO(const string &name, int width, int height) {
  findUniform(name, SAMPLE_INT)->bindFBO(width, height);
}


void Scene::updateAllUniforms(int curProg) {
  for (uniforms_t::iterator it = uniforms.begin(); it != uniforms.end(); ++it)
    if (curProg == (*it)->attachedProgram) (*it)->update(0);
}


/// Deletes all the GL resources we have allocated
void Scene::freeResources() {
  uniforms.clear();
}
