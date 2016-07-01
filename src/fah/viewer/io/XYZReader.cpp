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

#include "XYZReader.h"

#include <fah/viewer/Positions.h>
#include <fah/viewer/Topology.h>

#include <cbang/Exception.h>
#include <cbang/String.h>

#include <vector>
#include <string>

using namespace std;
using namespace cb;
using namespace FAH;


void XYZReader::read(Positions &positions, Topology *topology) {
  istream &stream = source.getStream();
  vector<string> tokens;
  char line[1024];

  // Read header line
  stream.getline(line, 1024);
  if (stream.fail()) THROWS("Failed to read XYZ");

  // Get atom count
  String::tokenize(line, tokens);
  if (tokens.size() < 1) THROW("Missing atom count in XYZ");

  unsigned count = String::parseU32(tokens[0]);
  unsigned lineNum = 1;

  // Reset
  positions.clear();
  if (topology) topology->clear();

  // Read atoms and positions
  while (!stream.fail() && count) {
    stream.getline(line, 1024);
    lineNum++;

    tokens.clear();
    String::tokenize(line, tokens);

    if (tokens.size() < 1 || !isdigit(tokens[0][0])) continue;
    if (tokens.size() < 5) THROWS("Invalid XYZ file at line: " << lineNum);

    if (topology) {
      Atom atom(tokens[1]);
      atom.setIndex(String::parseU32(tokens[0]));
      topology->add(atom);
    }

    positions.push_back(Vector3D(String::parseDouble(tokens[2]),
                                 String::parseDouble(tokens[3]),
                                 String::parseDouble(tokens[4])));

    count--;
  }

  positions.init();

  if (count) THROWS("Failed reading XYZ, expected " << count << " more atoms");
}
