/******************************************************************************\

                       This file is part of the FAHViewer.

            The FAHViewer displays 3D views of Folding@home proteins.
                    Copyright (c) 2016-2020, foldingathome.org
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

#ifndef FAH_XYZWRITER_H
#define FAH_XYZWRITER_H

#include <cbang/io/OutputSink.h>


namespace FAH {
  class Protein;
  class Positions;
  class Topology;

  class XYZWriter {
    const cb::OutputSink sink;

  public:
    XYZWriter(const cb::OutputSink &sink) : sink(sink) {}

    void write(const Positions &positions, const Topology &topology);
    void write(const Protein &protein);
  };
}

#endif // FAH_XYZWRITER_H

