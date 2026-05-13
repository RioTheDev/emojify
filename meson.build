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

schema_filename = 'data/xyz.riothedev.Emojify.gschema.xml'

install_data(schema_filename,
  install_dir: join_paths(get_option('datadir'), 'glib-2.0', 'schemas')
)

gnome.compile_schemas(depend_files: schema_filename)

install_data('data/xyz.riothedev.Emojify.desktop', 
             install_dir: get_option('datadir') / 'applications')

install_data('data/xyz.riothedev.Emojify.svg',
             install_dir: get_option('datadir') / 'icons' / 'hicolor' / 'scalable' / 'apps')
             
gnome.post_install(
  glib_compile_schemas: true,
  update_desktop_database: true,
)
inc_dir = include_directories('include')

subdir('src')