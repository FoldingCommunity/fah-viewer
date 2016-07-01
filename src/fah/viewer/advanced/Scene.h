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

// Author: Maurice Ribble
// 3D Application Research Group
// (C)Advanced Research, Inc.2006 All rights reserved.

#ifndef FAH_SCENE_H
#define FAH_SCENE_H

#include "Uniform.h"

#include <cbang/SmartPointer.h>

#include <vector>
#include <string>

namespace FAH {
  /// This class loads and draws the scene
  class Scene {
    typedef std::vector<cb::SmartPointer<Uniform> > uniforms_t;

    /// Vector of uniforms / textures / attributes
    uniforms_t uniforms;

    /// The most recent program handle to which attribs and uniforms are bound
    int recentProgramHandle;

  public:
    Scene();
    ~Scene();

    Uniform *findUniform(const std::string &name,
                         uniform_t type = SAMPLE_UNKNOWN);

    /// Draws the frame
    void drawFrame();

    /// Loads data from file: textures, attributes, uniforms, shaders
    void loadData(const std::string &filename);

    /// Puts the named program in use
    unsigned getProgramHandle(const std::string &name);

    /// Puts the named program in use
    void useProgram(const std::string &name);

    /// Updates the value of a uniform
    void updateUniform(const std::string &name, float *vals);

    /// Binds a texture into GL
    void bindTexture(const std::string &name, int width = 0, int height = 0);

    /// Links the uniform and binds a texture
    void linkAndBindTexture(const std::string &arg, const std::string &name,
                            int width = 0, int height = 0);

    /// Binds an FBO into GL
    void bindFBO(const std::string &name, int width = 0, int height = 0);

    /// Updates all the uniform data after a link
    void updateAllUniforms(int curProg);

    /// Deletes all the GL resources we have allocated
    void freeResources();
  };
};

#endif // FAH_SCENE_H
