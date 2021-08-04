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

#pragma once

#include "SimulationInfo.h"
#include "Trajectory.h"

#include <cbang/StdTypes.h>

#include <cbang/socket/Socket.h>

#include <cbang/buffer/MemoryBuffer.h>

#include <vector>


namespace FAH {
  namespace PyON {class Message;}


  class Client : public cb::Socket {
  public:
    typedef enum {
      STATE_WAITING,
      STATE_CONNECTING,
      STATE_HEADER,
      STATE_DATA,
    } state_t;

  protected:
    cb::IPAddress addr;
    std::string password;
    std::string command;
    std::vector<uint64_t> slots;
    unsigned slot;
    int64_t currentSlotID;

  private:
    state_t state;
    uint64_t lastConnect;
    uint64_t lastData;
    bool waitingForUpdate;

    bool override;
    bool has_loadable_slot = false;
    bool has_running_gpu = false;

    cb::MemoryBuffer buffer;
    unsigned searchOffset;
    unsigned messageStart;

  protected:
    SimulationInfo &info;
    Trajectory &trajectory;

  public:
    Client(const cb::IPAddress &addr, unsigned slot,
           SimulationInfo &info, Trajectory &trajectory,
           const std::string &password = std::string());
    virtual ~Client() {}

    const std::string &getCommand() const {return command;}
    void setCommand(const std::string &command) {this->command = command;}

    bool isConnected() const {return STATE_CONNECTING < state;}
    bool hasLoadableSlot() const {return has_loadable_slot;}
    state_t getState() const {return state;}

    bool setSlot(unsigned slot);
    unsigned getSlot() const {return slot;}

    bool update();

  protected:
    void sendCommands(const std::string &commands);
    void reconnect();
    void tryConnect();
    void checkConnect();
    bool readSome();
    void processMessage(const char *start, const char *end);
    virtual void handleMessage(const PyON::Message &msg);
  };
}
