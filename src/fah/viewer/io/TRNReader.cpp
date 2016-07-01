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

#include "TRNReader.h"
#include "GromacsErrorHandler.h"

#include <fah/viewer/Positions.h>
#include <fah/viewer/Topology.h>
#include <fah/viewer/Trajectory.h>

#include <cbang/Exception.h>

#include <stdio.h>

#if defined(HAVE_GROMACS)
extern "C" {
#include <gromacs/trnio.h>
}
#endif

using namespace std;
using namespace cb;
using namespace FAH;


namespace FAH {
  struct TRNReader::private_t  {
    t_fileio *file;
    t_trnheader header;
    rvec *x;

    private_t() : file(0), x(0) {}

    ~private_t() {
#if HAVE_GROMACS
      if (setjmp(gromacs_error_handler())) return; // Cannot throw here

      if (file) close_trn(file);
      file = 0;
      if (x) free(x);
      x = 0;
#endif
    }
  };
}


TRNReader::TRNReader(const string &filename) :
  filename(filename), p(new private_t) {

#if HAVE_GROMACS
  if (setjmp(gromacs_error_handler())) THROW("Failed to open TRN");

  if (!(p->file = open_trn(filename.c_str(), "r")))
    THROWS("Failed to open TRN file: " << filename);

  gmx_bool bOK;
  if (!fread_trnheader(p->file, &p->header, &bOK) || !bOK)
    THROWS("Failed to read TRN header: " << filename);

  p->x = (rvec *)malloc(p->header.natoms * sizeof(rvec));
#endif
}


bool TRNReader::read(Positions &positions, Topology *topology) {
#if HAVE_GROMACS
  if (setjmp(gromacs_error_handler())) THROW("Failed to read TRN");

  matrix box;
  if (!fread_htrn(p->file, &p->header, box, p->x, 0, 0))
    return false;

  // Bounds
  vector<Vector3D> _box(3);
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      _box[i][j] = box[i][j] * 10;
  positions.setBox(_box);

  // Get number of atoms
  unsigned count;
  if (topology) count = topology->getAtoms().size();
  else count = p->header.natoms;

  for (unsigned i = 0; i < count; i++) {
    // Get atom index
    unsigned index = 0;
    if (topology) index = topology->getAtoms()[i].getIndex();
    if (!index) index = i;

    // Get position
    Vector3D pos = Vector3D(((rvec *)p->x)[index][0],
                            ((rvec *)p->x)[index][1],
                            ((rvec *)p->x)[index][2]) * 10;

    // Store the position
    positions.push_back(pos);
  }

  return true;

#else
  THROWS("Not compiled with Gromacs support");
#endif
}


void TRNReader::read(Trajectory &trajectory) {
  while (true) {
    SmartPointer<Positions> positions = new Positions;
    if (!read(*positions, trajectory.getTopology().get())) break;
    trajectory.add(positions);
  }
}
