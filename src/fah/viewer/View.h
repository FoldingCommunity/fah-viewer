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

#ifndef FAH_VIEW_H
#define FAH_VIEW_H

#include "Client.h"
#include "Trajectory.h"
#include "Viewer.h"

#include <fah/viewer/basic/Texture.h>

#include <cbang/SmartPointer.h>
#include <cbang/geom/Vector.h>
#include <cbang/geom/Quaternion.h>
#include <cbang/time/Timer.h>
#include <cbang/config/Options.h>
#include <cbang/net/IPAddress.h>

#include <string>
#include <vector>


namespace FAH {
  class View : public ViewMode {
  protected:
    cb::Options &options;

    cb::SmartPointer<Client> client;
    cb::SmartPointer<ViewerBase> viewer;
    cb::SmartPointer<Trajectory> trajectory;
    SimulationInfo info;
    cb::SmartPointer<Protein> protein;

    unsigned width;
    unsigned height;

    double zoom;

    bool basic;
    bool wiggle;
    bool cycle;
    bool blur;

    std::string password;

    unsigned modeNumber;
    ViewMode mode;

    unsigned slot;

    bool pause;
    cb::QuaternionD rotation;
    cb::Vector2D degreesPerSec;

    double lastFrame;
    unsigned currentFrame;
    unsigned totalFrames;
    unsigned interpSteps;
    double fps;
    bool forward;

    std::string profile;

    cb::Timer clientUpdate;
    uint64_t connectTime;

    double renderSpeed;
    cb::Timer renderTimer;

    double idleSpeed;
    cb::Timer idleTimer;

    cb::Vector2D mousePosition;
    std::string buttonHover;

    bool showInfo;
    bool showLogos;
    bool showHelp;
    bool showAbout;
    bool showButtons;

    cb::SmartPointer<Texture> bgTexture;

    std::string connectionStatus;

  public:
    View(cb::Options &options);
    virtual ~View() {}

    void initView(const std::vector<std::string> &inputs =
                  std::vector<std::string>());

    void loadTestData();

    Client &getClient() {return *client;}
    void setClient(const cb::SmartPointer<Client> &client)
    {this->client = client;}

    ViewerBase &getViewer() {return *viewer;}
    void setViewer(const cb::SmartPointer<ViewerBase> &viewer);

    void setTrajectory(const cb::SmartPointer<Trajectory> &trajectory)
    {this->trajectory = trajectory;}
    Trajectory &getTrajectory() {return *trajectory;}

    SimulationInfo &getSimulationInfo() {return info;}
    const SimulationInfo &getSimulationInfo() const {return info;}

    void setWidth(unsigned width);
    unsigned getWidth() const {return width;}

    void setHeight(unsigned height);
    unsigned getHeight() const {return height;}

    double getZoom() const {return zoom;}

    void setBasic(bool basic) {this->basic = basic;}
    bool getBasic() const {return basic;}

    void setWiggle(bool wiggle) {this->wiggle = wiggle;}
    bool getWiggle() const {return wiggle;}

    void setCycle(bool cycle) {this->cycle = cycle;}
    bool getCycle() const {return cycle;}

    void setBlur(bool blur) {this->blur = blur;}
    bool getBlur() const {return blur;}

    void setMode(ViewMode mode);
    ViewMode getMode() const {return mode;}

    void setSlot(unsigned slot);
    unsigned getSlot() const {return slot;}

    void setPause(bool pause);
    bool getPause() const {return pause;}

    void setRotation(const cb::QuaternionD &rotation)
    {this->rotation = rotation;}
    const cb::QuaternionD &getRotation() const {return rotation;}

    void setDegreesPerSec(const cb::Vector2D &dps) {degreesPerSec = dps;}
    const cb::Vector2D &getDegreesPerSec() const {return degreesPerSec;}

    unsigned getCurrentFrame() const {return currentFrame;}
    unsigned getTotalFrames() const {return totalFrames;}
    unsigned getInterpSteps() const {return interpSteps;}

    void setFPS(double fps);
    double getFPS() const {return fps;}

    const cb::Vector2D &getMousePosition() const {return mousePosition;}

    void setShowInfo(bool showInfo) {this->showInfo = showInfo;}
    bool getShowInfo() const {return showInfo;}

    void setShowLogos(bool showLogos) {this->showLogos = showLogos;}
    bool getShowLogos() const {return showLogos;}

    void setShowHelp(bool showHelp) {this->showHelp = showHelp;}
    bool getShowHelp() const {return showHelp;}

    void setShowAbout(bool showAbout) {this->showAbout = showAbout;}
    bool getShowAbout() const {return showAbout;}

    void setShowButtons(bool showButtons) {this->showButtons = showButtons;}
    bool getShowButtons() const {return showButtons;}

    const cb::SmartPointer<Texture> &getBGTexture() const {return bgTexture;}

    const std::string &getConnectionStatus() const {return connectionStatus;}

    std::string getStatus() const;
    std::string getFrameDescription() const;

    void showPopup(const std::string &name);
    void closePopup();
    bool popupVisible();

    void zoomIn();
    void zoomOut();

    void click(const cb::Vector2D &pos);
    void hover(const cb::Vector2D &pos);

    void lineUp() {viewer->lineUp();}
    void lineDown() {viewer->lineDown();}

    void pageUp() {viewer->pageUp();}
    void pageDown() {viewer->pageDown();}

    void spinUp();
    void spinDown();
    void spinRight();
    void spinLeft();

    void draw();
    void resize(unsigned w, unsigned h);
    void update(bool fast);

    virtual cb::SmartPointer<Client> createClient(const cb::IPAddress &addr);
    virtual void redisplay() {}
    virtual void reshape(unsigned width, unsigned height) {}
  };
}

#endif // FAH_VIEW_H

