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

#ifndef FAH_VIEWER_H
#define FAH_VIEWER_H

#include "SimulationInfo.h"
#include "Protein.h"
#include "ViewMode.h"

#include <cbang/geom/Vector.h>

#include <string>

namespace FAH {
  class View;

  class ViewerBase : public ViewMode {
  public:
    virtual ~ViewerBase() {}

    virtual void lineUp(unsigned count = 1) {}
    virtual void lineDown(unsigned count = 1) {}
    virtual void pageUp() {}
    virtual void pageDown() {}

    virtual void init(ViewMode mode) = 0;
    virtual void release() = 0;
    virtual void draw(const SimulationInfo &info, const Protein *protein,
                      const View &view) = 0;
    virtual void resize(const View &view) = 0;
    virtual std::string pick(const cb::Vector2D &p) {return "";}
  };
}

#endif // FAH_VIEWER_H
