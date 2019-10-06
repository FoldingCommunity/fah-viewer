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

#include <cbang/json/Writer.h>


namespace FAH {
  namespace PyON {
    class Writer : public cb::JSON::Writer {
      /***
       * The differences between JSON (Javascript Object Notation) mode and
       * Python mode are as follows:
       *
       *                       |  JSON                |  Python
       *-----------------------------------------------------------------
       * Boolean Literals      |  'true' & 'false'    |  'True' & 'False'
       * Empty set literal     |  'null'              |  'None'
       * Trailing comma        |  not allowed         |  allowed
       * Single quoted strings |  not allowed         |  allowed
       * Unquoted names        |  not allowed         |  allowed
       *
       * Python also allows other forms of escaped strings.
       * Also note that Javascript is more permissive than JSON.
       * See http://www.json.org/ for more info.
       */

    public:
      Writer(std::ostream &stream, unsigned level = 0, bool compact = false) :
        cb::JSON::Writer(stream, level, compact) {}


      // From cb::JSON::Writer
      void writeNull() {
        NullSink::writeNull();
        stream << "None";
      }


      void writeBoolean(bool value) {
        NullSink::writeBoolean(value);
        stream << (value ? "True" : "False");
      }


      void write(const std::string &value) {
        NullSink::write(value);
        stream << '"' << escape(value, "\\x%02x") << '"';
      }
    };
  }
}
