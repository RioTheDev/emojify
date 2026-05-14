#pragma once

#include "EmojiManager.hpp"
#include "windows/SettingsWindow.hpp"
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/grid.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/searchentry.h>
#include <gtkmm/window.h>
#include <vector>
class EmojiWindow : public Gtk::Window {
public:
  EmojiWindow();
  virtual ~EmojiWindow() = default;
  void reset_data();

protected:
  void create_titlebar();
  void change_category();
  struct GroupMeta {
    EmojiGroup group;
    const char *icon;
    const char *tooltip;
  };
  static constexpr EmojiWindow::GroupMeta groups[] = {
      {EmojiGroup::SmileysEmotion, "😀", "Smileys & Emotion"},
      {EmojiGroup::PeopleBody, "👋", "People & Body"},
      {EmojiGroup::AnimalsNature, "🐶", "Animals & Nature"},
      {EmojiGroup::FoodDrink, "🍎", "Food & Drink"},
      {EmojiGroup::TravelPlaces, "✈️", "Travel & Places"},
      {EmojiGroup::Activities, "⚽", "Activities"},
      {EmojiGroup::Objects, "💡", "Objects"},
      {EmojiGroup::Symbols, "❤️", "Symbols"},
      {EmojiGroup::Flags, "🏁", "Flags"},
  };
  size_t currentTab = 0;

  Gtk::Box m_titlebar;
  Gtk::Button m_close_btn;
  Gtk::Button m_settings_btn;
  Gtk::SearchEntry m_search_entry;

  Gtk::ScrolledWindow m_scrolled_window;
  Gtk::Grid m_grid;
  std::vector<Gtk::Button *> m_buttons;
  std::unique_ptr<SettingsWindow> m_settings_window;
  Gtk::Box m_category_bar;
  std::vector<Gtk::Button *> m_tab_buttons;
  Gtk::WindowHandle m_handle;
  void set_active_tab(size_t tab_index);
  void populate_grid_recent();
  void clear_grid();
  void populate_grid_group(EmojiGroup group);
  void create_emoji_button(EmojiManager::EmojiEntry e, size_t col, size_t row);
  void on_search_changed();
  void on_settings_clicked();
  void on_emoji_clicked(EmojiManager::EmojiEntry e);
};
