#pragma once

#include "SettingsManager.hpp"
#include <gtkmm.h>

class SettingsWindow : public Gtk::Window {
public:
  SettingsWindow();

private:
  Gtk::Box m_main_box{Gtk::Orientation::VERTICAL, 0};
  Gtk::ListBox m_list_box;

  Gtk::Switch m_paste_switch;
  Gtk::Switch m_multi_emoji_switch;
  Gtk::SpinButton m_timeout_spin;
  Gtk::SpinButton m_column_spin;
  Gtk::DropDown m_skin_tone_dropdown;
  Gtk::Switch m_background_switch;

  Gtk::ListBoxRow *m_timeout_row{nullptr};
  Gtk::ListBoxRow *m_column_row{nullptr};

  void setup_list();
  void setup_bindings();
  void setup_shortcut_hint();
  Gtk::ListBoxRow *make_row(const Glib::ustring &title,
                            const Glib::ustring &subtitle, Gtk::Widget &widget);
};