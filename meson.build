project('swipe', ['c', 'cpp'],
  version : '0.1',
  default_options : ['warning_level=3', 'c_std=c11', 'cpp_std=c++20'])

add_project_arguments(['-DWLR_USE_UNSTABLE', '-fnon-call-exceptions'], language: 'cpp')
subdir('protocols')


subproject('wlroots')
subproject('wayland-protocols')
deps = [dependency('wlroots'), dependency('wayland-server')]

srcfiles = [
  'src/swipe.cpp',
  'src/output.cpp',
  'src/cursor.cpp',
  'src/display.cpp',
  'src/server.cpp',
  'src/xdg_shell.cpp',
  'src/view.cpp',
  'src/seat.cpp',
  'src/keyboard.cpp'
]

exe = executable('swipe', protocols + srcfiles,
  dependencies: deps,
  include_directories: include_directories('protocols'))

test('basic', exe)
