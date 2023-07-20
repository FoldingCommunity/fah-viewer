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

#include <iostream>
#include <string>
#include <cstdint>

#define FAH_PYON_MAGIC "PyON"
#define FAH_PYON_VERSION 1


namespace FAH {
  namespace PyON {
    class Header {
      const char *magic;
      uint32_t version;
      std::string type;
      bool valid;

    public:
      Header(const std::string &type = std::string(),
             const char *magic = FAH_PYON_MAGIC,
             uint32_t version = FAH_PYON_VERSION);

      const char *getMagic() const {return magic;}
      uint32_t getVersion() const {return version;}
      const std::string &getType() const {return type;}
      bool isValid() const {return valid;}

      void write(std::ostream &stream) const;
      void read(std::istream &stream);
    };

    static inline
    std::ostream &operator<<(std::ostream &stream, const Header &header) {
      header.write(stream); return stream;
    }

    static inline
    std::istream &operator>>(std::istream &stream, Header &header) {
      header.read(stream); return stream;
    }
  }
}
