sources = files(
    'main.cpp',
    'windows/EmojiWindow.cpp',
    'EmojiManager.cpp',
    'SettingsManager.cpp',
)

executable('emojify', 
  sources,res,
  include_directories: inc_dir,
  dependencies:[gtkmm_dep, glib_dep, giomm_dep,adwaita_dep],
  install: true
)