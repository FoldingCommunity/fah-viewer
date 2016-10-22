Folding@home 3D Viewer
=========

The Folding@home viewer allows you to visualize protein folding simulations
and monitor the status of the simulation work units as they run on your
computer.  Installing and running the viewer is not necessary to run
Folding@home.

For more information see: http://folding.stanford.edu/

# Building from Source
To build FAHViewer from source you must first build C!.  This section outlines
the recommended procedure.

## Install Git & Scons
If you don't already have them install both Git and SCons:

 - http://git-scm.com/downloads
 - http://www.scons.org/download.php

Or in Debian Linux:

    sudo apt-get install scons git

## Get the Source
First create a build directory then get all the source repositories from GitHub:

    mkdir build
    cd build
    git clone https://github.com/CauldronDevelopmentLLC/cbang.git
    git clone https://github.com/FoldingAtHome/fah-viewer.git

## Setup the Environment
In the *build* directory setup some environment variables which will allow
the build systems to find each other.

In Windows:

    set BUILD_ROOT=%HOMEPATH%\path\to\build
    set CBANG_HOME=%BUILD_ROOT%\cbang
    set FAH_VIEWER_HOME=%BUILD_ROOT%\fah-viewer

Replace *%HOMEPATH%\path\to\build* with the correct path.

In Linux or OS-X:

    BUILD_ROOT=$HOME/path/to/build
    export CBANG_HOME=$BUILD_ROOT/cbang
    export FAH_VIEWER_HOME=$BUILD_ROOT/fah-viewer

Replace *$HOME/path/to/build* with the correct path.

It is often convenient to put these variables in a *env* file, or *env.bat* for
Windows.  Then you can reload the environment at any time with:

In Windows:

    env.bat

In Linux or OS-X:

    source ./env

## Build C!
See the link below for instructions:

  https://github.com/CauldronDevelopmentLLC/cbang#prerequisites

## Build FAHViewer
Once you've got the code, setup your environment and built C!:

    scons -C $FAH_VIEWER_HOME

If all goes well this will produce *FAHViewer* (or *FAHViewer.exe* in Windows)
in *$FAH_VIEWER_HOME*.

## Debug Build
To build in debug mode add `debug=1 optimze=0` to all of the *scons* commands.

## Building the Package
To build a package for your system you can run:

    scons -C $FAH_VIEWER_HOME package

## Troubleshooting
### Build Errors
If you encounter errors during the build process you can try bulding in
non-strict mode by adding `strict=0` to the *scons* commands.  This tells
the build system to not treat compile warnings as errors.

### SCons Configuration Errors
If a build fails, SCons will usually create a file called *config.log*.  If you
look towards the end of the file you can often see exactly what failed.  When
reporting build problems it is a good idea to include this file in the report.

### Resetting SCons
Sometimes SCons get's messed up.  This can happen if it is interrupted during
the configuration process.  You can delete SCons' data and start again with
the following commands:

In Windows:

    rd /S /Q .sconf_temp
    del .sconsign.dblite

In Linux or OS-X:

    rm -rf .scons*

Then try your build again.
