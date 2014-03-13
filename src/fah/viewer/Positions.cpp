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

#include "Positions.h"

using namespace std;
using namespace cb;
using namespace FAH;


void Positions::init() {
  // Compute radius as max distance from origin
  radius = 0;
  Vector3D origin;

  for (unsigned i = 0; i < size(); i++) {
    double r = at(i).distance(origin);
    if (radius < r) radius = r;
  }

  // Find bounds
  bounds = Rectangle3D();
  for (unsigned i = 0; i < size(); i++) bounds.add(at(i));
}


void Positions::translate(const Vector3D &offset) {
  for (unsigned i = 0; i < size(); i++)
    at(i) += offset;

  this->offset += offset;

  init();
}


Vector3D Positions::findCenterOfMass() const {
  Vector3D com;
  for (unsigned i = 0; i < size(); i++) com += at(i);
  return com / size();
}


void Positions::translateToCenterOfMass() {
  translate(-findCenterOfMass());
}


SmartPointer<JSON::Value> Positions::getJSON() const {
  SmartPointer<JSON::List> list = new JSON::List;

  for (const_iterator it = begin(); it != end(); it++) {
    SmartPointer<JSON::List> coord = new JSON::List;
    coord->append(it->x());
    coord->append(it->y());
    coord->append(it->z());
    list->push_back(coord);
  }

  return list;
}


void Positions::loadJSON(const JSON::Value &value, float scale) {
  clear();

  for (unsigned i = 0; i < value.size(); i++) {
    const JSON::List &coord = value.getList(i);
    if (coord.size() != 3) THROWS("Position expected list of length 3");

    push_back(Vector3D(coord.getNumber(0), coord.getNumber(1),
                       coord.getNumber(2)) * scale);
  }

  init();
}
