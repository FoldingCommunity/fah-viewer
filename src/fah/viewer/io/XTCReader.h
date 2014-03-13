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

#ifndef FAH_XTC_READER_H
#define FAH_XTC_READER_H

#include <cbang/SmartPointer.h>


namespace FAH {
  class Positions;
  class Topology;
  class Trajectory;

  class XTCReader {
    const std::string &filename;
    bool first;
    int natoms;

    struct private_t;
    cb::SmartPointer<private_t> p;

  public:
    XTCReader(const std::string &filename);

    bool read(Positions &positions, Topology *topology = 0);
    void read(Trajectory &trajectory);
  };
}

#endif // FAH_XTC_READER_H

