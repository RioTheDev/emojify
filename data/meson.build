schema_filename = 'xyz.riothedev.emojify.gschema.xml'
gnome.compile_schemas(depend_files: schema_filename)

install_data(schema_filename,
  install_dir: join_paths(get_option('datadir'), 'glib-2.0', 'schemas')
)

install_data('xyz.riothedev.emojify.desktop', 
             install_dir: get_option('datadir') / 'applications')

install_data('xyz.riothedev.emojify.svg',
             install_dir: get_option('datadir') / 'icons' / 'hicolor' / 'scalable' / 'apps')

install_data('xyz.riothedev.emojify.metainfo.xml',
             install_dir: get_option('datadir') / 'metainfo')

desktop_file_validate = find_program('desktop-file-validate', required: false)
if desktop_file_validate.found()
  test('Validate desktop file',
    desktop_file_validate,
    args: [meson.current_source_dir() / 'xyz.riothedev.emojify.desktop']
  )
endif

appstream_util = find_program('appstream-util', required: false)
if appstream_util.found()
  test('Validate metainfo',
    appstream_util,
    args: ['validate', meson.current_source_dir() / 'xyz.riothedev.emojify.metainfo.xml']
  )
endif