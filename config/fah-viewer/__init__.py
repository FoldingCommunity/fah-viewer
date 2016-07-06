from SCons.Script import *


def configure_deps(conf, withGraphics = True):
    env = conf.env

    # C!
    conf.CBConfig('cbang')

    # For viewer
    if withGraphics:
        conf.CBConfig('freetype2')
        conf.CBConfig('opengl')
        conf.CBConfig('glut')

    # gromacs
    conf.CBConfig('fah-gromacs')


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
    env.CBLoadTools('freetype2 opengl glut fah-gromacs')


def exists(env):
    return 1
