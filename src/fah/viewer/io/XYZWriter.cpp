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

#include "XYZWriter.h"

#include <fah/viewer/Protein.h>
#include <fah/viewer/Positions.h>
#include <fah/viewer/Topology.h>

using namespace std;
using namespace cb;
using namespace FAH;


void XYZWriter::write(const Positions &positions, const Topology &topology) {
  ostream &stream = sink.getStream();

  const Topology::atoms_t &atoms = topology.getAtoms();

  stream << atoms.size() << '\t' << sink.getName() << '\n';

  for (unsigned i = 0; i < atoms.size(); i++)
    stream << (i + 1) << '\t' << atoms[i].getType() << '\t'
           << positions[i].x() << '\t' << positions[i].y() << '\t'
           << positions[i].z() << "\t1\n";
}


void XYZWriter::write(const Protein &protein) {
  write(*protein.getPositions(), *protein.getTopology());
}
