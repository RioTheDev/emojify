schema_filename = 'xyz.riothedev.emojify.gschema.xml'
gnome.compile_schemas(depend_files: schema_filename)

install_data(schema_filename,
  install_dir: join_paths(get_option('datadir'), 'glib-2.0', 'schemas')
)

