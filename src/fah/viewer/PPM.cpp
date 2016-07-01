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

#include "PPM.h"

#include <cbang/Exception.h>

#include <cbang/log/Logger.h>

#include <cbang/os/SystemUtilities.h>

#include <ctype.h>

using namespace std;
using namespace cb;
using namespace FAH;


PPM::PPM(const uint8_t *data, uint64_t length) :
  width(0), height(0), maxColor(0), raster(0), data(0) {
  parse(data, length);
}


PPM::PPM(const string &filename) 
  : width(0), height(0), maxColor(0), raster(0), data(0) {
  uint64_t length = SystemUtilities::getFileSize(filename);
  data = new uint8_t[length];

  SystemUtilities::open(filename, ios::in)->read((char *)data, length);

  parse(data, length);
}


PPM::~PPM() {
  if (data) delete [] data;
}


void PPM::parse(const uint8_t *data, uint64_t length) {
  if (*data++ != 'P' || *data++ != '6') THROW("Invalid PPM magic");

  if (!isspace(*data)) THROW("Invalid PPM header before width");
  while (isspace(*data)) data++;

  // Comment
  if (*data == '#') {
    while (*data != '\n') data++;
    while (isspace(*data)) data++;
  }

  // Width
  while (isdigit(*data)) width = width * 10 + *data++ - '0';

  if (!isspace(*data)) THROW("Invalid PPM header before height");
  while (isspace(*data)) data++;

  // Height
  while (isdigit(*data)) height = height * 10 + *data++ - '0';

  if (!isspace(*data)) THROW("Invalid PPM header before max color");
  while (isspace(*data)) data++;

  // Comment
  if (*data == '#') {
    while (*data != '\n') data++;
    while (isspace(*data)) data++;
  }

  // Max color
  while (isdigit(*data)) maxColor = maxColor * 10 + *data++ - '0';

  if (!isspace(*data)) THROW("Invalid PPM header before raster");

  // Raster
  raster = data + 1;
}
