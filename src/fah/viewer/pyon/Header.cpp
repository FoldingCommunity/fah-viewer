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

#include "Header.h"

#include <cbang/String.h>

using namespace std;
using namespace cb;
using namespace FAH::PyON;


Header::Header(const string &type, const char *magic, uint32_t version) :
  magic(magic), version(version), type(type), valid(!type.empty()) {}


void Header::write(ostream &stream) const {
  stream << magic << ' ' << version << ' ' << type;
}


void Header::read(istream &stream) {
  vector<string> tokens;
  String::tokenizeLine(stream, tokens);

  if (tokens.size() != 3 || tokens[0] != magic) valid = false;
  else {
    version = String::parseU32(tokens[1]);
    type = tokens[2];
    valid = true;
  }
}
