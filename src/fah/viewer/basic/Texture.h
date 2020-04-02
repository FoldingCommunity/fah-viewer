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

#ifndef FAH_TEXTURE_H
#define FAH_TEXTURE_H

#include <string>

namespace FAH {
  class Texture {
    const std::string name;

    int width;
    int height;

    float alpha;

    unsigned id;

    bool loaded;

  public:
    Texture(const std::string &name, int width = 0, int height = 0,
            float alpha = 0);
    virtual ~Texture() {release();}

    const std::string &getName() const {return name;}
    int getWidth() const {return width;}
    int getHeight() const {return height;}
    float getAlpha() const {return alpha;}
    unsigned getID() const {return id;}

    virtual void load();
    virtual void release();
    virtual void draw(float x = 0, float y = 0, float w = 0, float h = 0) const;
  };
}

#endif // FAH_TEXTURE_H

