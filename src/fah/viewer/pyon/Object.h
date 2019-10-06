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

#pragma once

#include <cbang/SmartPointer.h>
#include <cbang/json/Value.h>

#include <iostream>


namespace FAH {
  namespace PyON {
    class Object {
    public:
      virtual ~Object() {}

      virtual const char *getPyONType() const = 0;
      virtual cb::SmartPointer<cb::JSON::Value> getJSON() const = 0;
      virtual void loadJSON(const cb::JSON::Value &value) = 0;

      virtual void write(std::ostream &stream) const;
      virtual void read(std::istream &stream);
    };


    static inline
    std::ostream &operator<<(std::ostream &stream, const Object &obj) {
      obj.write(stream); return stream;
    }

    static inline
    std::istream &operator>>(std::istream &stream, Object &obj) {
      obj.read(stream); return stream;
    }
  }
}
