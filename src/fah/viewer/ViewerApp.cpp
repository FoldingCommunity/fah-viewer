/******************************************************************************\

                     This file is part of the FAHViewer.

           The FAHViewer displays 3D views of Folding@home proteins.
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

#include "ViewerApp.h"

#include "GL.h"

#include <cbang/Exception.h>
#include <cbang/Info.h>
#include <cbang/log/Logger.h>
#include <cbang/util/Resource.h>

using namespace std;
using namespace cb;
using namespace FAH;

namespace FAH {
  namespace BuildInfo {
    void addBuildInfo(const char *category);
  }
}

namespace FAH {
  namespace Viewer {
    extern const DirectoryResource resource0;
  }
}


static void mouseCallback(int button, int state, int x, int y) {
  ViewerApp::instance().mouse(button, state, x, y);
}


static void motionCallback(int x, int y) {
  ViewerApp::instance().motion(x, y);
}


static void keysCallback(unsigned char key, int x, int y) {
  ViewerApp::instance().keys(key, x, y);
}


static void specialCallback(int key, int x, int y) {
  ViewerApp::instance().special(key, x, y);
}


static void renderCallback() {
  ViewerApp::instance().render();
}


static void resizeCallback(int w, int h) {
  ViewerApp::instance().resize(w, h);
}


static void visibilityCallback(int state) {
  ViewerApp::instance().visibility(state);
}


static void idleCallback() {
  ViewerApp::instance().idle();
}


ViewerApp *ViewerApp::singleton = 0;


ViewerApp::ViewerApp() :
  Application("Folding@home Viewer", ViewerApp::_hasFeature),
  View(getOptions()), visible(false), lastPause(false), formerWidth(0),
  formerHeight(0), mouseDragging(false), fullscreen(false), force(false) {

  if (singleton) THROW("ViewerApp already exists");
  singleton = this;

  // Configure commandline
  cmdLine.setAllowConfigAsFirstArg(false);
  cmdLine.setAllowPositionalArgs(true);

  Options &options = getOptions();
  options.addTarget("fullscreen", fullscreen, "Display in fullscreen mode");
  options.addTarget("force", force, "Force running on blacklisted GPUs or in "
                    "advanced modes");

  cmdLine["--config"].setDefault("viewer.xml");
  const char *helpText = FAH::Viewer::resource0.get("help.txt").getData();
  cmdLine.addUsageLine(helpText);

  // Info
  Info &info = Info::instance();
  BuildInfo::addBuildInfo("Build");
  info.add("System", "OpenGL Render", (const char *)glGetString(GL_RENDERER));

  // TODO move this stuff out to the build system
  info.add(name, "Website", "https://foldingathome.org/", true);
  info.add(name, "Copyright", "(c) 2009-2018 Stanford University");
  info.add(name, "Author", "Joseph Coffland <joseph@cauldrondevelopment.com>");
}


ViewerApp &ViewerApp::instance() {
  if (!singleton) singleton = new ViewerApp();
  return *singleton;
}


bool ViewerApp::_hasFeature(int feature) {
  switch (feature) {
  case FEATURE_INFO: return true;
  default: return Application::_hasFeature(feature);
  }
}


int ViewerApp::init(int argc, char *argv[]) {
  // Parse command line, etc.
  Application::init(argc, argv);

  // Blacklist certain GPUs which are known to BSOD on Windows.
  string render = Info::instance().get("System", "OpenGL Render");
  if (!force) {
    if (render == "xxx" || render == "xxx")
      THROWS("Your GPU has been blacklisted due to system crashes when running "
             "this program");
  }

  GLenum err = glewInit();
  if (err != GLEW_OK) THROWS("Initializing GLEW: " << glewGetErrorString(err));

  if (!GLEW_VERSION_1_1 && !force)
    THROWS("Need at least OpenGL 1.1 to run this application");

  if (!getBasic() && !GLEW_VERSION_2_0 && !force) {
    LOG_WARNING("Need at least OpenGL 2.0 for non-basic mode.  You may need "
                "to install your graphics card vendor's drivers.  Downgrading "
                "to basic mode.");
    setBasic(true);
  }

  // Clamp screen size
  setWidth(getWidth());
  setHeight(getHeight());

  // Fullscreen (must be after GL init)
  if (fullscreen) setFullscreen(fullscreen);
  else reshape(getWidth(), getHeight());

  initView(cmdLine.getPositionalArgs());

  // Callbacks
  glutMouseFunc(mouseCallback);
  glutMotionFunc(motionCallback);
  glutPassiveMotionFunc(motionCallback);
  glutKeyboardFunc(keysCallback);
  glutSpecialFunc(specialCallback);
  glutDisplayFunc(renderCallback);
  glutReshapeFunc(resizeCallback);
  glutVisibilityFunc(visibilityCallback);
  glutIdleFunc(idleCallback);

  return 0;
}


void ViewerApp::run() {
  glutMainLoop();
}


void ViewerApp::quit() {
  glutDestroyWindow(glutGetWindow());
  visible = false;

  exit(0);
}


void ViewerApp::setFullscreen(bool fullscreen) {
  this->fullscreen = fullscreen;

  if (fullscreen) {
    formerX = glutGet(GLUT_WINDOW_X);
    formerY = glutGet(GLUT_WINDOW_Y);
    formerWidth = getWidth();
    formerHeight = getHeight();
    glutFullScreen();

  } else {
    reshape(formerWidth ? formerWidth : getWidth(),
            formerHeight ? formerHeight: getHeight());
    formerWidth = formerHeight = 0;

#ifdef _WIN32
    // Windows needs this but it shifts the window on Linux
    glutPositionWindow(formerX, formerY);
#endif
  }

  redisplay();
}


Vector3D ViewerApp::findBallVector(unsigned px, unsigned py) {
  double width = getWidth();
  double height = getHeight();
  double x = (double)px / (width / 2.0) - 1.0;
  double y = 1.0 - (double)py / (height / 2.0);

  // Scale one dim
  if (width < height) x *= width / height;
  else y *= height / width;

  double z2 = 1.0 - x * x - y * y;
  double z = 0 < z2 ? sqrt(z2) : 0; // Clamp to 0
  
  return Vector3D(x, y, z).normalize();
}


void ViewerApp::mouse(int button, int state, int x, int y) {
  if (popupVisible()) {
    switch (button) {
    case GLUT_LEFT_BUTTON:
      if (state == GLUT_UP) click(Vector2D(x, y));
      break;

    case 3: lineUp(); break; // Wheel up
    case 4: lineDown(); break; // Wheel down
    }

  } else
    switch (button) {
    case GLUT_LEFT_BUTTON:
      switch (state) {
      case GLUT_DOWN:
        mouseStart = findBallVector(x, y);
        startRotation = getRotation();
        mouseDragging = true;
        lastPause = getPause();
        setPause(true);
        break;

      case GLUT_UP:
        mouseDragging = false;
        setPause(lastPause);
        break;
      }
      break;

    case 3: zoomOut(); break; // Wheel up
    case 4: zoomIn(); break; // Wheel down
    }

  switch (button) {
  case GLUT_LEFT_BUTTON:
    switch (state) {
    case GLUT_DOWN: mouseStart2D = Vector2D(x, y); break;
    case GLUT_UP:
      string pick = getViewer().pick(mouseStart2D);
      Vector2D mouseEnd = Vector2D(x, y);
      if (pick == getViewer().pick(mouseEnd)) {
        LOG_DEBUG(3, "Left mouse click " << mouseStart << ' ' << pick);

        showPopup(pick);
      }
      break;
    }
    break;
  }

  redisplay();
}


void ViewerApp::motion(int x, int y) {
  // Ignore if mouse is out of our window
  if (x <= 0 || (int)getWidth() <= x || y <= 0 || (int)getHeight() <= y) return;

  // Mouse hover over buttons
  hover(Vector2D(x, y));

  // Mouse drag
  if (!popupVisible() && mouseDragging) {
    Vector3D current = findBallVector(x, y);
    double angle = fmod((4 * mouseStart.angleBetween(current)), (2 * M_PI));
    if (angle < -0.01 || 0.01 < angle) {
      QuaternionD delta(AxisAngleD(angle, mouseStart.crossProduct(current)));
      QuaternionD rotation =
        QuaternionD(delta.normalize()).multiply(startRotation).normalize();
      setRotation(rotation);
    }

    redisplay();
  }
}


void ViewerApp::keys(unsigned char key, int x, int y) {
  LOG_DEBUG(3, "Key pressed '" << key << "' == 0x" << hex << (unsigned)key);

  if (!popupVisible()) {
    switch (key) {
    case '0':
      setCycle(!getCycle());
      LOG_DEBUG(1, "Snapshot cycling "
                << (getCycle() ? "enabled" : "disabled"));
      break;

    case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9':
      setMode((ViewMode::enum_t)(key - '1'));
      break;

    case ' ': setPause(!getPause()); break;
    case '+': setFPS(getFPS() * 2); break;
    case '-': setFPS(getFPS() / 2); break;
    case '[': if (getSlot()) setSlot(getSlot() - 1); break;
    case ']': setSlot(getSlot() + 1); break;
    case 'b': setBlur(!getBlur()); break;
    case 'i': setShowInfo(!getShowInfo()); break;
    case 'l': setShowLogos(!getShowLogos()); break;
    case 'q': case 'Q': quit(); break;
    case '\033': if (fullscreen) setFullscreen(false); break;
    }
  }

  switch (key) {
  case 'h': showPopup("help"); break;
  case 'a': showPopup("about"); break;
  case 'f': setFullscreen(!fullscreen); break;
  default: closePopup(); break;
  }

  redisplay();
}


void ViewerApp::special(int key, int x, int y) {
  if (popupVisible()) {
    switch (key) {
    case GLUT_KEY_UP: lineUp(); break;
    case GLUT_KEY_DOWN: lineDown(); break;
    case GLUT_KEY_PAGE_UP: pageUp(); break;
    case GLUT_KEY_PAGE_DOWN: pageDown(); break;
    }

  } else {
    switch (key) {
    case GLUT_KEY_UP: spinUp(); break;
    case GLUT_KEY_DOWN: spinDown(); break;
    case GLUT_KEY_RIGHT: spinRight(); break;
    case GLUT_KEY_LEFT: spinLeft(); break;
    case GLUT_KEY_PAGE_UP: zoomOut(); break;
    case GLUT_KEY_PAGE_DOWN: zoomIn(); break;
    }
  }

  redisplay();
}


void ViewerApp::render() {
  if (!visible) return;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  draw();
  glutSwapBuffers();
}


void ViewerApp::visibility(int state) {
  visible = state == GLUT_VISIBLE;
}


void ViewerApp::idle() {
  if (shouldQuit()) quit();
  update(mouseDragging);
}


void ViewerApp::redisplay() {
  if (visible) glutPostRedisplay();
}


void ViewerApp::reshape(unsigned w, unsigned h) {
  glutReshapeWindow(w, h);
}
