from SCons.Script import *


def configure(conf):
    env = conf.env

    conf.CBCheckHome('glut')

    if env['PLATFORM'] == 'darwin':
        conf.RequireOSXFramework('GLUT')
        conf.CBRequireCHeader('GLUT/glut.h')
        return

    conf.CBRequireHeader('GL/glut.h')

    if not (conf.CBCheckLib('glut') or conf.CBCheckLib('glut32') or
            conf.CBCheckLib('freeglut_static') or conf.CBCheckLib('freeglut')):
        raise 'Missing glut'

    env.CBDefine('FREEGLUT_STATIC')


def generate(env):
    env.CBAddConfigTest('glut', configure)
    env.CBLoadTools('osx')


def exists(env):
    return 1
