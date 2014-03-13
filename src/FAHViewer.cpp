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

#include <fah/viewer/ViewerApp.h>

#include <cbang/ApplicationMain.h>

#include <fah/viewer/GL.h>

// This causes windows to not automatically create a console
#if defined(_WIN32) && !defined(DEBUG)
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#endif

int main(int argc, char *argv[]) {
  // Init GLUT
  glutInit(&argc, argv);

  // Init GL
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(800, 600);
  // NOTE: OSX crashes if this is not in main()
  glutCreateWindow("Folding@home 3D visualization");

  return cb::doApplication<FAH::ViewerApp>(argc, argv);
}
