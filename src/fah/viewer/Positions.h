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

#ifndef FAH_POSITIONS_H
#define FAH_POSITIONS_H

#include <fah/viewer/pyon/Object.h>

#include <cbang/geom/Rectangle.h>
#include <cbang/time/TimeStamp.h>

#include <vector>

namespace FAH {
  class Positions : public PyON::Object, public std::vector<cb::Vector3D>,
                    public cb::TimeStamp {
    std::vector<cb::Vector3D> box;
    double radius;
    cb::Rectangle3D bounds;
    cb::Vector3D offset;

  public:
    Positions() : radius(0) {}
    Positions(const cb::JSON::Value &value, float scale = 1) : radius(0)
    {loadJSON(value, scale);}

    void setBox(const std::vector<cb::Vector3D> &box) {this->box = box;}
    const std::vector<cb::Vector3D> &getBox() const {return box;}

    double getRadius() const {return radius;}
    cb::Rectangle3D getBounds() const {return bounds;}

    void setOffset(const cb::Vector3D &offset) {this->offset = offset;}
    const cb::Vector3D &getOffset() const {return offset;}

    void init();

    void translate(const cb::Vector3D &offset);
    cb::Vector3D findCenterOfMass() const;
    void translateToCenterOfMass();

    // From PyONObject
    const char *getPyONType() const {return "positions";}
    cb::SmartPointer<cb::JSON::Value> getJSON() const;
    void loadJSON(const cb::JSON::Value &value) {loadJSON(value, 1);}

    void loadJSON(const cb::JSON::Value &value, float scale);
  };
}

#endif // FAH_POSITIONS_H

