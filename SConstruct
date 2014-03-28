# Setup
import os
env = Environment()
try:
    env.Tool('config', toolpath = [os.environ.get('CBANG_HOME')])
except Exception, e:
    raise Exception, 'CBANG_HOME not set?\n' + str(e)

# Override mostly_static to default True
env.CBAddVariables(
    BoolVariable('mostly_static', 'Link most libraries statically', 1))

env.CBLoadTools('compiler cbang dist build_info packager resources fah-viewer')
conf = env.CBConfigure()

# Version
version = open('version/version.txt', 'r').read().strip()
major, minor, revision = version.split('.')

# Config vars
env.Replace(PACKAGE_VERSION = version)
env.Replace(RESOURCES_NS = 'FAH::Viewer')
env.Replace(BUILD_INFO_NS = 'FAH::BuildInfo')

if not env.GetOption('clean'):
    conf.CBConfig('compiler')
    env.CBDefine('USING_CBANG') # Using CBANG macro namespace
    conf.CBConfig('fah-viewer-deps')
    env.CBDefine('GLEW_STATIC')

    # Mostly static libraries
    if env.get('mostly_static', 0):
        if env['PLATFORM'] == 'posix':
            ignores = ['pthread', 'dl', 'bz2', 'z', 'm', 'glut']
        else: ignores = None

        env.MostlyStaticLibs(ignores)

conf.Finish()

# GLEW
libGLEW = env.Library('GLEW', 'build/glew/glew.c')

# Source
subdirs = ['', 'advanced', 'basic', 'io']
src = []
for dir in subdirs:
    src += Glob('src/fah/viewer/' + dir + '/*.cpp')

# Build in 'build'
import re
VariantDir('build', 'src')
src = map(lambda path: re.sub(r'^src/', 'build/', str(path)), src)
env.AppendUnique(CPPPATH = ['#/build'])

# Resources
res = env.Resources('build/viewer-resources.cpp', ['#/src/resources/viewer'])
Precious(res)
resLib = env.Library('fah-viewer-resources', res)
Precious(resLib)


# Build lib
lib = env.Library('fah-viewer', src)


# Build Info
info = env.BuildInfo('build/build_info.cpp', [])
AlwaysBuild(info)


# FAHViewer
viewer = env.Program('#/FAHViewer',
                     ['build/FAHViewer.cpp', info, lib, resLib, libGLEW]);
Default(viewer)


# Clean
Clean(viewer, ['build', 'config.log'])

# Dist
docs = ['README.md', 'CHANGELOG.md', 'copyright']
distfiles = docs + [viewer, 'backgrounds', 'src', 'SConstruct']
tar = env.TarBZ2Dist('fahviewer', distfiles)
Alias('dist', tar)
AlwaysBuild(tar)

description = \
'''Folding@home is a distributed computing project using volunteered
computer resources run by Pandegroup of Stanford University.
'''
short_description = '''
This package contains the 3D simulation viewer.  It can connect to
local or remote FAHClients and visualize the running simulations.'''

description += short_description

if 'package' in COMMAND_LINE_TARGETS:
    # Don't package Windows here
    if env['PLATFORM'] == 'win32':
        f = open('package.txt', 'w');
        f.write('none');
        f.close()
        Exit(0)

    # Package
    pkg = env.Packager(
        'FAHViewer',

        version = version,
        maintainer = 'Joseph Coffland <joseph@cauldrondevelopment.com>',
        vendor = 'Folding@home',
        url = 'http://folding.stanford.edu/',
        license = 'copyright',
        bug_url = 'https://fah-web.stanford.edu/projects/FAHClient/',
        summary = 'Folding@home 3D Simulation Viewer',
        description = description,
        short_description = short_description,
        prefix = '/usr',
        copyright = 'Copyright 2010-2014 Stanford University',

        documents = docs,
        programs = [str(viewer[0])],
        desktop_menu = ['FAHViewer.desktop'],
        icons = ['images/FAHViewer-64.png', 'images/FAHViewer.icns'],
        changelog = 'ChangeLog',

        deb_directory = 'debian',
        deb_section = 'science',
        deb_depends = 'libx11-6, libc6, bzip2, zlib1g, libexpat1, ' + \
            'libgl1-mesa-glx, freeglut3',
        deb_priority = 'optional',
        deb_recommends = 'fahclient (=%s), fahcontrol (=%s)' % (
            version, version),

        rpm_license = 'Restricted',
        rpm_group = 'Applications/Internet',
        rpm_requires = 'libX11, mesa-libGL, expat, bzip2-libs',
        rpm_build = 'rpm/build',

        app_id = 'edu.stanford.folding.fahviewer',
        app_resources = [['osx/Resources/', '.']],
        pkg_resources = [['osx/Resources/', '.']],
        app_signature = '????',
        app_other_info = {'CFBundleIconFile': 'FAHViewer.icns'},
        app_shared = ['backgrounds'],
        app_programs = [str(viewer[0])],
        pkg_files = [['osx/FAHViewer', 'usr/bin/', 0755]],
        pkg_apps = [['FAHViewer.app', 'Folding@home/FAHViewer.app']],
        pkg_target = '10.4',
        pkg_scripts = 'osx/scripts',
        )

    AlwaysBuild(pkg)
    env.Alias('package', pkg)

    with open('package-description.txt', 'w') as f:
        f.write(short_description.strip())

