project('generate_signals', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20'])

qt6 = import('qt6')
qt6_dep = dependency('qt6', modules: ['Core'])

# These arguments are only used to build the shared library
# not the executables that use the library.
lib_args = ['-DBUILDING_GENERATE_SIGNALS']

inc = include_directories('include')

moc_file = qt6.compile_moc(
  headers: files(['include/generate_signals.hpp']),
  dependencies: qt6_dep
)

shlib = shared_library('generate_signals',
  'src/generate_signals.cpp',
  moc_file,
  install : true,
  cpp_args : lib_args,
  include_directories: inc,
  #gnu_symbol_visibility : 'hidden',
  dependencies: qt6_dep
)

moc_file_Tst = qt6.compile_moc(
  headers: files(['test/generate_signals_test.h']),
  include_directories: inc,
  dependencies: qt6_dep
)

test_exe = executable('generate_signals_test', 'test/generate_signals_test.cpp',
  moc_file_Tst,
  link_with : shlib,
  include_directories: inc,
  dependencies: qt6_dep,
)

test('generate_signals_tst', test_exe)


# Make this library usable as a Meson subproject.
generate_signals = declare_dependency(
  sources : 'include/generate_signals.hpp',
  include_directories: inc,
  link_with : shlib)

# Make this library usable from the system's
# package manager.
install_headers('include/generate_signals.hpp', subdir : 'generate_signals')

meson.override_dependency('generate_signals', generate_signals)

pkg_mod = import('pkgconfig')
pkg_mod.generate(
  name : 'generate_signals',
  filebase : 'generate_signals',
  description : 'generate_signals for cpp files and for qmllsp.',
  subdirs : 'generate_signals',
  libraries : shlib,
  version : '0.1',
)
