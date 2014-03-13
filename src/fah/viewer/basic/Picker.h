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

#ifndef FAH_PICKER_H
#define FAH_PICKER_H

#include <cbang/geom/Rectangle.h>

#include <map>
#include <string>

namespace FAH {
  /// Dumb picker implementation
  class Picker {
    typedef std::map<std::string, cb::Rectangle2D> regions_t;
    regions_t regions;

  public:
    void set(const std::string &name, const cb::Rectangle2D &bounds);
    std::string pick(const cb::Vector2D &v) const;
  };
}

#endif // FAH_PICKER_H

