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

#include "SimulationInfo.h"

#include <cbang/Exception.h>
#include <cbang/String.h>

#include <cbang/json/Dict.h>

#include <cbang/util/StringMap.h>
#include <cbang/time/Time.h>

using namespace std;
using namespace cb;
using namespace FAH;


SimulationInfo::SimulationInfo() :
  project(0), run(0), clone(0), gen(0), coreType(0), totalIterations(0),
  iterationsDone(0), energy(0), temperature(0), startTime(0), timeout(0),
  deadline(0), eta(0), progress(0), slot(0) {}


SmartPointer<JSON::Value> SimulationInfo::getJSON() const {
  SmartPointer<JSON::Value> dict = new JSON::Dict;

  dict->insert("user",             user);
  dict->insert("team",             team);
  dict->insert("project",          project);
  dict->insert("run",              run);
  dict->insert("clone",            clone);
  dict->insert("gen",              gen);
  dict->insert("core_type",        coreType);
  dict->insert("core",             core);
  dict->insert("total_iterations", totalIterations);
  dict->insert("iterations_done",  iterationsDone);
  dict->insert("energy",           energy);
  dict->insert("temperature",      temperature);
  dict->insert("start_time",       Time(startTime).toString());
  dict->insert("timeout",          timeout);
  dict->insert("deadline",         deadline);
  dict->insert("eta",              eta);
  dict->insert("progress",         progress);
  dict->insert("slot",             slot);

  return dict;
}


void SimulationInfo::loadJSON(const JSON::Value &value) {
  auto &dict = value.getDict();

  if (dict.has("user")) user = dict["user"]->getString();
  if (dict.has("team")) team = dict["team"]->getString();
  if (dict.has("project")) project = (uint32_t)dict["project"]->getNumber();
  if (dict.has("run")) run = (uint32_t)dict["run"]->getNumber();
  if (dict.has("clone")) clone = (uint32_t)dict["clone"]->getNumber();
  if (dict.has("gen")) gen = (uint32_t)dict["gen"]->getNumber();
  if (dict.has("core_type"))
    coreType = (uint32_t)dict["core_type"]->getNumber();
  if (dict.has("core")) core = dict["core"]->getString();
  if (dict.has("total_iterations"))
    totalIterations = (uint64_t)dict["total_iterations"]->getNumber();
  if (dict.has("iterations_done"))
    iterationsDone = (uint64_t)dict["iterations_done"]->getNumber();
  if (dict.has("energy")) energy = dict["energy"]->getNumber();
  if (dict.has("temperature")) temperature = dict["temperature"]->getNumber();
  if (dict.has("start_time"))
    startTime = Time::parse(dict["start_time"]->getString());
  if (dict.has("timeout")) timeout = (uint64_t)dict["timeout"]->getNumber();
  if (dict.has("deadline")) deadline = (uint64_t)dict["deadline"]->getNumber();
  if (dict.has("eta")) eta = (uint64_t)dict["eta"]->getNumber();
  if (dict.has("progress")) progress = dict["progress"]->getNumber();
  if (dict.has("slot")) slot = (uint32_t)dict["slot"]->getNumber();
}
