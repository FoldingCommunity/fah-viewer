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

#ifndef FAH_SIMULATION_INFO_H
#define FAH_SIMULATION_INFO_H

#include <cbang/SmartPointer.h>
#include <cbang/StdTypes.h>

#include <cbang/pyon/Object.h>

#include <iostream>
#include <string>


namespace FAH {
  // TODO Convert structure to an X-Macro
  class SimulationInfo : public cb::PyON::Object {
  public:
    std::string user;
    std::string team;

    uint32_t project;
    uint32_t run;
    uint32_t clone;
    uint32_t gen;

    uint32_t coreType;
    std::string core;

    uint64_t totalIterations;
    uint64_t iterationsDone;

    double energy;
    double temperature;

    uint64_t startTime;
    uint64_t timeout;
    uint64_t deadline;
    uint64_t eta;
    double progress;

    uint32_t slot;

    SimulationInfo();

    // From PyONObject
    const char *getPyONType() const {return "simulation-info";}
    cb::SmartPointer<cb::JSON::Value> getJSON() const;
    void loadJSON(const cb::JSON::Value &value);
  };
}

#endif // FAH_SIMULATION_INFO_H
