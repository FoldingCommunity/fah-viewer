# Setup
import os
env = Environment(ENV = os.environ)
try:
    env.Tool('config', toolpath = [os.environ.get('CBANG_HOME')])
except Exception as e:
    raise Exception('CBANG_HOME not set?\n' + str(e))

env.CBLoadTools('compiler cbang dist build_info packager resources ' +
                'fah-client-version fah-viewer')

# Override mostly_static to default True
env.CBAddVariables(
    BoolVariable('mostly_static', 'Link most libraries statically', 1))

conf = env.CBConfigure()

# Version
try:
    version = env.FAHClientVersion()
except:
    version = '0.0.0'

# Config vars
env.Replace(RESOURCES_NS = 'FAH::Viewer')
env.Replace(BUILD_INFO_NS = 'FAH::BuildInfo')

if not env.GetOption('clean'):
    conf.CBConfig('compiler')
    conf.CBConfig('fah-viewer-deps')
    env.CBDefine('GLEW_STATIC')
    env.CBDefine('USING_CBANG') # Using CBANG macro namespace

    if env['PLATFORM'] == 'posix':
        env.Append(PREFER_DYNAMIC = 'bz2 z m GLU glut'.split())

    env.CBConfConsole() # Build console app on Windows

    if env['PLATFORM'] == 'darwin':
        # Cleanup part of old package build so fah installer
        # will not bundle an old viewer and claim success
        import shutil
        shutil.rmtree('build/pkg', True)

conf.Finish()

# Viewer
Export('env')
viewer, lib = \
    SConscript('src/FAHViewer.scons', variant_dir = 'build', duplicate = 0)
Default(viewer)

# Clean
Clean(viewer, ['build', 'config.log'])

# Dist
docs = ['README.md', 'CHANGELOG.md', 'LICENSE']
distfiles = docs + [viewer, 'backgrounds', 'src', 'SConstruct']
tar = env.TarBZ2Dist('fahviewer', distfiles)
Alias('dist', tar)
AlwaysBuild(tar)

description = \
'''Folding@home is a distributed computing project using volunteered
computer resources.
'''
short_description = '''
This package contains the 3D simulation viewer.  It can connect to
local or remote FAHClients and visualize the running simulations.'''

description += short_description

if 'package' in COMMAND_LINE_TARGETS:
    # Don't package Windows here
    if env['PLATFORM'] == 'win32' or int(env.get('cross_mingw', 0)):
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
        url = 'https://foldingathome.org/',
        license = 'LICENSE',
        bug_url = 'https://foldingathome.org/',
        summary = 'Folding@home 3D Simulation Viewer',
        description = description,
        short_description = short_description,
        prefix = '/usr',
        copyright = 'Copyright 2010-2020 foldingathome.org',

        documents = docs,
        programs = [str(viewer[0])],
        desktop_menu = ['FAHViewer.desktop'],
        icons = ['images/FAHViewer-64.png', 'images/FAHViewer.icns'],
        changelog = 'CHANGELOG.md',

        deb_directory = 'debian',
        deb_section = 'science',
        deb_depends = 'libx11-6, libc6, bzip2, zlib1g, libexpat1, ' + \
            'libgl1-mesa-glx, libglu1, freeglut3',
        deb_priority = 'optional',
        deb_recommends = 'fahclient (=%s), fahcontrol (=%s)' % (
            version, version),

        rpm_license = 'Restricted',
        rpm_group = 'Applications/Internet',
        rpm_requires = 'libX11, mesa-libGL, expat, bzip2-libs, freeglut, ' + \
            'mesa-libGLU',
        rpm_build = 'rpm/build',

        app_id = 'org.foldingathome.fahviewer',
        app_resources = [['osx/Resources/', '.'],
                        ['osx/entitlements.plist', '.']],
        app_signature = '????',
        app_other_info = {'CFBundleIconFile': 'FAHViewer.icns'},
        app_shared = ['backgrounds'],
        app_programs = [str(viewer[0])],

        pkg_resources = 'osx/Resources',
        pkg_apps = [['FAHViewer.app', 'Folding@home/FAHViewer.app']],
        pkg_target = '10.7',
        pkg_scripts = 'osx/scripts',
        pkg_distribution = 'osx/distribution.xml',
        pkg_plist = 'osx/pkg.plist',
        )

    AlwaysBuild(pkg)
    env.Alias('package', pkg)

    with open('package-description.txt', 'w') as f:
        f.write(short_description.strip())
