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

#include "Client.h"

#include "Positions.h"
#include "Topology.h"

#include <fah/viewer/pyon/Message.h>

#include <cbang/String.h>

#include <cbang/Catch.h>
#include <cbang/time/Time.h>
#include <cbang/time/Timer.h>
#include <cbang/iostream/ArrayDevice.h>
#include <cbang/log/Logger.h>

#include <cbang/buffer/BufferDevice.h>

#include <cbang/json/List.h>
#include <cbang/json/Dict.h>

#include <cbang/socket/SocketSet.h>

#include <string>
#include <vector>

using namespace std;
using namespace cb;
using namespace FAH;


#define CLIENT_CATCH_ERROR \
  catch (const Socket::EndOfStream &) {} \
  CATCH_ERROR


Client::Client(const IPAddress &addr, unsigned slot, SimulationInfo &info,
               Trajectory &trajectory, const string &password) :
  addr(addr), password(password), slot(slot), currentSlotID(-1),
  state(STATE_WAITING), lastConnect(0), lastData(0), waitingForUpdate(false),
  info(info), trajectory(trajectory) {

  if (!password.empty()) command = "auth \"" + password + "\"\n";

  command +=
    "updates add 0 5 $heartbeat\n"
    "updates add 1 5 $slot-info\n";
}


bool Client::setSlot(unsigned slot) {
  if (waitingForUpdate || slots.size() <= slot) return false;

  this->slot = slot;
  if (isConnected()) {
    lastConnect = 0;
    reconnect();
  }

  return true;
}


bool Client::update() {
  if (!isOpen()) state = STATE_WAITING;

  switch (state) {
  case STATE_WAITING: tryConnect(); break;
  case STATE_CONNECTING: checkConnect(); break;
  case STATE_HEADER: case STATE_DATA: {
    bool updated = false;
    double start = Timer::now();
    while (readSome()) {
      updated = true;
      if (0.25 < Timer::now() - start) break;
    }
    return updated;
  }
  }

  return false;
}


void Client::sendCommands(const string &commands) {
  string cmds = String::replace(commands, "@SLOT@", String(currentSlotID));

  // Assume we can write at least this much
  write(cmds.c_str(), cmds.size());
}


void Client::reconnect() {
  close();
  state = STATE_WAITING;
}


void Client::tryConnect() {
  if (lastConnect + 15 < Time::now())
    try {
      lastConnect = Time::now();
      setBlocking(false);
      connect(addr);
      setTimeout(0.1);
      buffer.clear();
      state = STATE_CONNECTING;
    } CLIENT_CATCH_ERROR;
}


void Client::checkConnect() {
  try {
    SocketSet socketSet;
    socketSet.add(*this, SocketSet::WRITE | SocketSet::EXCEPT);
    socketSet.select(0);

    if (!socketSet.isSet(*this, SocketSet::EXCEPT)) {
      if (socketSet.isSet(*this, SocketSet::WRITE)) {
        lastData = Time::now();
        sendCommands(command);
        state = STATE_HEADER;
      }

      return;
    }
  } CLIENT_CATCH_ERROR;

  // Some error occured
  reconnect();
}


static const char *find_string(const char *haystack, unsigned size,
                               const char *needle) {
  const char *end = haystack + size;

  for (const char *ptr = haystack; ptr < end; ptr++)
    if (*ptr == *needle) {
      const char *a = ptr;
      const char *b = needle;
      while (a < end && *a++ == *b++) if (!*b) return ptr; // Found
    }

  return 0;
}


bool Client::readSome() {
  try {
    // Make sure there is space
    if (buffer.getSpace() < 4 * 1024)
      buffer.increase(buffer.getCapacity() + 4 * 1024);

    // Read some data
    int count = read(buffer.end(), buffer.getSpace());
    if (count <= 0) {
      if (count < 0 || lastData + 20 < Time::now()) reconnect();
      return false;
    }
    buffer.incFill(count);
    lastData = Time::now();
    LOG_DEBUG(5, "Read " << count);

    if (buffer.getFill() < 15) return true; // Not enough

    switch (state) {
    case STATE_HEADER: {
      // Search for start of message
      const char *ptr =
        find_string(buffer.begin(), buffer.getFill(), "\nPyON ");

      if (!ptr) { // Not found
        if (4096 < buffer.getFill()) {
          // Discard all but the end of the buffer
          memcpy(buffer.begin(), buffer.end() - 5,  5);
          buffer.clear(); // Reset fill to zero
          buffer.incFill(5);
        }
        return true;
      }

      messageStart = ptr - buffer.begin() + 1; // Save offset
      searchOffset = messageStart + 6;
      state = STATE_DATA;
      // Fall through to next case
    }

    case STATE_DATA: {
      // Search for end of message
      const char *ptr =
        find_string(buffer.begin() + searchOffset,
                    buffer.getFill() - searchOffset, "\n---\n");

      if (!ptr) {
        searchOffset = buffer.getFill() - 5;
        return true;
      }

      // Found a complete message
      processMessage(buffer.begin() + messageStart, ptr + 5);

      // Cleanup buffer
      unsigned end = (ptr + 4) - buffer.begin();
      if (end == buffer.getFill()) buffer.clear();
      else {
        unsigned remaining = buffer.getFill() - end;
        memmove(buffer.begin(), buffer.begin() + end, remaining);
        buffer.clear();
        buffer.incFill(remaining);
      }

      state = STATE_HEADER;
      return true;
    }

    default: THROW("Invalid state");
    }
  } CLIENT_CATCH_ERROR;

  reconnect();
  return false;
}


void Client::processMessage(const char *start, const char *end) {
  PyON::Message msg;
  ArrayStream<const char> stream(start, end - start);

  stream >> msg;
  if (!msg.isValid()) THROW("Invalid message");

  LOG_DEBUG(4, "Received " << msg.getType());

  try {
    handleMessage(msg);
  } CLIENT_CATCH_ERROR;
}


void Client::handleMessage(const PyON::Message &msg) {
  if (msg.getType() == "slots") {
    auto &list = msg.get()->getList();
    has_loadable_slot = false;
    has_running_gpu_slot = false;

    for (unsigned i = 0; i < list.size(); i++) {
      slots.push_back(String::parseU64(list.getDict(i)["id"]->getString()));
      if (list.getDict(i)["status"]->getString() == "RUNNING")
        if ((list.getDict(i)["description"]->getString()).substr(0,3) == "cpu")
          has_loadable_slot = true;
        else if((list.getDict(i)["description"]->getString()).substr(0,3) == "gpu")
          has_running_gpu_slot = true;
    }

    if (!slots.empty()) {
      slot %= slots.size();
      if ((int64_t)slots[slot] != currentSlotID) {
        currentSlotID = slots[slot];

        string cmd =
            "updates add 2 5 $(simulation-info @SLOT@)\n";

        if (command.find(cmd) == string::npos) command += cmd;
        sendCommands(cmd);
      }
    }

  } else if (msg.getType() == "topology") {
    SmartPointer<Topology> topology = new Topology;
    topology->loadJSON(*msg.get());
    trajectory.clear();
    trajectory.setTopology(topology);
    waitingForUpdate = false;

  } else if (msg.getType() == "positions") {      
    SmartPointer<Positions> positions = new Positions;
    positions->loadJSON(*msg.get());   
    trajectory.add(positions);               

  } else if (msg.getType() == "simulation-info") {
      const JSON::Value& value = *msg.get();
      auto& dict = value.getDict();    
      uint32_t coreType = (uint32_t)dict["core_type"]->getNumber();   

      switch (coreType) {
      case 34:
          info.loadJSON(*msg.get());
      case 0:          
          if (!setSlot(slot + 1))
              if (slot)
                  setSlot(slot - 1);
          break;
      default:
          if (info.coreType == 0) {
              string cmd =
                  "updates add 3 5 $(trajectory @SLOT@)\n";

              if (command.find(cmd) == string::npos) command += cmd;
              sendCommands(cmd);
          }
          info.loadJSON(*msg.get());
          if (has_running_gpu_slot) has_loadable_slot = true;
          break;
      }
  }
}
