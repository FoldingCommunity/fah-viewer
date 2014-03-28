Folding@home 3D Viewer
=========

The Folding@home viewer allows you to visualize protein folding simulations
and monitor the status of the simulation work units as they run on your
computer.  Installing and running the viewer is not necessary to run
Folding@home.

For more information or to report a bug see:

  https:/fah.stanford.edu/projects/FAHClient/

and:

  http://folding.stanford.edu/

# Building from Source
To build FAHViewer from source you must first build C! and fah-gromacs.  This
second outlines the recommended procedure.

## Install Git & Scons
If you don't already have them install both Git and SCons:

  http://git-scm.com/downloads
  http://www.scons.org/download.php

Or in Debian Linux:

    sudo apt-get install scons git

## Get the Source
First create a build directory and get all the source repositories from GitHub:

    mkdir build
    cd build
    git clone https://github.com/CauldronDevelopmentLLC/cbang.git
    git clone https://github.com/FoldingAtHome/fah-gromacs.git
    git clone https://github.com/FoldingAtHome/fah-viewer.git

## Setup the Environment
In the *build* directory setup some environment variables which will allow
the build systems to find eachother.

In Windows:

    set BUILD_ROOT=%HOMEPATH%\path\to\build

    set CBANG_HOME=%BUILD_ROOT%\cbang
    set FAH_GROMACS_HOME=%BUILD_ROOT%\fah-gromacs
    set FAH_VIEWER_HOME=%BUILD_ROOT%\fah-viewer

Make sure you replace *%HOMEPATH%\path\to\build* with the correct path.

In Linux or OS-X:

    BUILD_ROOT=$HOME/path/to/build

    export CBANG_HOME=$BUILD_ROOT/cbang
    export FAH_GROMACS_HOME=$BUILD_ROOT/fah-gromacs
    export FAH_VIEWER_HOME=$BUILD_ROOT/fah-viewer

Make sure you replace *$HOME/path/to/build* with the correct path.

It is often convenient to put these variables in a *env* (or *env.bat* for
Windows) file.  Then you can reload the environment at any time with:

In Windows:

    env.bat

In Linux or OS-X:

    . ./env

## Build C!
See the link below for instructions:

  https://github.com/CauldronDevelopmentLLC/cbang#prerequisites

## Build FAHViewer
Once you've got the code, setup your environment and built both C!:

    scons -C $FAH_GROMACS_HOME
    scons -C $FAH_VIEWER_HOME

If all goes well this will produce *FAHViewer* (or *FAHViewer.exe* in Windows)
in *$FAH_VIEWER_HOME*.

## Debug Build
To build in debug mode add `debug=1 optimze=0` to all of the *scons* commands.

## Troubleshooting
### Build Errors
If encounter errors during the build process you can try bulding in non-strict
mode by adding `strict=0` to the *scons* commands.  This tells the bulid system
to not treat compile warnings as errors.

### SCons Configuration Errors
If a build fails usually SCons will create a file called *config.log*.  If you
look towards the end of the file you can often see exactly what failed.  When
reporting build problems it is a good idea to include this file in the report.

### Resetting SCons
Sometimes SCons get's messed up.  This can happen if it is interrupted during
the configuration process.  You can delete SCons' data and start again with
the following command:

In Windows:

    rd /S /Q .sconf_temp
    del .sconsign.dblite

In Linux or OS-X:

    rm -rf .scons*
