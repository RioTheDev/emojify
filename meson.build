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

schema_filename = 'xyz.riothedev.Emojify.gschema.xml'

install_data(schema_filename,
  install_dir: join_paths(get_option('datadir'), 'glib-2.0', 'schemas')
)

gnome.compile_schemas(depend_files: schema_filename)

inc_dir = include_directories('include')

subdir('src')