project('emojify', 'cpp',
  version: '1.0',
  default_options: [
    'cpp_std=c++20',
    'warning_level=2' 
  ]
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

install_data('data/xyz.riothedev.emojify.desktop', 
             install_dir: get_option('datadir') / 'applications')

install_data('data/xyz.riothedev.emojify.svg',
             install_dir: get_option('datadir') / 'icons' / 'hicolor' / 'scalable' / 'apps')
             
gnome.post_install(
  glib_compile_schemas: true,
  update_desktop_database: true,
)
inc_dir = include_directories('include')

subdir('src')