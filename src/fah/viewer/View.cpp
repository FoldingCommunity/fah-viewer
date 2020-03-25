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

#include "View.h"

#include "TestData.h"
#include "wtypes.h"

#include <fah/viewer/advanced/AdvancedViewer.h>
#include <fah/viewer/basic/BasicViewer.h>

#include <cbang/Exception.h>
#include <cbang/log/Logger.h>
#include <cbang/time/Time.h>
#include <cbang/os/SystemUtilities.h>
#include <cbang/Catch.h>

using namespace std;
using namespace cb;
using namespace FAH;


View::View(cb::Options &options) :
  options(options), width(1024), height(768), zoom(1.05), basic(true),
  wiggle(true), cycle(true), blur(true), modeNumber(4), slot(0), pause(false),
  rotation(0, 0, 0, 0.999), degreesPerSec(0, 3), lastFrame(0), currentFrame(0),
  totalFrames(0), interpSteps(35), fps(45), forward(true), profile("default"),
  connectTime(0), renderSpeed(1.0 / 45.0), idleSpeed(1.0 / 6.0), showInfo(true),
  showLogos(true), showHelp(false), showAbout(false), showButtons(true),
  connectionStatus("None") {

  RECT desktop;
  // Returns available screen size without taskbar
  const HWND hDesktop = GetDesktopWindow();
  GetWindowRect(hDesktop, &desktop);
  if (desktop.bottom <= 768 || desktop.right <= 1024) {
    width=800;
    height=600;
  }

  // Add options
  options.add("connect", "An address and port to connect to in the form: "
              "<host | IP>:<port>")->setDefault("127.0.0.1:36330");
  options.addTarget("password", password, "A password for accessing the remote "
                    "client")->setObscured();
  options.addTarget("slot", slot, "Slot on the client to view");
  options.add("test", "Load test data")->setDefault(true);

  options.add("recompute-bonds", "Recompute bonds from expected bond lengths"
              )->setDefault(false);
  options.addTarget("interpolation-steps", interpSteps, "The number of "
                    "interpolated protein views to calculate between "
                    "snapshots.");
  options.addTarget("wiggle", wiggle, "Enable atom wiggling which "
                    "approximates simulation activity");
  options.addTarget("x-rotation", degreesPerSec.x(), "Rotation about the "
                    "X-axis in degrees per second");
  options.addTarget("y-rotation", degreesPerSec.y(), "Rotation about the "
                    "Y-axis in degrees per second");
  options.addTarget("width", width, "Screen width");
  options.addTarget("height", height, "Screen height");
  options.addTarget("zoom", zoom, "Zoom level");
  options.addTarget("mode", modeNumber, "Render mode");
  options.addTarget("blur", blur, "Enable blur (advanced only)");
  options.addTarget("show-info", showInfo, "Display simulation info");
  options.addTarget("show-logos", showLogos, "Display logos");
  options.addTarget("show-buttons", showButtons, "Display buttons");
  options.addTarget("basic", basic, "Disable advanced modes which require "
                    "OpenGL 2.2");
  options.addTarget("cycle-snapshots", cycle, "Cycle through snapshot "
                    "frames.");
  options.add("fps", "Snapshot display speed in frames per second."
              )->setDefault(fps);
  options.add("background", "Specify an alternative background image to use.  "
              "Must be in PPM format.  The value 'none' will disable the "
              "background image.");
  options.addTarget("profile", profile, "Set performance profile.  This "
                    "effects the CPU usage vs. smooth rendering.  Valid "
                    "options are: lean, default & mean");
}


void View::initView(const vector<string> &inputs) {
  // Clamp screen size
  setWidth(getWidth());
  setHeight(getHeight());

  // Zoom
  if (zoom < 0.2) zoom = 0.2;
  if (3 < zoom) zoom = 3;

  // Mode
  if (modeNumber) mode = (ViewMode::enum_t)(modeNumber - 1);
  setMode(mode);

  // Background
  if (options["background"].hasValue()) {
    if (options["background"].toString() != "none")
      bgTexture = new Texture(string("file://") + options["background"]);
  } else bgTexture =
           new Texture(string("background_") + (basic ? "small" : "large"));

  if (!bgTexture.isNull()) bgTexture->load();

  // Check interpolation steps
  if (100 < interpSteps) {
    LOG_WARNING("Too many interpolation steps, reducing to 100");
    interpSteps = 100;
  }

  // Preformance profile
  if (profile == "lean") {
    renderSpeed = 1.0 / 2.0;
    idleSpeed = 1.0;

  } else if (profile == "mean") {
    renderSpeed = 1.0 / 60.0;
    idleSpeed = 1.0 / 4.0;

  } else if (profile != "default")
    LOG_WARNING("Unsupported profile='" << profile << "'");

  trajectory = new Trajectory(true, true, interpSteps);

  // Load data
  if (!inputs.empty()) {
    for (unsigned i = 0; i < inputs.size(); i++) {
      string ext = SystemUtilities::extension(inputs[i]);

      try {
        if (ext == "xyz") trajectory->readXYZ(inputs[i]);
        else if (ext == "json") trajectory->readJSON(inputs[i]);
        else THROW("Input file with unknown extension '" << inputs[i] << "'");
      } CATCH_ERROR;
    }

    // Make fake topology if none was loaded
    trajectory->ensureTopology();

    if (options["recompute-bonds"].toBoolean()) trajectory->recomputeBonds();

  } else if (options["test"].isSet() && options["test"].toBoolean())
    loadTestData();

  else if (client.isNull() && options["connect"].hasValue() &&
           String::toLower(options["connect"].toString()) != "false") {

    IPAddress addr(options["connect"]);
    if (!addr.getPort()) addr.setPort(36330); // The default port
    if (addr.getIP()) client = createClient(addr);
  }
}


void View::loadTestData() {
  TestData::load(*trajectory);
}


void View::setViewer(const cb::SmartPointer<ViewerBase> &viewer) {
  if (!this->viewer.isNull()) this->viewer->release();
  this->viewer = viewer;
  if (!viewer.isNull()) viewer->init(mode);
}


void View::setWidth(unsigned width) {
  if (width < 100) this->width = 100;
  else if (10000 < width) this->width = 1024;
  else this->width = width;
}


void View::setHeight(unsigned height) {
  if (height < 100) this->height = 100;
  else if (10000 < height) this->height = 768;
  else this->height = height;
}


void View::setMode(ViewMode mode) {
  if (!viewer.isNull() && this->mode == mode) return;
  if (ViewMode::getCount() <= (unsigned)mode)
    mode = (ViewMode::enum_t)((unsigned)mode % ViewMode::getCount());
  if (MODE_ADV_SPACE_FILLED <= mode && basic)
    mode = (ViewMode::enum_t)((unsigned)mode % (unsigned)MODE_ADV_SPACE_FILLED);

  if (!viewer.isNull()) viewer->release();

  // Update viewer
  if (this->mode < MODE_ADV_SPACE_FILLED && MODE_ADV_SPACE_FILLED <= mode)
    viewer = new AdvancedViewer;

  if (mode < MODE_ADV_SPACE_FILLED && MODE_ADV_SPACE_FILLED <= this->mode)
    viewer = new BasicViewer;

  if (viewer.isNull()) viewer = mode < MODE_ADV_SPACE_FILLED ?
                         new BasicViewer : new AdvancedViewer;

  this->mode = mode;

  viewer->init(mode);

  LOG_INFO(1, "Mode " << mode);
}


void View::setSlot(unsigned slot) {
  if (slot == this->slot) return;

  if (!client.isNull()) {
    if (!client->setSlot(slot)) return;
    connectTime = Time::now();
    trajectory->clear();
    info = SimulationInfo();
  }

  this->slot = slot;
}


void View::setPause(bool pause) {
  if (!pause) lastFrame = Time::now();
  this->pause = pause;
}


void View::setFPS(double fps) {
  if (64 < fps) this->fps = 64;
  else if (fps < 0.25) this->fps = 0.25;
  else this->fps = fps;
}


string View::getStatus() const {
  return trajectory->empty() ? (!client.isNull() && client->hasLoadableSlot() ? "Loading" : (!client.isNull() && client->isConnected() ? "Awaiting" : "")) : (info.project ? "Live" : "Demo");
}


string View::getFrameDescription() const {
  unsigned total = totalFrames;
  unsigned current = total ? 1 + currentFrame : 0;

  if (interpSteps && 1 < total) {
    total = total / (interpSteps + 1) + 1;
    unsigned iCurrent = (current - 1) % (interpSteps + 1);
    current = (current - 1) / (interpSteps + 1) + 1;
    return String::printf("%d.%d of %d", current, iCurrent, total);
  }

  return String::printf("%d of %d", current, total);
}


void View::showPopup(const string &name) {
  if (name == "home") {return;}		// todo: open homepage or forum, then return

  if (name == "about") {closePopup(); showAbout = true;}
  else if (name == "help") {closePopup(); showHelp = true;}
  redisplay();
}


void View::closePopup() {
  if (!popupVisible()) return;
  showHelp = showAbout = false;
  redisplay();
}


bool View::popupVisible() {
  return showHelp || showAbout;
}


void View::zoomIn() {
  if (0.4 < zoom) zoom *= 0.9;
}


void View::zoomOut() {
  if (zoom < 5) zoom *= 1.1;
}


void View::click(const Vector2D &pos) {
  string pick = viewer->pick(pos);

  if (pick == "up") viewer->lineUp(5);
  else if (pick == "down") viewer->lineDown(5);
  else if (pick == "close") closePopup();
}


void View::hover(const Vector2D &pos) {
  mousePosition = pos;
  string pick = viewer->pick(pos);

  if (pick != buttonHover) {
    buttonHover = pick;
    redisplay();
  }
}


static double expInc(double x) {
  if (x < 1024) {
    if (0 <= x) return x < 8 ? 8 : (x * 2);
    else return -8 <= x ? 0 : (x / 2);
  }

  return x;
}


static double expDec(double x) {
  return -expInc(-x);
}


void View::spinUp() {
  degreesPerSec.x() = expInc(degreesPerSec.x());
}


void View::spinDown() {
  degreesPerSec.x() = expDec(degreesPerSec.x());
}


void View::spinRight() {
  degreesPerSec.y() = expInc(degreesPerSec.y());
}


void View::spinLeft() {
  degreesPerSec.y() = expDec(degreesPerSec.y());
}


void View::draw() {
  viewer->draw(info, protein.get(), *this);
  renderTimer.throttle(renderSpeed);
}


void View::resize(unsigned width, unsigned height) {
  if (!height) height = 1; // Avoid div by 0

  this->width = width;
  this->height = height;

  viewer->resize(*this);
}


static uint32_t xorshift_rand() {
  static uint32_t rand_x = 123456789;
  static uint32_t rand_y = 362436069;
  static uint32_t rand_z = 521288629;
  static uint32_t rand_w = 88675123;

  uint32_t t = rand_x ^ (rand_x << 11);
  rand_x = rand_y; rand_y = rand_z; rand_z = rand_w;
  return rand_w = rand_w ^ (rand_w >> 19) ^ (t ^ (t >> 8));
}


void View::update(bool fast) {
  bool redisplay = false;

  // Update client connection
  if (!client.isNull()) {
    if (client->update()) redisplay = true;

    // Load "Demo" protein after timeout
    if (!client->isConnected()) {
      if (!connectTime) connectTime = Time::now();
      else if (trajectory->empty() && connectTime + 5 <= Time::now()) {
        loadTestData();
        redisplay = true;
      }
    }

    // Update status
    string status;
    switch (client->getState()) {
    case Client::STATE_WAITING: status = "Trying"; break;
    case Client::STATE_CONNECTING: status = "Connecting"; break;
    default: status = "Connected"; break;
    }

    if (connectionStatus != status) {
      connectTime = 0;
      connectionStatus = status;
      redisplay = true;
    }
  }

  // Animate
  totalFrames = trajectory->size();
  if (totalFrames <= currentFrame) currentFrame = 0;
  if (!trajectory->empty() && !pause && lastFrame + 1.0 / fps < Timer::now()) {
   // Rotate X
    if (degreesPerSec.x()) {
      double angle = (Timer::now() - lastFrame) *
        -degreesPerSec.x() / 180 * M_PI;
      QuaternionD delta(AxisAngleD(angle, 1, 0, 0));
      rotation = QuaternionD(delta.normalize()).multiply(rotation).normalize();
      redisplay = true;
    }

    // Rotate Y
    if (degreesPerSec.y()) {
      double angle = (Timer::now() - lastFrame) *
        degreesPerSec.y() / 180 * M_PI;
      QuaternionD delta(AxisAngleD(angle, 0, 1, 0));
      rotation = QuaternionD(delta.normalize()).multiply(rotation).normalize();
      redisplay = true;
    }

    lastFrame = Timer::now();

    // Cycle frames
    unsigned oldFrame = currentFrame;
    if (1 < trajectory->size() && cycle) {
      // Advance frame
      if (forward) {
        if (++currentFrame == trajectory->size()) {
          currentFrame -= 2;
          forward = false;
        }
        
      } else if (currentFrame-- == 0) {
        currentFrame = 1;
        forward = true;
      }

    } else if (!trajectory->empty()) currentFrame = trajectory->size() - 1;

    if (oldFrame != currentFrame) {
      if (currentFrame < trajectory->size())
        protein = trajectory->getProtein(currentFrame);
      else protein = 0;

      redisplay = true;
    }
    if (protein.isNull() && currentFrame < trajectory->size())
      protein = trajectory->getProtein(currentFrame);

    // Wiggle
    if (!protein.isNull() && wiggle) {
      double radius = protein->getRadius();
      SmartPointer<Positions> positions =
        new Positions(*trajectory->at(currentFrame));

      for (unsigned i = 0; i < positions->size(); i++) {
        uint32_t r = xorshift_rand();
        positions->at(i) += Vector3D(0.1 - (r & 255) / (float)1280,
                                     0.1 - ((r >> 8) & 255) / (float)1280,
                                     0.1 - ((r >> 16) & 255) / (float)1280);
      }

      protein = new Protein(protein->getTopology(), positions);
      protein->setRadius(radius);
      redisplay = true;
    }
  }

  if (redisplay) this->redisplay();

  // Throttle
  if ((!pause && degreesPerSec != Vector2D()) ||
      (1 < trajectory->size() && cycle) || fast)
    idleTimer.throttle(renderSpeed);
  else idleTimer.throttle(idleSpeed);
}


SmartPointer<Client> View::createClient(const cb::IPAddress &addr) {
  return new Client(addr, slot, info, *trajectory, password);
}
