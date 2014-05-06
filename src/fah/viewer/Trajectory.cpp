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

#include "Trajectory.h"

#include <fah/viewer/io/XYZReader.h>
#include <fah/viewer/io/TPRReader.h>
#include <fah/viewer/io/XTCReader.h>
#include <fah/viewer/io/TRNReader.h>

#include <cbang/Exception.h>
#include <cbang/Math.h>
#include <cbang/String.h>
#include <cbang/log/Logger.h>
#include <cbang/util/Random.h>
#include <cbang/json/JSON.h>

using namespace std;
using namespace cb;
using namespace FAH;


SmartPointer<Protein> Trajectory::getProtein(unsigned i) {
  SmartPointer<Protein> protein = new Protein(topology, at(i));

  // Get maximum radius to avoid zooming effect
  double radius = 0;
  for (unsigned i = 0; i < size(); i++)
    if (radius < at(i)->getRadius()) radius = at(i)->getRadius();
  protein->setRadius(radius);

  return protein;
}


void Trajectory::add(const SmartPointer<Positions> &positions) {
  if (positions->empty()) THROW("Not adding empty positions");

  if (!topology->isEmpty()) {
    if (positions->size() != topology->getAtoms().size())
      LOG_ERROR("Size of positions " << positions->size()
                << " does not match topology " << topology->getAtoms().size());

  } else if (!empty()) {
    SmartPointer<Positions> last = back();
    if (positions->size() != last->size())
      LOG_ERROR("Size of positions " << positions->size()
                << " does not match trajectory " << last->size());
  }

  shiftIntoBox(*positions);
  if (center) positions->translateToCenterOfMass();
  if (align) alignToLast(*positions);
  if (interpolate) interpolateTo(*positions);

  if (!topology.isNull() && topology->getBonds().empty())
    topology->findBonds(*positions);

  push_back(positions);
}


void Trajectory::readXYZ(const string &filename) {
  SmartPointer<Positions> positions = new Positions;
  XYZReader(filename).read(*positions, topology.get());
  topology->setTS();
  add(positions);
}


void Trajectory::readTPR(const string &filename) {
  SmartPointer<Positions> positions = new Positions;
  TPRReader(filename).read(*positions, *topology);
  topology->setTS();
  add(positions);
}


void Trajectory::readXTC(const string &filename, bool onlyNewFrames) {
  XTCReader reader(filename);

  for (unsigned count = 0; true; count++) {
    SmartPointer<Positions> positions = new Positions;
    if (!reader.read(*positions, topology->isEmpty() ? 0 : topology.get()))
      break;
    if (!onlyNewFrames || size() <= count) {
      add(positions);
      onlyNewFrames = false;
    }
  }
}


void Trajectory::readTRN(const string &filename, bool onlyNewFrames) {
  TRNReader reader(filename);

  for (unsigned count = 0; true; count++) {
    SmartPointer<Positions> positions = new Positions;
    if (!reader.read(*positions, topology->isEmpty() ? 0 : topology.get()))
      break;
    if (!onlyNewFrames || size() <= count) {
      add(positions);
      onlyNewFrames = false;
    }
  }
}


void Trajectory::readJSON(const string &filename) {
  JSON::Reader reader(filename);
  JSON::ValuePtr data = reader.parse();

  if (data->isDict()) {
    // Units
    float scale = 10;
    if (data->has("units")) {
      string units = String::toUpper(data->getString("units"));

      if (units == "NM" || units == "NANOMETERS") scale = 1;
      else if (units != "A" && units != "ANGSTROM" && units == "ANGSTROMS")
        LOG_WARNING("Unrecognized units '" << data->getString("units") << "'");
    }

    // Topology
    if (data->has("atoms")) {
      topology->loadJSON(*data, scale);
      topology->setTS();
    }

    // Positions
    if (data->has("positions")) {
      JSON::List list = data->getList("positions");
      for (unsigned i = 0; i < list.size(); i++)
        add(new Positions(list.getList(i), scale));
    }

  } else add(new Positions(*data, 10));
}


void Trajectory::ensureTopology() {
  if (!topology->isEmpty()) return;
  if (empty()) THROW("Cannot create topology with no positions");

  Atom atom("C");
  for (unsigned i = 0; i < front()->size(); i++) topology->add(atom);
}


void Trajectory::recomputeBonds() {
  if (empty()) return;
  ensureTopology();
  topology->findBonds(*at(0));
}


void Trajectory::shiftIntoBox(Positions &p) {
  if (p.getBox().empty()) return;
  const vector<Vector3D> &box = p.getBox();

  if (offsets.size() < p.size()) offsets.resize(p.size());

  // NOTE: This method was stolen from:
  //    gromacs-4.5.4/src/gmxlib/rmpbc.c:rm_gropbc()
  //  With the difference that we are accumulating offsets over the whole
  //  trajectory.

  for (unsigned n = 1; n < p.size(); n++) {
    Vector3D orig = p[n];
    p[n] += offsets[n];

    for (int m = 2; 0 <= m; m--) {
      double dist;

      while (0.75 * box[m][m] < fabs(dist = p[n][m] - p[n - 1][m])) {
        if (10 * box[m][m] < fabs(dist)) continue; // Ignore unreasonable
        if (0 < dist) for (int d = 0; d <= m; d++) p[n][d] -= box[m][d];
        else for (int d = 0; d <= m; d++) p[n][d] += box[m][d];
      }
    }

    offsets[n] = p[n] - orig;
  }
}


static double alignment(const Positions &p1, const Positions &p2) {
  // Computes a metric of how aligned the two sets of positions are
  double d = 0;

  // Only consider every 8th position for better performance
  for (unsigned i = 1; i < p1.size(); i += 8)
    d += p1[i].distanceSquared(p2[i]);

  return sqrt(d) * 8 / p1.size();
}


void Trajectory::alignToLast(Positions &p) {
  // This function uses a random search, similar to simulated annealing, to
  // find a rotation of the current positions which is close to the previous.
  // This helps stabilize the view of the protein and improve interpolation
  // between frames.

  if (empty() || p.empty()) return;

  const Positions &last = *back();

  double align = alignment(p, last);
  double start = align;
  double angle = 2 * M_PI;
  unsigned rounds = 64;
  const double magicStoppingPoint = 0.3;
  double rate = pow(4.0 / 360.0, 1.0 / rounds); // Within 4 degrees

  for (unsigned j = 0; magicStoppingPoint < align && j < rounds; j++) {
    for (unsigned l = 0; l < 2; l++) {
      uint8_t r = Random::instance().rand<uint8_t>();
      Vector3D v(!(r & 1), !(r & 2), !(r & 4));
      AxisAngleD a(angle * l ? -1 : 1, v.normalize());

      // Rotate
      Positions tmp(p);
      for (unsigned i = 0; i < p.size(); i++)
        tmp[i] = a.rotate(tmp[i]);

      // Recheck
      double newAlign = alignment(tmp, last);
      if (newAlign < align) {
        // Accept
        align = newAlign;
        p = tmp;
      }
    }

    angle *= rate;
  }

  if (magicStoppingPoint <= start)
    LOG_DEBUG(3, "Alignment start=" << start << " end=" << align << " improved "
              << String::printf("%0.2f%%", (1.0 - align / start) * 100));
}


void Trajectory::interpolateTo(const Positions &p2) {
  if (empty()) return;

  // Linear interpolation between the positions
  const Positions &p1 = *back();
  for (unsigned i = 0; i < interpolate; i++) {
    SmartPointer<Positions> step = new Positions(p1);

    double t = (double)(i + 1) / (interpolate + 1);
    for (unsigned i = 0; i < p1.size(); i++)
      step->at(i) = p1[i].intersect(p2[i], t);

    step->init();
    push_back(step);
  }
}
