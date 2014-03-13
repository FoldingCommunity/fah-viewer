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

#ifndef FAH_TRAJECTORY_H
#define FAH_TRAJECTORY_H

#include "Protein.h"
#include "Topology.h"
#include "Positions.h"

#include <cbang/SmartPointer.h>
#include <cbang/geom/Quaternion.h>

#include <vector>


namespace FAH {
  class Trajectory : protected std::vector<cb::SmartPointer<Positions> > {
    typedef std::vector<cb::SmartPointer<Positions> > Super_T;

    cb::SmartPointer<Topology> topology;
    Positions offsets;
    cb::QuaternionD rotation;
    bool center;
    bool align;
    unsigned interpolate;

  public:
    Trajectory(bool center = true, bool align = false, unsigned interpolate = 0,
               const cb::SmartPointer<Topology> &topology = new Topology) :
      topology(topology), center(center), align(align),
      interpolate(interpolate) {}

    void setTopology(const cb::SmartPointer<Topology> &topology)
    {this->topology = topology;}
    const cb::SmartPointer<Topology> &getTopology() const {return topology;}

    cb::SmartPointer<Protein> getProtein(unsigned i);

    void clear() {topology = new Topology; Super_T::clear();}
    void add(const cb::SmartPointer<Positions> &positions);

    void readXYZ(const std::string &filename);
    void readTPR(const std::string &filename);
    void readXTC(const std::string &filename, bool onlyNewFrames = false);
    void readTRN(const std::string &filename, bool onlyNewFrames = false);
    void readJSON(const std::string &filename);

    void ensureTopology();
    void recomputeBonds();

    // From Super_T
    using Super_T::empty;
    using Super_T::at;
    using Super_T::operator[];
    using Super_T::size;
    using Super_T::front;
    using Super_T::back;

  protected:
    void shiftIntoBox(Positions &p);
    void alignToLast(Positions &p);
    void interpolateTo(const Positions &p);
  };
}

#endif // FAH_TRAJECTORY_H

