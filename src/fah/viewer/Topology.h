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

#ifndef FAH_TOPOLOGY_H
#define FAH_TOPOLOGY_H

#include <cbang/SmartPointer.h>

#include "Atom.h"
#include "Bond.h"

#include <cbang/pyon/Object.h>
#include <cbang/geom/Rectangle.h>
#include <cbang/time/TimeStamp.h>

#include <iostream>
#include <vector>


namespace FAH {
  class Positions;

  class Topology : public cb::PyON::Object, public cb::TimeStamp {
  public:
    typedef std::vector<Atom> atoms_t;
    typedef std::vector<Bond> bonds_t;

  protected:
    atoms_t atoms;
    bonds_t bonds;

  public:
    bool isEmpty() const {return atoms.empty();}

    const atoms_t &getAtoms() const {return atoms;}
    const bonds_t &getBonds() const {return bonds;}

    void add(const Atom &atom) {atoms.push_back(atom);}
    void add(const Bond &bond) {bonds.push_back(bond);}

    void validate(const Positions &positions) const;
    void clear();

    unsigned findBonds(std::vector<unsigned> &bondCounts,
                       const Positions &positions);
    void findBonds(const Positions &positions);

    // From PyONObject
    const char *getPyONType() const {return "topology";}
    cb::SmartPointer<cb::JSON::Value> getJSON() const;
    void loadJSON(const cb::JSON::Value &value) {loadJSON(value, 1);}

    void loadJSON(const cb::JSON::Value &value, float scale);
  };
}

#endif // FAH_TOPOLOGY_H

