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

#include "Topology.h"
#include "Positions.h"

#include <cbang/Exception.h>
#include <cbang/log/Logger.h>
#include <cbang/json/JSON.h>

#include <limits>

// Windows needs this
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

using namespace std;
using namespace cb;
using namespace FAH;


void Topology::validate(const Positions &positions) const {
  if (positions.size() != atoms.size())
    THROWS("Number of positions (" << positions.size() << ") and atoms ("
           << atoms.size() << ") do not agree");
}


void Topology::clear() {
  atoms.clear();
  bonds.clear();
}


unsigned Topology::findBonds(vector<unsigned> &bondCounts,
                             const Positions &positions) {
  unsigned count = 0;

  validate(positions);

  for (unsigned i = 0; i < atoms.size(); i++) {
    if (!bondCounts[i]) continue;

    bool bondAdded = false;
    double minDist = numeric_limits<double>::max();

    for (unsigned j = 0; j < atoms.size(); j++) {
      if (i == j || !bondCounts[j]) continue; // Exclude self

      double dist = positions.at(i).distance(positions.at(j));

      // Check if atoms are too far apart to have a bond
      if (atoms.at(i).averageBondLength(atoms.at(j)) * 1.1 < dist) continue;

      if (dist < minDist) {
        // left < right
        unsigned left = (i > j) ? j : i;
        unsigned right = (i > j) ? i : j;

        // Ignore double and triple bonds
        bool duplicate = false;
        for (unsigned k = 0; k < bonds.size(); k++)
          if (bonds.at(k).left == left && bonds.at(k).right == right) {
            duplicate = true;
            break;
          }
        if (duplicate) continue;

        minDist = dist;

        if (!bondAdded) {
          bonds.push_back(Bond());
          bondAdded = true;
        }

        bonds.at(bonds.size() - 1) = Bond(left, right); // Set bond
      }
    }

    if (bondAdded) {
      const Bond &bond = bonds.at(bonds.size() - 1);
      bondCounts[bond.left]--;
      bondCounts[bond.right]--;
      count += 2;
    }
  }

  return count;
}


void Topology::findBonds(const Positions &positions) {
  bonds.clear();

  // Set max bond counts
  vector<unsigned> bondCounts(atoms.size()); // Zeroed
  unsigned total = 0;
  for (unsigned i = 0; i < atoms.size(); i++) {
    switch (atoms.at(i).getNumber()) {
    case Atom::HYDROGEN: bondCounts[i] = 1; break;
    case Atom::OXYGEN:   bondCounts[i] = 2; break;
    case Atom::NITROGEN: bondCounts[i] = 4; break;
    case Atom::CARBON:   bondCounts[i] = 4; break;
    case Atom::SULFUR:   bondCounts[i] = 4; break;
    }

    total += bondCounts[i];
  }

  unsigned remaining = total;
  unsigned lastRemaining = remaining;
  for (int i = 0; i < 100 && 0 < remaining; i++) {
    remaining -= findBonds(bondCounts, positions);

    if (remaining == lastRemaining) break;
    lastRemaining = remaining;
  }

  if (remaining)
    LOG_DEBUG(3, remaining << " of " << total << " bonds not found");
}


SmartPointer<JSON::Value> Topology::getJSON() const {
  SmartPointer<JSON::Dict> dict = new JSON::Dict;

  // Atoms
  SmartPointer<JSON::List> list = new JSON::List;
  for (atoms_t::const_iterator it = atoms.begin(); it != atoms.end(); it++)
    list->push_back(it->getJSON());
  dict->insert("atoms", list);

  // Bonds
  list = new JSON::List;
  for (bonds_t::const_iterator it = bonds.begin(); it != bonds.end(); it++)
    list->push_back(it->getJSON());
  dict->insert("bonds", list);  

  return dict;
}


void Topology::loadJSON(const JSON::Value &value, float scale) {
  clear();

  // Atoms
  if (value.has("atoms")) {
    const JSON::List &atoms = value.getList("atoms");
    for (unsigned i = 0; i < atoms.size(); i++) {
      if (atoms.getList(i).getString(0) == "UNKNOWN") break;
      this->atoms.push_back(Atom(atoms.getList(i), scale));
    }
  }

  // Bonds
  if (value.has("bonds")) {
    const JSON::List &bonds = value.getList("bonds");
    for (unsigned i = 0; i < bonds.size(); i++) {
      if (atoms.size() <= bonds.getList(i).getNumber(0) ||
          atoms.size() <= bonds.getList(i).getNumber(1)) continue;
      this->bonds.push_back(Bond(bonds.getList(i)));
    }
  }
}
