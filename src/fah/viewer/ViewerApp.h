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

#ifndef FAH_VIEWER_APP_H
#define FAH_VIEWER_APP_H

#include "View.h"

#include <cbang/SmartPointer.h>
#include <cbang/time/Timer.h>
#include <cbang/Application.h>
#include <cbang/geom/Quaternion.h>


namespace FAH {
  class ViewerApp : public cb::Application, public View {
    static ViewerApp *singleton;

    bool visible;
    bool lastPause;

    int formerX;
    int formerY;
    int formerWidth;
    int formerHeight;

    cb::Vector2D mouseStart2D;
    cb::Vector3D mouseStart;
    bool mouseDragging;
    cb::QuaternionD startRotation;
    cb::Vector3D xAxis;
    cb::Vector3D xOffset;

    bool fullscreen;
    bool force;

  public:
    ViewerApp();

    static ViewerApp &instance();
    static bool _hasFeature(int feature);

    // From Application
    int init(int argc, char *argv[]);
    void run();
    void quit();

    void setFullscreen(bool fullscreen);
    cb::Vector3D findBallVector(unsigned x, unsigned y);

    // GLUT call backs
    void mouse(int button, int state, int x, int y);
    void motion(int x, int y);
    void keys(unsigned char key, int x, int y);
    void special(int key, int x, int y);
    void render();
    void visibility(int state);
    void idle();

    // From View
    void redisplay();
    void reshape(unsigned width, unsigned height);
  };
}

#endif // FAH_VIEWER_APP_H

