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

#include "GromacsErrorHandler.h"

#include <cbang/log/Logger.h>

#include <setjmp.h>

#if defined(HAVE_GROMACS)
extern "C" {
#include <gromacs/gmx_fatal.h>
}
#endif

using namespace cb;
using namespace FAH;

static jmp_buf jumpEnv;

#if defined(HAVE_GROMACS)
extern "C" {
  void set_program_name(char *name); // in gromacs/statutil.h

  static void error_handler(const char *msg) {
    LOG_ERROR(msg);
    longjmp(jumpEnv, 1);
  }
}
#endif

namespace FAH {
  jmp_buf &gromacs_error_handler() {
#if defined(HAVE_GROMACS)
    set_program_name((char *)"Folding@home");
    set_gmx_error_handler(error_handler);
#endif

    return jumpEnv;
  }
}
