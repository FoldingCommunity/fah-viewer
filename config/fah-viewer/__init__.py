from SCons.Script import *


def configure_deps(conf, withGraphics = True):
    env = conf.env

    # C!
    conf.CBConfig('cbang')

    if env['PLATFORM'] == 'win32':
        from SCons.Tool.MSCommon.vc import msvc_version_to_maj_min
        maj, _ = msvc_version_to_maj_min(env['MSVC_VERSION'])

        # Visual C++ 2015 and onwards requires a compatibility library to link
        # libraries compiled with older definitions of printf and scanf, such as
        # the MSYS2 packages for freeglut compiled with MinGW.
        #     https://docs.microsoft.com/en-us/cpp/porting/visual-cpp-change-history-2003-2015?view=vs-2019#stdio_and_conio
        if maj >= 14:
            conf.CBRequireLib('legacy_stdio_definitions')

    # For viewer
    if withGraphics:
        conf.CBConfig('freetype2')
        conf.CBConfig('opengl')
        conf.CBConfig('glut')


def configure(conf, withGraphics = True):
    env = conf.env

    home = conf.CBCheckHome('fah-viewer', inc_suffix = '/src', lib_suffix = '')
    conf.CBConfig('fah-viewer-deps', withGraphics = withGraphics)

    conf.CBRequireLib('fah-viewer-resources')
    conf.CBRequireLib('fah-viewer')
    conf.CBRequireCXXHeader('fah/viewer/SimulationInfo.h')


def generate(env):
    env.CBAddConfigTest('fah-viewer', configure)
    env.CBAddConfigTest('fah-viewer-deps', configure_deps)
    env.CBLoadTools('freetype2 opengl glut')


def exists(env):
    return 1
