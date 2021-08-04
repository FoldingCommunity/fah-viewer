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

#pragma once

#include <cbang/StdTypes.h>

#include <string>

namespace FAH {
  class PPM {
    unsigned width;
    unsigned height;
    unsigned maxColor;
    const uint8_t *raster;

    const uint8_t *data;

  public:
    PPM(const uint8_t *data, uint64_t length);
    PPM(const std::string &filename);
    ~PPM();

    unsigned getWidth() const {return width;}
    unsigned getHeight() const {return height;}
    unsigned getSize() const {return width * height;}
    const uint8_t *getRaster() const {return raster;}

  protected:
    void parse(const uint8_t *data, uint64_t length);
  };
}
