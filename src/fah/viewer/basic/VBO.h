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

#ifndef FAH_VBO_H
#define FAH_VBO_H

#include <cbang/SmartPointer.h>


namespace FAH {
  class VBO {
    unsigned size;

    unsigned vert;
    unsigned text;
    unsigned norm;

    bool textured;

    cb::SmartPointer<float>::Array vertData;
    cb::SmartPointer<float>::Array textData;
    cb::SmartPointer<float>::Array normData;

  public:
    VBO(bool textured) :
    size(0), vert(0), text(0), norm(0), textured(textured) {}
    virtual ~VBO();

    virtual void draw() = 0;

    void bind();
    void unbind();

    void loadArrays(unsigned size,
                    const cb::SmartPointer<float>::Array &vertData,
                    const cb::SmartPointer<float>::Array &textData,
                    const cb::SmartPointer<float>::Array &normData);
  };
}

#endif // FAH_VBO_H

