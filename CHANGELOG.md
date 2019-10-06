Folding@home 3D Viewer Changelog
================================

## v7.5.1
 - OSX installer updates. @kbernhagen
 - Fixed PyON message parsing.

## v7.4.17
 - Fixed ``terminate called. . .'cb::Socket::EndOfStream'`` on client exit.

## v7.4.16
 - Read JSON files from command line.
 - Use basic mode by default.
 - Removed support for reading Gromacs format files .trn, .tpr, .xtc.
 - Removed ``--xyz <file>`` option.  Not needed.

## v7.4.5
 - Show snapshot for command line loaded proteins.
 - Don't try to shift atoms which are unreasonably far from center.

## v7.3.13
 - ESC ends fullscreen mode.  #1067
 - Default screen size changed to 800x600.  #1081

## v7.3.11
 - Automatically detect type of input files on command line.
 - Read JSON files.  #980

## v7.3.0
 - Updated copyright dates.

## v7.2.13
 - Removed project description.

## v7.2.10
 - Fixed slot handling when there are gaps in the sequence of IDs.
 - Improved automatic zooming.
 - Don't display info when values are unknown.

## v7.2.9
 - Fixed simulation info update when slot is loading.
 - Fixed blank protein when viewer started while client still loading.

## v7.2.8
 - Use password to authenticate with remote clients.
 - Link libssl libcrypto and libexpat statically in .deb.

## v7.2.7
 - Fixed disappearing protein on window click in Windows.

## v7.2.6
 - Fixed crash with OpenGL < 1.4 in button highlighting code.
 - Attempt to fix disappearing protein on window refocus.
 - Load chemical bonds from .tpr rather than recomputing them.
 - Eliminated rotation jump after unpause.
 - Further improved compensation for periodic boundaries.

## v7.2.5
 - Added missing resource files.

## v7.2.4
 - Fixed textures at start with advanced modes.
 - Fixed bonds, broken in v7.2.3.
 - Automatically adjust text popup height based on windows size.

## v7.2.3
 - Only warn when an invalid 'profile' option is specified.
 - Fixed atom type handling for atoms with lower-cased names.
 - Display all atom groups which are not just hydrogens and oxygens.
 - Try to realign protein between snapsots by rotating.
 - Fixed .tpr/.xtc loading from the command line, broken in v7.2.2.

## v7.2.2
 - Fixed zooming in and out when protein size changes in trajectory.
 - Fixed crashing in pre OpenGL 1.5 due lack of VBO support.
 - Don't show water chains.
 - Check for and correct negative atom radius.
 - Fixed demo timeout logic and display.
 - Allow switching slots with [ and ].
 - Wiggle even if only displaying one frame.
 - Make mode 4 (advanced) default with OpenGL > 2.2.

## v7.2.1
 - ESC does not quit app only popups.
 - Use basic mode by default. #817, #864
 - Added Installed-Size control field to .deb.  #853
 - Major performance improvements for "basic" modes.
 - Fixed split proteins caused by periodic boundaries.
 - Download whole trajectories from client not just snapshots.
 - Partially fixed shadows in advanced modes.
 - Allow disabling blur in advanced modes for better performance.
 - Fixed texture alpha blending.
 - Combined basic and advanced modes.
 - Option 'override-blacklist'->'force', forces enabling advanced modes.

## v7.2.0
 - Try again on statically linking with libGLEW. #900
 - Fixed popups in basic view.
 - Fixed basic view transparent atoms problem.
 - Support reading .tpr files.

## v7.1.52
 - Link statically with libGLEW.

## v7.1.46
 - Integrated caxalot's OSX launcher script.
 - Statically link libGLEW to avoid 1.5/1.6 dep problems.  #792

## v7.1.45
 - Use more fine grained WU progress estimated by the clinet.  #808
 - Fixed bug which caused viewer to become jerky after client reconnect.
 - Fixed view/client eta/progress synchronization problems.  #811
 - Check OSX app signature to ???.  #821

## v7.1.43
 - Updated copyright dates.

## v7.1.40
 - Fixed deb package problems.

## v7.1.39
 - Initial separate 3D viewer package
 - Ignore SIGPIPE which kills viewer in OSX.
