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

#ifndef FAH_ADVANCED_VIEWER_H
#define FAH_ADVANCED_VIEWER_H

#include <fah/viewer/basic/BasicViewer.h>

#include <cbang/geom/AxisAngle.h>

#include "Scene.h"

#define SHADOW_MAP_SIZE 1024


namespace FAH {
  class AdvancedViewer : public BasicViewer {
    Scene scene;

    float cameraProjectionMatrix[16];
    float cameraViewMatrix[16];
    float lightProjectionMatrix[16];
    float lightViewMatrix[16];

  public:
    AdvancedViewer();
    ~AdvancedViewer() {release();}

    // From BasicViewer
    void resetDraw(const View &view);
    void setupShadow(const cb::Vector3D &coord, const cb::AxisAngleD &angle);
    void drawAtom(const Atom &atom, const cb::Vector3D &position);

    void updatePerspective(float radius, const View &view);
    void drawProtein(const Protein &protein);
    void drawBackground(const View &view);
    void applyBlur(const View &view);
    void drawShadows(const Protein &protein);
    void drawRealScene(const Protein &protein);
    void drawScene(const Protein *protein, const View &view);

    void init(ViewMode mode);
    void release();
    void draw(const SimulationInfo &info, const Protein *protein,
              const View &view);
  };
}

#endif // FAH_ADVANCED_VIEWER_H

