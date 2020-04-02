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

#include "Box.h"

#include <cbang/Exception.h>
#include <fah/viewer/GL.h>

using namespace cb;
using namespace FAH;


Box::Box(float alpha) :
  left("box_left", 0, 0, alpha), top("box_top", 0, 0, alpha),
  right("box_right", 0, 0, alpha), bottom("box_bottom", 0, 0, alpha),
  middle("box_middle", 0, 0, alpha),
  tl("box_tl", 0, 0, alpha), tr("box_tr", 0, 0, alpha),
  br("box_br", 0, 0, alpha), bl("box_bl", 0, 0, alpha) {}


void Box::load() {
  left.load(); top.load(); right.load(); bottom.load();
  middle.load();
  tl.load(); tr.load(); br.load(); bl.load();

  // Check dimensions
  if (tl.getWidth() != tr.getWidth() || tl.getWidth() != br.getWidth() ||
      tl.getWidth() != bl.getWidth() || tl.getWidth() != right.getWidth() ||
      tl.getWidth() != left.getWidth())
    THROW("Box widths don't match");

  if (tl.getHeight() != tr.getHeight() || tl.getHeight() != br.getHeight() ||
      tl.getHeight() != bl.getHeight() || tl.getHeight() != top.getHeight() ||
      tl.getHeight() != bottom.getHeight())
    THROW("Box heights don't match");
}


void Box::release() {
  left.release(); top.release(); right.release(); bottom.release();
  middle.release();
  tl.release(); tr.release(); br.release(); bl.release();
}


void Box::draw(float width, float height) const {
  // Compute dims
  float w = width / 2;
  float cWidth = tl.getWidth() < w ? tl.getWidth() : w;
  w = width - 2 * cWidth;
  if (w < 0) w = 0;

  float h = height / 2;
  float cHeight = tl.getHeight() < w ? tl.getHeight() : w;
  h = height - 2 * cHeight;
  if (h < 0) h = 0;

  // Corners
  tl.draw(0,          cHeight + h, cWidth, cHeight);
  tr.draw(cWidth + w, cHeight + h, cWidth, cHeight);
  bl.draw(0,          0,           cWidth, cHeight);
  br.draw(cWidth + w, 0,           cWidth, cHeight);

  // Middle
  middle.draw(cWidth, cHeight, w, h);

  // Sides
  if (w) {
    top.draw   (cWidth, cHeight + h, w, cHeight);
    bottom.draw(cWidth, 0,           w, cHeight);
  }
  if (h) {
    left.draw (0,          cHeight, cWidth, h);
    right.draw(cWidth + w, cHeight, cWidth, h);
  }
}
