/******************************************************************************\

                       This file is part of the FAHViewer.

            The FAHViewer displays 3D views of Folding@home proteins.
                    Copyright (c) 2016-2019, foldingathome.org
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

#ifndef FAH_BASIC_VIEWER_H
#define FAH_BASIC_VIEWER_H

#include <cbang/SmartPointer.h>

#include <cbang/geom/AxisAngle.h>
#include <cbang/geom/Vector.h>

#include <fah/viewer/Viewer.h>

#include "GLFreeType.h"
#include "Texture.h"
#include "Box.h"
#include "Picker.h"
#include "SphereVBO.h"
#include "CylinderVBO.h"

#include <string>
#include <utility>

#define SUBDIVISIONS 32
#define SPHERE_SIZE 1.0
#define SPHERE_SIZE_SMALL 0.5
#define SPHERE_SIZE_TINY 0.2
#define BOND_RADIUS 0.2

namespace FAH {
  class BasicViewer : public ViewerBase {
  protected:
    ViewMode mode;

    bool fontsLoaded;
    GLFreeType *font;
    GLFreeType *fontBold;

    cb::SmartPointer<SphereVBO> sphere;
    cb::SmartPointer<CylinderVBO> cylinder;

    Box box;
    Box darkBox;
    std::vector<cb::SmartPointer<Texture> > logos;
    std::vector<cb::SmartPointer<Texture> > buttons;
    Texture cdLogo;
    Texture fahLogo;

    Picker picker;

    float popupYOffset;
    float popupPageHeight;
    float popupLineHeight;

    bool initialized;

  public:
    BasicViewer();
    ~BasicViewer();

    virtual void loadFonts();

    virtual void print(unsigned x, unsigned y, const std::string &s,
                       bool bold = false);
    virtual void resetDraw(const View &view);
    virtual void setMaterial(const Atom &atom);
    virtual void setupShadow(const cb::Vector3D &coord,
                             const cb::AxisAngleD &angle) {}
    virtual void drawAtom(const Atom &atom, const cb::Vector3D &position);
    virtual void drawBond(const Protein &protein, const Bond &bond);

    void drawCuboid(const cb::Rectangle3D &r);
    void drawBox(const Positions &positions);
    void drawAtoms(const Protein &protein);
    void drawBonds(const Protein &protein);
    void setupPerspective(const View &view, double radius);
    void drawProtein(const Protein &protein, const View &view);
    void drawInfo(const SimulationInfo &info, const View &view);
    void drawLogos(const View &view);
    void drawButtons(const View &view);
    void drawBackground(const View &view);
    void drawPopup(const View &view, float width, float height,
                   float contentHeight = 0);
    void drawAbout(const View &view);
    void drawTextPopup(const View &view, const std::string &title,
                       const std::string &text);
    void drawRest(const SimulationInfo &info, const View &view);

    void lineUp(unsigned count = 1) {popupYOffset -= count * popupLineHeight;}
    void lineDown(unsigned count = 1) {popupYOffset += count * popupLineHeight;}
    void pageUp() {popupYOffset -= popupPageHeight;}
    void pageDown() {popupYOffset += popupPageHeight;}

    void init(ViewMode mode);
    void release();
    void draw(const SimulationInfo &info, const Protein *protein,
              const View &view);
    void resize(const View &view);
    std::string pick(const cb::Vector2D &p);

    static cb::Vector2D project(const cb::Vector2D &v);
  };
}

#endif // FAH_BASIC_VIEWER_H
