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

#ifndef CBANG_ENUM_EXPAND
#ifndef FAH_VIEW_MODE_H
#define FAH_VIEW_MODE_H

#define CBANG_ENUM_NAME ViewMode
#define CBANG_ENUM_NAMESPACE FAH
#define CBANG_ENUM_PATH fah/viewer
#define CBANG_ENUM_PREFIX 5
#include <cbang/enum/MakeEnumeration.def>

#endif // FAH_VIEW_MODE_H
#else // CBANG_ENUM_EXPAND

CBANG_ENUM_EXPAND(MODE_SPACE_FILLED,         0)
CBANG_ENUM_EXPAND(MODE_BALL_AND_STICK,       1)
CBANG_ENUM_EXPAND(MODE_STICK,                2)
CBANG_ENUM_EXPAND(MODE_ADV_SPACE_FILLED,     3)
CBANG_ENUM_EXPAND(MODE_ADV_BALL_AND_STICK,   4)
CBANG_ENUM_EXPAND(MODE_ADV_STICK,            5)
CBANG_ENUM_EXPAND(MODE_TOON_SPACE_FILLED,    6)
CBANG_ENUM_EXPAND(MODE_TOON_BALL_AND_STICK,  7)

#endif // CBANG_ENUM_EXPAND
