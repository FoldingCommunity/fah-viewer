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

#ifndef FAH_ATOM_H
#define FAH_ATOM_H

#include <fah/viewer/pyon/Object.h>

#include <cbang/StdTypes.h>
#include <cbang/geom/Vector.h>

#include <iostream>
#include <string>


namespace FAH {
  class Atom : public PyON::Object {
  public:
    enum {
      HYDROGEN = 1,
      CARBON = 6,
      NITROGEN = 7,
      OXYGEN = 8,
      SULFUR = 16,
      HEAVY = 999,
    };

  protected:
    std::string type;
    unsigned number;
    float charge;
    float radius;
    float mass;
    unsigned index;

  public:
    Atom() : number(0), charge(0), radius(0), mass(0), index(0) {}
    Atom(const cb::JSON::Value &value, float scale = 1) :
      number(0), charge(0), radius(0), mass(0), index(0)
    {loadJSON(value, scale);}
    Atom(const std::string &type, float charge = 0, float radius = 0,
         float mass = 0, unsigned number = 0);

    const std::string &getType() const {return type;}
    void setType(const std::string &type) {this->type = type;}

    unsigned getNumber() const {return number;}
    void setNumber(unsigned number) {this->number = number;}

    float getCharge() const {return charge;}
    float getRadius() const {return radius;}
    float getMass() const {return mass;}

    void setIndex(unsigned index) {this->index = index;}
    unsigned getIndex() const {return index;}

    static float chargeFromNumber(unsigned number);
    static float radiusFromNumber(unsigned number);
    static float massFromNumber(unsigned number);
    static unsigned numberFromName(const std::string &name);

    void setDataFromNumber(unsigned number);

    double averageBondLength(const Atom &atom) const;

    // From PyONObject
    const char *getPyONType() const {return "atom";}
    cb::SmartPointer<cb::JSON::Value> getJSON() const;
    void loadJSON(const cb::JSON::Value &value) {loadJSON(value, 1);}

    void loadJSON(const cb::JSON::Value &value, float scale);
  };
}

#endif // FAH_ATOM_H

