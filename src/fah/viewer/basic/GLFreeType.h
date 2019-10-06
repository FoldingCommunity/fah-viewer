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

#ifndef FAH_GL_FREE_TYPE_H
#define FAH_GL_FREE_TYPE_H

#include <cbang/geom/Vector.h>

#include <string>

struct FT_FaceRec_;
typedef struct FT_FaceRec_ *FT_Face;

// Wrap everything in a namespace, that we can use common
// function names like "print" without worrying about
// overlapping with anyone else's code.
namespace FAH {
  // This holds all of the information related to any
  // freetype font that we want to create.
  class GLFreeType {
    float h; //< Holds the height of the font
    float lineHeight;
    unsigned textures[128]; //< Holds the texture id's
    unsigned widths[128]; //< Holds the character widths
    unsigned listBase; //< Holds the first display list id

  public:
    // The init function will create a font of
    // of the height h from the file fname.
    GLFreeType(const std::string &fname, unsigned h, float lineHeight = 1.75);

    // Free all the resources assosiated with the font
    ~GLFreeType();

    float getLineHeight() const {return lineHeight * h;}

    cb::Vector2D dimensions(const std::string &s) const;
    float width(const std::string &s) const;
    float height(const std::string &s) const;

    // The flagship function of the library - this thing will print
    // out text at window coordinates x,y, using the font ft_font.
    // The current modelview matrix will also be applied to the text.
    void print(float x, float y, const std::string &s,
               unsigned center = 0) const;

  protected:
    void displayList(FT_Face face, unsigned char ch);
  };
}

#endif // FAH_GL_FREE_TYPE_H
