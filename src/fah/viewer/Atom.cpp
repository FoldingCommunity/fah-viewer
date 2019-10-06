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

#include "Atom.h"

#include <cbang/json/List.h>

#include <cbang/Exception.h>
#include <cbang/String.h>

#include <cctype>

using namespace std;
using namespace cb;
using namespace FAH;


Atom::Atom(const string &type, float charge, float radius, float mass,
           unsigned number) :
  type(type), number(number ? number : numberFromName(type)),
  charge(charge), radius(radius), mass(mass), index(0) {
  if (!charge) this->charge = chargeFromNumber(number);
  if (radius <= 0) this->radius = radiusFromNumber(number);
  if (mass <= 0) this->mass = massFromNumber(number);
}


float Atom::chargeFromNumber(unsigned number) {
  switch (number) {
  case HYDROGEN: return  0.370;
  case CARBON:   return -0.119;
  case NITROGEN: return -0.333;
  case OXYGEN:   return -0.380;
  case SULFUR:   return -0.470;
  default:       return  0.370; // ???
  }
}


float Atom::radiusFromNumber(unsigned number) {
  switch (number) {
  case HYDROGEN: return 1.09;
  case CARBON:   return 1.70;
  case NITROGEN: return 1.55;
  case OXYGEN:   return 1.52;
  case SULFUR:   return 1.80;
  default:       return 1.00; // ???
  }
}


float Atom::massFromNumber(unsigned number) {
  // TODO This isn't quite right
  switch (number) {
  case HYDROGEN: return 1.00794;
  case CARBON:   return 12.0107;
  case NITROGEN: return 14.0067;
  case OXYGEN:   return 15.9994;
  case SULFUR:   return 32.0650;
  default:       return 100; // Assume a heavy atom
  }
}


void Atom::setDataFromNumber(unsigned number) {
  if (!number) THROW("Atom number cannot be zero");

  charge = chargeFromNumber(number);
  radius = radiusFromNumber(number);
  mass = massFromNumber(number);
}

unsigned Atom::numberFromName(const string &name) {
  if (name.empty()) THROW("Atom name cannot be empty");

  switch (toupper(name[0])) {
  case 'H': return HYDROGEN;
  case 'C': return CARBON;
  case 'N': return NITROGEN;
  case 'O': return OXYGEN;
  case 'S': return SULFUR;
  default:
    if (1 < name.length()) return numberFromName(name.substr(1));
    return HEAVY;
  }
}


static int numberToIndex(char number) {
  switch (number) {
  case Atom::HYDROGEN: return 0;
  case Atom::CARBON:   return 1;
  case Atom::NITROGEN: return 2;
  case Atom::OXYGEN:   return 3;
  case Atom::SULFUR:   return 4;
  default: return -1;
  }
}


double Atom::averageBondLength(const Atom &atom) const {
  // See: http://www.wiredchemist.com/chemistry/data/bond_energies_lengths.html
  const double table[][5] = {
    // H     C     N     O     S
    {0.74, 1.09, 1.01, 0.96, 1.34}, // H
    {1.09, 1.54, 1.47, 1.43, 1.82}, // C
    {1.01, 1.47, 1.45, 1.40, 1.43}, // N
    {0.96, 1.43, 1.40, 1.48, 1.43}, // O
    {1.34, 1.82, 1.43, 1.43, 1.49}, // S
  };

  int i = numberToIndex(number);
  int j = numberToIndex(atom.number);

  if (i == -1 || j == -1) return 2; // a wild guess

  return table[i][j];
}


SmartPointer<JSON::Value> Atom::getJSON() const {
  SmartPointer<JSON::Value> list = new JSON::List;

  list->append(type);
  list->append(charge);
  list->append(radius);
  list->append(mass);
  list->append(number);

  return list;
}


void Atom::loadJSON(const JSON::Value &value, float scale) {
  if (0 < value.size()) type = value.getString(0);
  else THROW("Atom expected list of at least length 1");
  if (String::toUpper(type) == "UNKNOWN") type = "?";

  if (1 < value.size()) charge = (float)value.getNumber(1);
  if (2 < value.size()) radius = (float)value.getNumber(2) * scale;
  if (3 < value.size()) mass = (float)value.getNumber(3);
  if (4 < value.size()) number = (unsigned)value.getNumber(4);
  else number = numberFromName(type);

  if (!number) number = numberFromName(type);
  if (!charge) charge = chargeFromNumber(number);
  if (!radius) radius = radiusFromNumber(number);
  if (!mass) mass = massFromNumber(number);
}
