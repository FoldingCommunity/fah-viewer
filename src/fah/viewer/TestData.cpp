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

#include "TestData.h"
#include "Positions.h"
#include "Trajectory.h"

#include <fah/viewer/io/XYZReader.h>

#include <cbang/String.h>
#include <cbang/util/Resource.h>


using namespace std;
using namespace cb;
using namespace FAH;

namespace FAH {
  namespace Viewer {
    extern const DirectoryResource resource0;
  }
}


void TestData::load(Trajectory &trajectory) {
  for (unsigned i = 0; true; i++) {
    string filename = String::printf("snapshot%d.xyz", i);
    const Resource *resource = FAH::Viewer::resource0.find(filename);
    if (!resource) break;

    SmartPointer<Positions> positions = new Positions;
    // Only load the topology the first time
    XYZReader(*resource).read(*positions,
                              i ? 0 : trajectory.getTopology().get());
    trajectory.add(positions);
  }
}
