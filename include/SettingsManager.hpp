#pragma once

#include <giomm/settings.h>
#include <glibmm.h>

class SettingsManager {
public:
  static SettingsManager &get_instance() {
    static SettingsManager instance;
    return instance;
  };
  SettingsManager(const SettingsManager &) = delete;
  SettingsManager &operator=(const SettingsManager &) = delete;

  int get_timeout_ms() const;
  int get_skin_tone() const;
  int get_columns() const;
  bool get_paste_on_select() const;
  bool get_run_in_background() const;
  bool get_multi_emoji() const;

  void set_timeout_ms(int ms);
  void set_skin_tone(int tone);
  void set_paste_on_select(bool value);
  void set_run_in_background(bool value);
  void set_multi_emoji(bool value);

  Glib::RefPtr<Gio::Settings> get_settings() { return m_settings; }

private:
  SettingsManager();
  Glib::RefPtr<Gio::Settings> m_settings;

  const Glib::ustring KEY_TIMEOUT = "timeout-ms";
  const Glib::ustring KEY_COLUMNS = "columns";
  const Glib::ustring KEY_SKIN_TONE = "skin-tone";
  const Glib::ustring KEY_PASTE = "paste-on-select";
  const Glib::ustring KEY_RUN_IN_BG = "run-in-background";
  const Glib::ustring KEY_MULTI_EMOJI = "multi-emoji";
  const Glib::ustring SCHEMA_ID = "xyz.riothedev.emojify";
};