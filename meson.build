project('mpc', 'c',
  version: '0.32',
  default_options: [
    'c_std=c99',
  ],
  license: 'GPLv2+',
)

cc = meson.get_compiler('c')

conf = configuration_data()
conf.set_quoted('PACKAGE', meson.project_name())
conf.set_quoted('VERSION', meson.project_version())

iconv = get_option('iconv')
if iconv == 'false'
  iconv = false
elif cc.has_function('iconv')
  iconv = true
elif iconv == 'auto'
  iconv = false
else
  error('iconv() not available')
endif
conf.set('HAVE_ICONV', iconv)

configure_file(output: 'config.h', configuration: conf)

common_cflags = [
  # for localtime_r() with glibc
  '-D_GNU_SOURCE',
]

test_cflags = [
  '-Wall',
  '-Wextra',
  '-Wno-deprecated-declarations',
  '-Wmissing-prototypes',
  '-Wshadow',
  '-Wpointer-arith',
  '-Wstrict-prototypes',
  '-Wcast-qual',
  '-Wwrite-strings',
]

if get_option('buildtype') != 'debug'
  test_cflags += [
    '-ffunction-sections',
    '-fdata-sections',
    '-fvisibility=hidden',
  ]
endif

foreach f: test_cflags
  if cc.has_argument(f)
    common_cflags += [ f ]
  endif
endforeach

add_global_arguments(common_cflags, language: 'c')

libmpdclient_dep = dependency('libmpdclient', version: '>= 2.9')

inc = include_directories(
  'src',

  # for the generated config.h
  '.',
)

if iconv
  iconv_sources = files('src/charset.c')
else
  iconv_sources = []
endif

executable('mpc',
  'src/main.c',
  'src/list.c',
  'src/password.c',
  'src/status.c',
  'src/args.c',
  'src/format.c',
  'src/song_format.c',
  'src/util.c',
  'src/command.c',
  'src/queue.c',
  'src/output.c',
  'src/sticker.c',
  'src/tab.c',
  'src/idle.c',
  'src/message.c',
  'src/search.c',
  'src/options.c',
  'src/path.c',
  iconv_sources,
  include_directories: inc,
  dependencies: [
    libmpdclient_dep,
  ],
  install: true
)

install_data('AUTHORS', 'COPYING', 'NEWS', 'README.rst',
  install_dir : join_paths(get_option('datadir'), 'doc', meson.project_name()))

install_data(
  'contrib/mpd-m3u-handler.sh', 'contrib/mpd-pls-handler.sh',
  'contrib/mpc-completion.bash',
  install_dir: join_paths(get_option('datadir'), 'doc', meson.project_name(), 'contrib'))

if get_option('test')
  check_dep = dependency('check')
  subdir('test')
endif

subdir('doc')
