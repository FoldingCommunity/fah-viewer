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

#include "Bond.h"

#include <cbang/json/List.h>

#include <cbang/String.h>

using namespace std;
using namespace cb;
using namespace FAH;


SmartPointer<JSON::Value> Bond::getJSON() const {
  SmartPointer<JSON::Value> list = new JSON::List;

  list->append(left);
  list->append(right);

  return list;
}


void Bond::loadJSON(const JSON::Value &value) {
  auto &list = value.getList();

  if (list.size() != 2) THROW("Bond expected list of length 2");
  left = (uint32_t)list[0]->getNumber();
  right = (uint32_t)list[1]->getNumber();
}
