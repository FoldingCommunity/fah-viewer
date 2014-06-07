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

#include "TPRReader.h"
#include "GromacsErrorHandler.h"

#include <fah/viewer/Positions.h>
#include <fah/viewer/Topology.h>

#include <cbang/Exception.h>
#include <cbang/geom/Vector.h>
#include <cbang/log/Logger.h>

#if defined(HAVE_GROMACS)
extern "C" {
#include <gromacs/tpxio.h>
}
#endif

using namespace std;
using namespace cb;
using namespace FAH;


void TPRReader::read(Positions &positions, Topology &topology) {
#if HAVE_GROMACS
  if (setjmp(gromacs_error_handler())) THROW("Failed to read TPR");

  // Load header
  t_tpxheader tpx;
  read_tpxheader(filename.c_str(), &tpx, TRUE, 0, 0);

  if (!tpx.bX) THROWS("No positions in " << filename);
  if (!tpx.bTop) THROWS("No topology in " << filename);

  // Load state & topology
  t_state state;
  rvec *f = 0;
  t_inputrec ir;
  gmx_mtop_t mtop;

  read_tpx_state(filename.c_str(), tpx.bIr ? &ir : 0, &state, tpx.bF ? f : 0,
                 tpx.bTop ? &mtop : 0);

  try {
    if (!state.x)
      THROWS("Positions array empty in " << filename << ", you may need to "
             "link with the double precision Gromacs libraries");

    // Reset
    positions.clear();
    topology.clear();

    // Save box
    if (tpx.bBox) {
      vector<Vector3D> box(3);
      for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
          box[i][j] = state.box[i][j] * 10;

      positions.setBox(box);
    }

    // Loop over all molecules in the topology
    t_atomtypes &atomtypes = mtop.atomtypes;
    unsigned count = 0;
    for (int i = 0; i < mtop.nmoltype; i++) {
      gmx_moltype_t &moltype = mtop.moltype[i];
      t_atoms &atoms = moltype.atoms;
      string name = *moltype.name;

      LOG_DEBUG(5, "Moltype: " << name << " size=" << atoms.nr);

      // Skip over non-proteins
      bool skip = true;
      for (int j = 0; j < atoms.nr; j++) {
        t_atom &atom = atoms.atom[j];

        int number = atom.atomnumber;
        if (number <= 0) number = Atom::numberFromName(*atoms.atomname[j]);

        switch (number) {
        case Atom::CARBON: case Atom::NITROGEN: case Atom::SULFUR:
          skip = false;
          break;

        case Atom::HYDROGEN: case Atom::OXYGEN: break;

        default:
          LOG_DEBUG(5, "Atom: '" << *atoms.atomname[j] << "' "
                    << atom.atomnumber);
          break;
        }
      }

      if (skip) {
        count += atoms.nr;
        continue;
      }

      // Loop over atoms in molecule
      unsigned firstAtom = topology.getAtoms().size();
      for (int j = 0; j < atoms.nr; j++) {
        int number = atoms.atom[j].atomnumber;
        float radius = atomtypes.gb_radius[atoms.atom[j].type] * 10;
        float charge = atoms.atom[j].q;
        float mass = atoms.atom[j].m;

        // Atom
        Atom atom(*atoms.atomname[j], charge, radius, mass);
        if (0 < number) atom.setNumber(number);
        atom.setIndex(count);
        topology.add(atom);

        // Position
        Vector3D p = Vector3D(state.x[count][0], state.x[count][1],
                              state.x[count][2]) * 10;
        positions.push_back(p);

        LOG_DEBUG(5, "TPR: " << j << '/' << atoms.nr << ' ' << p);
        count++;
      }

      // Get chemical bonds
      for (int k = 0; k < F_NRE; k++) {
        if (!IS_CHEMBOND(k)) continue;
        t_ilist &ilist = moltype.ilist[k];

        for (int j = 0; j < ilist.nr; j += 3)
          topology.add(Bond(firstAtom + ilist.iatoms[j + 1],
                            firstAtom + ilist.iatoms[j + 2]));
      }
    }

  } catch (...) {
    done_state(&state);
    throw;
  }

  done_state(&state);

#else
  THROWS("Not compiled with Gromacs support");
#endif
}
