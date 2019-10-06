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

#ifndef FAH_PROTEIN_H
#define FAH_PROTEIN_H

#include "Topology.h"
#include "Positions.h"

#include <cbang/SmartPointer.h>


namespace FAH {
  class Protein {
    cb::SmartPointer<Topology> topology;
    cb::SmartPointer<Positions> positions;

    double radius;

  public:
    Protein(const cb::SmartPointer<Topology> &topology,
            const cb::SmartPointer<Positions> &positions) :
      topology(topology), positions(positions),
      radius(positions.isNull() ? 0 : positions->getRadius()) {}

    const cb::SmartPointer<Topology> &getTopology() const {return topology;}
    const cb::SmartPointer<Positions> &getPositions() const {return positions;}

    void setRadius(double radius) {this->radius = radius;}
    double getRadius() const {return radius;}
  };
}

#endif // FAH_PROTEIN_H

