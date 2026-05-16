project('emojify', 'cpp',
  version: '1.2.12',
  default_options: [
    'cpp_std=c++20',
    'warning_level=2' 
  ]
)

conf_data = configuration_data()

conf_data.set('APP_VERSION', meson.project_version())
conf_data.set('MESON_VERSION', meson.version())

configure_file(
  input: 'include/config.h.in',
  output: 'config.h',
  configuration: conf_data
)

gtkmm_dep = dependency('gtkmm-4.0')
glib_dep = dependency('glibmm-2.68')
giomm_dep = dependency('giomm-2.68')
gnome = import('gnome')
adwaita_dep = dependency('libadwaita-1')

res = gnome.compile_resources(
    'emojify_resources',
    'resources.gresource.xml',
    source_dir: '.'
)

subdir('data')



gnome.post_install(
  glib_compile_schemas: true,
  update_desktop_database: true,
)
inc_dir = include_directories('include')

subdir('src')