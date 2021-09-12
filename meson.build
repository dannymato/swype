project('swipe', ['c', 'cpp'],
  version : '0.1',
  default_options : ['warning_level=3', 'c_std=c11', 'cpp_std=c++17'])

add_project_arguments(['-DWLR_USE_UNSTABLE', '-fnon-call-exceptions'], language: 'cpp')
subdir('protocols')

deps = [dependency('wlroots'), dependency('wayland-server')]


exe = executable('swipe', protocols + ['src/output.cpp', 'src/swipe.cpp', 'src/display.cpp', 'src/server.cpp', 'src/xdg_shell.cpp', 'src/view.cpp'],
  dependencies: deps,
  include_directories: include_directories('protocols'))

test('basic', exe)
