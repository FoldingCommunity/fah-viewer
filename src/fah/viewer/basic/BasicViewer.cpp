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

#include "BasicViewer.h"

#include <cbang/Exception.h>
#include <cbang/Zap.h>
#include <cbang/String.h>
#include <cbang/Math.h>
#include <cbang/SStream.h>

#include <cbang/time/TimeInterval.h>

#include <cbang/util/DefaultCatch.h>
#include <cbang/util/Random.h>
#include <cbang/util/Resource.h>

#include <cbang/geom/Vector.h>
#include <cbang/geom/Rectangle.h>

#include <fah/viewer/GL.h>
#include <fah/viewer/View.h>

#include <cctype>

// Windows needs this
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

using namespace std;
using namespace cb;
using namespace FAH;

namespace FAH {
  namespace Viewer {
    extern const DirectoryResource resource0;
  }
}


BasicViewer::BasicViewer() :
  mode(MODE_SPACE_FILLED), fontsLoaded(false), font(0), fontBold(0), box(0.6),
  darkBox(0.8), cdLogo("cauldron_logo", 128, 48, 1),
  fahLogo("FAH_logo2", 96, 96, 1), popupYOffset(0), popupPageHeight(0),
  popupLineHeight(21), initialized(false) {

  logos.push_back(new Texture("medschool_logo", 128, 48, 0.9));
  logos.push_back(new Texture("FAH_logo",       128, 64, 0.9));

  const unsigned bSize = 48;
  buttons.push_back(new Texture("help",        bSize, bSize, 0.9));
  buttons.push_back(new Texture("about",       bSize, bSize, 0.9));
}


BasicViewer::~BasicViewer() {
  release();
  zap(font);
  zap(fontBold);
}


void BasicViewer::loadFonts() {
  if (fontsLoaded) return;
  fontsLoaded = true;

  try {
    fontBold = new GLFreeType("Courier_New_Bold.ttf", 16);
  } CBANG_CATCH_ERROR;

  try {
    font = new GLFreeType("Courier_New.ttf", 12);
  } CBANG_CATCH_ERROR;
}


void BasicViewer::print(unsigned x, unsigned y, const string &s, bool bold) {
  loadFonts();
  (bold ? fontBold : font)->print(x, y, s);
}


void BasicViewer::resetDraw(const View &view) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, view.getWidth(), 0, view.getHeight());
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glFrontFace(GL_CCW);
  glColor3f(1, 1, 1);
}


void BasicViewer::setMaterial(const Atom &atom) {
  static const float shine[] = {
    60, 20, 25, 30, 30, 100,
  };

  static const float specular[][4] = {
    {0.45, 0.45, 0.50, 1.00}, // Carbon
    {0.20, 0.20, 0.20, 1.00}, // Hydrogen
    {0.20, 0.20, 0.20, 1.00}, // Nitrogen
    {0.20, 0.20, 0.20, 1.00}, // Oxygen
    {0.20, 0.20, 0.20, 1.00}, // Sulfur
    {0.25, 0.50, 0.25, 1.00}, // Heavy atoms
  };

  static const float material[][4] = {
    {0.20, 0.20, 0.20, 1.00}, // dark grey
    {0.60, 0.60, 0.60, 1.00}, // grey
    {0.10, 0.10, 0.80, 1.00}, // blue
    {0.80, 0.15, 0.15, 1.00}, // red
    {0.60, 0.60, 0.15, 1.00}, // yellow
    {0.50, 0.00, 0.60, 1.00}, // purple
  };

  int i;
  switch (atom.getNumber()) {
  case Atom::CARBON:   i = 0; break;
  case Atom::HYDROGEN: i = 1; break;
  case Atom::NITROGEN: i = 2; break;
  case Atom::OXYGEN:   i = 3; break;
  case Atom::SULFUR:   i = 4; break;
  default:             i = 5; break;
  }

  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material[i]);
  glMaterialfv(GL_FRONT, GL_SHININESS, &shine[i]);
  glMaterialfv(GL_FRONT, GL_SPECULAR, specular[i]);
}


void BasicViewer::drawAtom(const Atom &atom, const Vector3D &position) {
  glPushMatrix();
  glTranslatef(position.x(), position.y(), position.z());

  setMaterial(atom);

  // Scale based on atom type
  if (mode != MODE_STICK && mode != MODE_ADV_STICK) {
    float scale = atom.getRadius() / 1.7;
    if (0 < scale) glScalef(scale, scale, scale);
  }

  sphere->draw();

  glPopMatrix();
}


void BasicViewer::drawBond(const Protein &protein, const Bond &bond) {
  const Positions &positions = *protein.getPositions();
  const Topology::atoms_t &atoms = protein.getTopology()->getAtoms();
  const Atom leftAtom = atoms[bond.left];
  const Atom rightAtom = atoms[bond.right];
  const Vector3D &left = positions[bond.left];
  const Vector3D &right = positions[bond.right];
  Vector3D diff = right - left;
  double length = left.distance(right);
  double avgLength = leftAtom.averageBondLength(rightAtom);

  // Don't draw bonds which are too long
  if (avgLength * 2 < length) {
    LOG_DEBUG(3, "Bond too long " << left << "->" << right << " length="
              << length << " avg bond length=" << avgLength);
    return;
  }

  AxisAngleD angle(acos(diff.z() / length) * 57.2957, -diff.y(), diff.x(), 0);

  glPushMatrix();

  glDisable(GL_CULL_FACE);
  glShadeModel(GL_SMOOTH);

  glTranslatef(left.x(), left.y(), left.z());
  glRotatef(angle.angle(), angle.x(), angle.y(), angle.z());

  setupShadow(left, angle); // Used by AdvancedViewer

  if (mode == MODE_STICK || mode == MODE_ADV_STICK) {
    setMaterial(leftAtom);

    glScalef(1, 1, 0.5 * length);

    cylinder->draw();

    glTranslatef(0, 0, 1);
    setupShadow(left + diff / 2.0, angle); // Used by AdvancedViewer

    setMaterial(rightAtom);

    cylinder->draw();

  } else {
    float shine = 20;
    float specular[] = {0.25, 0.25, 0.25, 1.0};
    float diffuse[] = {0.4, 0.4, 0.0, 1.0};

    glMaterialfv(GL_FRONT, GL_SHININESS, &shine);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);

    glScalef(1, 1, length);
    cylinder->draw();
  }

  glEnable(GL_CULL_FACE);
  glPopMatrix();
}


void BasicViewer::drawCuboid(const cb::Rectangle3D &r) {
  glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT);
  glDisable(GL_LIGHTING);
  glLineWidth(1);
  glColor3f(0.5, 0.5, 0.5);

  glBegin(GL_LINES);
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < i; j++) {
      int x = i ^ j;

      if (x == 1 || x == 2 || x == 4) {
        glVertex3f(r[!(i & 1)].x(), r[!(i & 2)].y(), r[!(i & 4)].z());
        glVertex3f(r[!(j & 1)].x(), r[!(j & 2)].y(), r[!(j & 4)].z());
      }
    }

  glEnd();
  glPopAttrib();
}


void BasicViewer::drawBox(const Positions &positions) {
  if (!positions.getBox().empty()) {
    Rectangle3D r(positions.getOffset(), positions.getOffset());
    for (int i = 0; i < 3; i++) r[1][i] += positions.getBox()[i][i];
    drawCuboid(r);
  }
}


void BasicViewer::drawAtoms(const Protein &protein) {
  Positions &positions = *protein.getPositions();
  const Topology::atoms_t &atoms = protein.getTopology()->getAtoms();

  //drawBox(positions);

  sphere->bind();
  for (unsigned i = 0; i < atoms.size(); i++)
    drawAtom(atoms[i], positions[i]);
  sphere->unbind();
}


void BasicViewer::drawBonds(const Protein &protein) {
  const Topology::bonds_t &bonds = protein.getTopology()->getBonds();

  if (mode != MODE_SPACE_FILLED && mode != MODE_ADV_SPACE_FILLED) {
    cylinder->bind();
    for (unsigned i = 0; i < bonds.size(); i++)
      drawBond(protein, bonds[i]);
    cylinder->unbind();
  }
}


void BasicViewer::setupPerspective(const View &view, double radius) {
  radius *= view.getZoom();
  double zNear = 1;
  double zFar = zNear + radius * 4 / view.getZoom();
  double left = -radius;
  double right = radius;
  double bottom = -radius;
  double top = radius;

  double aspect = (double)view.getWidth() / view.getHeight();
  if (aspect < 1) { // window taller than wide
    bottom /= aspect;
    top /= aspect;
  } else {
    left *= aspect;
    right *= aspect;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(left, right, bottom, top, zNear, zFar);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0, 0, zNear + (zFar - zNear) / 2, 0, 0, 0, 0, 1, 0);

  // Rotate
  double rotation[4];
  view.getRotation().toAxisAngle().toGLRotation(rotation);
  glRotated(rotation[0], rotation[1], rotation[2], rotation[3]);
}


void BasicViewer::drawProtein(const Protein &protein, const View &view) {
  glFrontFace(GL_CW);
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  setupPerspective(view, protein.getRadius());

  // Draw
  drawAtoms(protein);
  drawBonds(protein);

  // Cean up
  glPopMatrix();
  glDisable(GL_DEPTH_TEST);

  CHECK_GL_ERROR("");
}


void BasicViewer::drawInfo(const SimulationInfo &info, const View &view) {
  if (!view.getShowInfo()) return;

  glDisable(GL_LIGHTING);

  // Draw the small callout text
  resetDraw(view);
  glTranslatef(view.getWidth() - 264, view.getHeight() - 124, 0);

  box.draw(260, 120);

  glColor3ub(0x73, 0x96, 0xcf);
  print(12, 94, "Donor", true);

  glColor3ub(0x9, 0xa7, 0xb7);
  print(12, 56, "Name:");
  print(66, 56, info.user);

  print(12, 28, "Team:");
  print(66, 28, info.team);

  // Draw the status callout text
  resetDraw(view);
  glTranslatef(4, 4, 0);

  box.draw(260, 142);

  glColor3ub(0x73, 0x96, 0xcf);
  print(12, 116, "Status", true);

  glColor3ub(0x9, 0xa7, 0xb7);
  print(12, 88, "Snapshots:");
  if (info.project)
    print(126, 88, view.getFrameDescription());

  print(12, 66, "Connection:");
  print(126, 66, view.getConnectionStatus());

  print(12, 44, "Protein:");
  print(126, 44, view.getStatus());

  print(12, 22, "Slot:");
  print(126, 22, info.project ? String(info.slot) : "");

  // Draw the large callout text
  resetDraw(view);
  glTranslatef(4, view.getHeight() - 146, 0);

  box.draw(530, 142);

  glColor3ub(0x73, 0x96, 0xcf);
  print(12, 116, "Current Work Unit", true);

  glColor3ub(0x9, 0xa7, 0xb7);

  // WU
  print(12, 84, "Project:");
  if (info.project)
    print(140, 84,
          String::printf("%-6d Run: %-4d Clone: %-4d Gen: %-4d", info.project,
                         info.run, info.clone, info.gen));

  // Core name
  print(12, 62, "FahCore:");
  if (info.project)
    print(140, 62, info.core + String::printf(" 0x%02x", info.coreType));

  // Progress
  print(12, 40, "Progress:");

  if (info.project) {
    double percentDone;
    if (info.progress && info.progress <= 1) percentDone = info.progress;
    else percentDone = info.totalIterations ?
           (double)info.iterationsDone / info.totalIterations : 0;

    float x = 140;
    float y = 38;
    float w = 280;
    float h = 16;

    glPolygonMode(GL_FRONT, GL_LINE);
    glLineWidth(2);
    glColor3ub(0x7d, 0x83, 0x90);
    glRectf(x, y, x + w, y + h);

    glColor3ub(0xff, 0xff, 0xff);
    glPolygonMode(GL_FRONT, GL_FILL);
    glRectf(x, y, x + w * percentDone, y + h);

    glColor3ub(0x9, 0xa7, 0xb7);
    print(264, 40, String::printf("%0.2f%%", 100 * percentDone));
  }

  // Print ETA
  print(12, 18, "Time Left:");
  if (info.eta) print(140, 18, SSTR(TimeInterval(info.eta)));

  CHECK_GL_ERROR("");
}


void BasicViewer::drawLogos(const View &view) {
  if (!view.getShowLogos()) return;

  float spacing = 16;
  float totalWidth = 4 + spacing * (logos.size() - 1);
  float maxHeight = 0;

  for (unsigned i = 0; i < logos.size(); i++) {
    totalWidth += logos[i]->getWidth();
    if (maxHeight < logos[i]->getHeight()) maxHeight = logos[i]->getHeight();
  }

  // width = view.getWidth() - 4 - (width of small box) - 4
  float width = view.getWidth() - 268;

  if (width <= 64 * logos.size()) return; // Not enough room

  glDisable(GL_LIGHTING);
  resetDraw(view);

  glTranslatef(view.getWidth(), 0, 0);

  if (width < totalWidth) {
    float scale = width / totalWidth;
    glScalef(scale, scale, 1);
  }

  glTranslatef(-4, 0, 0);
  for (unsigned i = 0; i < logos.size(); i++) {
    glTranslatef(-logos[i]->getWidth(), 0, 0);
    float y = (maxHeight - logos[i]->getHeight()) / 2 + 2;
    logos[i]->draw(0, y);
    glTranslatef(-spacing, 0, 0);
  }

  CHECK_GL_ERROR("");
}


void BasicViewer::drawButtons(const View &view) {
  if (!view.getShowButtons()) return;

  float spacing = 16;
  float totalHeight = 4 + spacing * (logos.size() - 1);
  float maxWidth = 0;

  for (unsigned i = 0; i < buttons.size(); i++) {
    totalHeight += buttons[i]->getHeight();
    if (maxWidth < buttons[i]->getWidth())
      maxWidth = buttons[i]->getWidth();
  }

  float xOffset = view.getWidth() - maxWidth - spacing;
  if (xOffset < 0) xOffset = 0;

  float yOffset = view.getHeight() - 120; // view.getHeight() - small box height
  if (yOffset < 0) yOffset = 0;

  glDisable(GL_LIGHTING);
  resetDraw(view);

  for (unsigned i = 0; i < buttons.size(); i++) {
    float x = (maxWidth - buttons[i]->getWidth()) / 2 + 2;
    Vector2D min(x + xOffset, view.getHeight() - yOffset);

    yOffset -= buttons[i]->getHeight() + spacing;

    Vector2D max(x + xOffset + buttons[i]->getWidth(),
                 view.getHeight() - yOffset);
    Rectangle2D bounds(min, max);

    picker.set(buttons[i]->getName(), bounds);

    if (glSecondaryColor3f && bounds.contains(view.getMousePosition())) {
      glEnable(GL_COLOR_SUM_EXT);
      glSecondaryColor3f(0.2, 0.2, 0.4);
    }

    buttons[i]->draw(x + xOffset, yOffset);

    if (glSecondaryColor3f) glDisable(GL_COLOR_SUM_EXT);
  }

  CHECK_GL_ERROR("");
}


void BasicViewer::drawPopup(const View &view, float width, float height,
                            float contentHeight) {
  glDisable(GL_LIGHTING);
  resetDraw(view);

  // Set page height
  popupPageHeight = height;

  // Clamp offset
  int maxYOffset = (int)(contentHeight - popupPageHeight);
  if (maxYOffset < popupYOffset) popupYOffset = maxYOffset;
  if (popupYOffset < 0) popupYOffset = 0;

  // Save original dims
  float origWidth = width;
  float origHeight = height;

  // Add padding
  float padding = 24;
  width += 2 * padding;
  height += 2 * padding;

  // Add scroll
  if (popupPageHeight < contentHeight) width += 16;

  // Scale if needed and calculate margins
  float xMargin, yMargin;
  float scale = 1;

  if (view.getWidth() - width < 0 || view.getHeight() - height < 0) {
    float xScale = view.getWidth() / width;
    float yScale = view.getHeight() / height;

    if (xScale < yScale) {
      scale = xScale;
      xMargin = 0;
      yMargin = Math::round((view.getHeight() - height * scale) / 2 / scale);

    } else {
      scale = yScale;
      xMargin = Math::round((view.getWidth() - width * scale) / 2 / scale);
      yMargin = 0;
    }

    glScalef(scale, scale, 0);

  } else {
    xMargin = Math::round((view.getWidth() - width) / 2);
    yMargin = Math::round((view.getHeight() - height) / 2);
  }

  // Draw box
  glTranslatef(xMargin, yMargin, 0);
  darkBox.draw(width, height);

  // Draw close
  {
    glPushMatrix();
    glTranslatef(padding, origHeight + padding / 2, 0);

    Vector2D min = project(Vector2D(-8, -8));
    Vector2D max = project(Vector2D(24, 24));
    Rectangle2D bounds = Rectangle2D(min, max);
    picker.set("close", bounds);

    if (bounds.contains(view.getMousePosition())) glColor3f(1, 1, 1);
    else glColor3f(0.8, 0.6, 0.6);

    glBegin(GL_POLYGON);
    glVertex2f(0, 2);
    glVertex2f(2, 0);
    glVertex2f(12, 10);
    glVertex2f(10, 12);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(10, 0);
    glVertex2f(12, 2);
    glVertex2f(2, 12);
    glVertex2f(0, 10);
    glEnd();

    glPopMatrix();
  }

  // Draw scroll
  if (popupPageHeight < contentHeight) {
    glPushMatrix();

    glTranslatef(padding + origWidth + 8, padding / 2, 0);

    // Down
    if (popupYOffset < maxYOffset) {
      Vector2D min = project(Vector2D(-8, -8));
      Vector2D max = project(Vector2D(24, 16 + origHeight / 2));
      Rectangle2D bounds(min, max);
      picker.set("down", bounds);

      if (bounds.contains(view.getMousePosition())) glColor3f(1, 1, 1);
      else glColor3f(0.8, 0.6, 0.6);

      glBegin(GL_POLYGON);
      glVertex2f(8, 0);
      glVertex2f(16, 16);
      glVertex2f(0, 16);
      glEnd();
    }

    // Up
    if (popupYOffset) {
      glTranslatef(0, origHeight, 0);

      Vector2D min = project(Vector2D(-8, -origHeight / 2));
      Vector2D max = project(Vector2D(24, 24));
      Rectangle2D bounds = Rectangle2D(min, max);
      picker.set("up", bounds);

      if (bounds.contains(view.getMousePosition())) glColor3f(1, 1, 1);
      else glColor3f(0.8, 0.6, 0.6);

      glBegin(GL_POLYGON);
      glVertex2f(0, 0);
      glVertex2f(16, 0);
      glVertex2f(8, 16);
      glEnd();
    }

    glPopMatrix();
  }

  // Translate to correct position for content
  glTranslatef(padding, height - padding + popupYOffset, 0);

  // Enable scissor
  glScissor((int)((padding + xMargin) * scale),
            (int)((padding + yMargin) * scale),
            (int)(origWidth * scale), (int)(origHeight * scale));
  glEnable(GL_SCISSOR_TEST);
}


void BasicViewer::drawAbout(const View &view) {
  // Reformat
  const Resource &res = FAH::Viewer::resource0.get("about.txt");
  string aboutText = String::fill(res.getData());
  const char *creditsText = FAH::Viewer::resource0.get("credits.txt").getData();

  // Measure
  Vector2D aboutTextDims = font->dimensions(aboutText);
  Vector2D creditsTextDims = font->dimensions(creditsText);

  float width = aboutTextDims.x();
  float height = fontBold->getLineHeight() + 16 + fahLogo.getHeight() +
    aboutTextDims.y() + 16 + cdLogo.getHeight() + 16 +
    font->getLineHeight() + 16 + fontBold->getLineHeight() + 8 +
    creditsTextDims.y();
  float y = 16;

  // Draw
  drawPopup(view, width, view.getHeight() * 0.8, height);

  glColor3ub(0x73, 0x96, 0xcf);
  fontBold->print(0, -y, "About the Folding@home Viewer", (unsigned)width);
  y += fontBold->getLineHeight() + 8;

  glColor3f(1, 1, 1);
  y += fahLogo.getHeight();
  fahLogo.draw((width - fahLogo.getWidth()) / 2, -y + 16);
  y += 8;

  glColor3ub(0x98, 0xb8, 0xd6);
  font->print(0, -y, aboutText.c_str(), (unsigned)width);
  y += aboutTextDims.y() + 16;
  glColor3f(1, 1, 1);
  y += cdLogo.getHeight();
  cdLogo.draw((width - cdLogo.getWidth()) / 2, -y + 16);
  y += 16;

  glColor3ub(0x98, 0xb8, 0xd6);
  font->print(0, -y, "http://cauldrondevelopment.com/", (unsigned)width);
  y += font->getLineHeight() + 16;

  glColor3ub(0x73, 0x96, 0xcf);
  fontBold->print(0, -y, "Credits", (unsigned)width);
  y += fontBold->getLineHeight() + 8;

  glColor3ub(0x98, 0xb8, 0xd6);
  font->print(0, -y, creditsText, (unsigned)width);

  glDisable(GL_SCISSOR_TEST);
}


void BasicViewer::drawTextPopup(const View &view, const string &title,
                                const string &text) {
  Vector2D titleDims = fontBold->dimensions(title);
  Vector2D textDims = font->dimensions(text);

  float width = max(titleDims.x(), textDims.x());
  float height = titleDims.y() + textDims.y();

  drawPopup(view, width, view.getHeight() * 0.8, height);

  glColor3ub(0x98, 0xb8, 0xd6);
  fontBold->print(0, -16, title, (unsigned)width);

  glColor3ub(0x98, 0xb8, 0xd6);
  font->print(0, -titleDims.y() - 12, text);

  glDisable(GL_SCISSOR_TEST);
}


void BasicViewer::drawBackground(const View &view) {
  if (view.getBGTexture().isNull()) return;

  resetDraw(view);
  glDisable(GL_LIGHTING);
  glEnable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);

  // Draw background
  view.getBGTexture()->draw(0, 0, view.getWidth(), view.getHeight());

  CHECK_GL_ERROR("");
}


void BasicViewer::drawRest(const SimulationInfo &info, const View &view) {
  // Draw buttons
  drawButtons(view);

  // Draw logos
  drawLogos(view);

  // Draw simulation info
  drawInfo(info, view);

  if (view.getShowAbout()) drawAbout(view);
  if (view.getShowHelp()) {
    const char *helpText = FAH::Viewer::resource0.get("help.txt").getData();
    drawTextPopup(view, "Help Screen", helpText);
  }
}


void BasicViewer::init(ViewMode mode) {
  if (initialized) THROW("BasicViewer already initialized");

  this->mode = mode;

  glColor4f(1, 1, 1, 1);
  glClearColor(0, 0, 0, 0);
  glClearDepth(1);

  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_NORMALIZE);
  glEnable(GL_CULL_FACE);

  // lights
  const float ambient0[]        = {0.1, 0.1, 0.1, 1.0};
  const float diffuse0[]        = {1.0, 1.0, 1.0, 1.0};
  const float position0[]       = {1.0, 1.0, 1.0, 0.0};
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
  glLightfv(GL_LIGHT0, GL_POSITION, position0);

  const float ambient1[]        = {0.2, 0.2, 0.2, 1.0};
  const float diffuse1[]        = {0.5, 0.5, 0.5, 1.0};
  const float position1[]       = {-1.0, -1.0, 1.0, 0.0};
  glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
  glLightfv(GL_LIGHT1, GL_POSITION, position1);

  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

  const float front_shininess[] = {60};
  const float front_specular[]  = {0.7, 0.7, 0.7, 1};
  glMaterialfv(GL_FRONT, GL_SHININESS, front_shininess);
  glMaterialfv(GL_FRONT, GL_SPECULAR, front_specular);
  glDrawBuffer(GL_BACK);

  // Create atom sphere
  double sphereSize = 1;
  switch (mode % 3) {
  case MODE_SPACE_FILLED: sphereSize = SPHERE_SIZE; break;
  case MODE_BALL_AND_STICK: sphereSize = SPHERE_SIZE_SMALL; break;
  case MODE_STICK: sphereSize = SPHERE_SIZE_TINY; break;
  }

  sphere = new SphereVBO(Vector3D(), sphereSize, SUBDIVISIONS, true);

  // Create bond cylinder
  cylinder = new CylinderVBO(BOND_RADIUS, BOND_RADIUS, 1, 10, 2, true);

  // Load textures
  box.load();
  darkBox.load();
  for (unsigned i = 0; i < logos.size(); i++) logos[i]->load();
  for (unsigned i = 0; i < buttons.size(); i++) buttons[i]->load();
  cdLogo.load();
  fahLogo.load();

  initialized = true;

  CHECK_GL_ERROR("");
}


void BasicViewer::release() {
  if (!initialized) return;

  // Release textures
  box.release();
  for (unsigned i = 0; i < logos.size(); i++) logos[i]->release();

  initialized = false;

  CHECK_GL_ERROR("");
}


void BasicViewer::draw(const SimulationInfo &info, const Protein *protein,
                       const View &view) {
  // Draw background
  drawBackground(view);

  // Draw protein
  if (protein) drawProtein(*protein, view);

  // Draw rest
  drawRest(info, view);
}


void BasicViewer::resize(const View &view) {
  glViewport(0, 0, view.getWidth(), view.getHeight());
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (float)view.getWidth() / view.getHeight(), 1, 1000);
  glMatrixMode(GL_MODELVIEW);
}


string BasicViewer::pick(const Vector2D &p) {
  return picker.pick(p);
}


Vector2D BasicViewer::project(const Vector2D &v) {
  double model[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, model);

  double proj[16];
  glGetDoublev(GL_PROJECTION_MATRIX, proj);

  int view[4];
  glGetIntegerv(GL_VIEWPORT, view);

  double x, y, z;
  gluProject(v.x(), v.y(), 0, model, proj, view, &x, &y, &z);

  return Vector2D(x, view[3] - y);
}
