from SCons.Script import *


def configure(conf):
    env = conf.env

    conf.CBCheckHome('glut')

    if env['PLATFORM'] == 'darwin':
        conf.RequireOSXFramework('GLUT')
        conf.CBRequireCHeader('GLUT/glut.h')
        return

    conf.CBRequireHeader('GL/glut.h')

    if env['PLATFORM'] == 'win32' or int(env.get('cross_mingw', 0)):
        if not conf.CBCheckLib('freeglut_static'):
            conf.CBRequireLib('glut32')

        env.CBDefine('FREEGLUT_STATIC')
        return

    conf.CBRequireLib('glut')


def generate(env):
    env.CBAddConfigTest('glut', configure)
    env.CBLoadTools('osx')


def exists(env):
    return 1
