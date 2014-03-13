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

#include "XTCReader.h"
#include "GromacsErrorHandler.h"

#include <fah/viewer/Positions.h>
#include <fah/viewer/Topology.h>
#include <fah/viewer/Trajectory.h>

#include <cbang/Exception.h>
#include <cbang/log/Logger.h>
#include <cbang/geom/Vector.h>

#if defined(HAVE_GROMACS)
extern "C" {
#include <gromacs/xtcio.h>
}
#endif

using namespace std;
using namespace cb;
using namespace FAH;


namespace FAH {
  struct XTCReader::private_t  {
    t_fileio *file;
    rvec *x;

    private_t() : file(0), x(0) {}

    ~private_t() {
#if HAVE_GROMACS
      if (setjmp(gromacs_error_handler())) return; // Cannot throw here

      if (file) close_xtc(file);
      file = 0;
      if (x) free(x);
      x = 0;
#endif
    }
  };
}


XTCReader::XTCReader(const string &filename) :
  filename(filename), first(true), p(new private_t) {

#if HAVE_GROMACS
  if (setjmp(gromacs_error_handler())) THROW("Failed to read XTC");

  if (!(p->file = open_xtc(filename.c_str(), "r")))
    THROWS("Failed to open XTC file: " << filename);
#endif
}


bool XTCReader::read(Positions &positions, Topology *topology) {
#if HAVE_GROMACS
  if (setjmp(gromacs_error_handler())) THROW("Failed to read XTC");

  int step = 0;
  real time = 0;
  matrix box;
  real prec = 0;
  gmx_bool bOK = 0;
  int ret;

  if (first) {
    ret = read_first_xtc(p->file, &natoms, &step, &time, box, &p->x, &prec,
                         &bOK);
     first = false;

  } else
    ret = read_next_xtc(p->file, natoms, &step, &time, box, p->x, &prec, &bOK);

  if (!ret) return false;

  Vector3D bounds = Vector3D(box[0][0], box[1][1], box[2][2]) * 10;
  LOG_DEBUG(3, "Read XTC frame: ret=" << ret << " natoms=" << natoms
            << " step=" << step << " time=" << time << " box=" << bounds
            << " prec=" << prec << " bOK=" << bOK);

  vector<Vector3D> _box(3);
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      _box[i][j] = box[i][j] * 10;
  positions.setBox(_box);

  // Get number of atoms
  unsigned count;
  if (topology) count = topology->getAtoms().size();
  else count = natoms;

  for (unsigned i = 0; i < count; i++) {
    // Get atom index
    unsigned index = 0;
    if (topology) index = topology->getAtoms()[i].getIndex();
    if (!index) index = i;

    // Store the position
    positions.push_back(Vector3D(((rvec *)p->x)[index][0],
                                 ((rvec *)p->x)[index][1],
                                 ((rvec *)p->x)[index][2]) * 10);
  }

  return true;

#else
  THROWS("Not compiled with Gromacs support");
#endif
}


void XTCReader::read(Trajectory &trajectory) {
  while (true) {
    SmartPointer<Positions> positions = new Positions;
    if (!read(*positions, trajectory.getTopology().get())) break;
    trajectory.add(positions);
  }
}
