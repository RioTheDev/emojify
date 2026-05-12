#include "windows/EmojiWindow.hpp"
#include "EmojiManager.hpp"
#include "giomm.h"
#include <gtkmm/eventcontrollerkey.h>
#include <gtkmm/windowhandle.h>
#include <iostream>

EmojiWindow::EmojiWindow()
    : m_titlebar(Gtk::Orientation::HORIZONTAL),
      m_category_bar{Gtk::Orientation::HORIZONTAL} {
  set_title("Emojify");
  set_default_size(300, 400);
  set_hide_on_close(true);
  create_titlebar();

  auto controller = Gtk::EventControllerKey::create();

  controller->signal_key_pressed().connect(
      [this](guint keyval, guint keycode, Gdk::ModifierType state) {
        if (keyval == GDK_KEY_Escape) {
          this->hide();
          return true;
        }
        return false;
      },
      false);

  this->add_controller(controller);

  m_scrolled_window.set_policy(Gtk::PolicyType::NEVER,
                               Gtk::PolicyType::AUTOMATIC);
  m_scrolled_window.set_expand(true);
  m_scrolled_window.set_propagate_natural_height(true);

  m_grid.set_column_spacing(5);
  m_grid.set_row_spacing(5);
  m_grid.set_margin(10);
  m_grid.set_column_homogeneous(true);
  // m_grid.set_row_homogeneous(true);

  m_grid.set_hexpand(true);
  m_grid.set_vexpand(false);
  m_scrolled_window.set_child(m_grid);

  m_category_bar.set_spacing(2);
  m_category_bar.set_margin(4);
  m_category_bar.add_css_class("category-bar");

  auto recent_btn = Gtk::make_managed<Gtk::Button>("🕐");
  recent_btn->set_tooltip_text("Recently Used");
  recent_btn->set_has_frame(false);
  recent_btn->add_css_class("category-btn");
  recent_btn->add_css_class("category-btn--active");
  recent_btn->signal_clicked().connect([this]() { set_active_tab(0); });
  m_category_bar.append(*recent_btn);
  m_tab_buttons.push_back(recent_btn);

  for (size_t i = 0; i < std::size(groups); i++) {
    auto &gm = groups[i];
    auto btn = Gtk::make_managed<Gtk::Button>(gm.icon);
    btn->set_tooltip_text(gm.tooltip);
    btn->set_has_frame(false);
    btn->add_css_class("category-btn");
    btn->signal_clicked().connect([this, g = i]() { set_active_tab(g + 1); });

    m_category_bar.append(*btn);
    m_tab_buttons.push_back(btn);
  }

  auto main_vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
  main_vbox->append(m_scrolled_window);
  main_vbox->append(m_category_bar);
  main_vbox->set_margin(4);
  set_child(*main_vbox);

  EmojiManager::get_instance().load_recents();
  populate_grid_recent();
}

void EmojiWindow::create_titlebar() {
  auto handle = Gtk::make_managed<Gtk::WindowHandle>();

  m_titlebar.add_css_class("custom-header");
  m_titlebar.set_spacing(10);

  m_settings_btn.set_icon_name("emblem-system-symbolic");
  m_settings_btn.add_css_class("settings-btn");
  m_settings_btn.set_valign(Gtk::Align::CENTER);
  m_titlebar.append(m_settings_btn);

  m_search_entry.set_placeholder_text("Search emojis...");
  m_search_entry.set_hexpand(true);
  m_search_entry.set_valign(Gtk::Align::CENTER);
  m_search_entry.signal_changed().connect(
      sigc::mem_fun(*this, &EmojiWindow::on_search_changed));

  m_search_entry.signal_activate().connect([this]() {
    on_emoji_clicked(EmojiManager::get_instance().get_emoji_by_character(
        m_buttons[0]->get_label()));
  });

  m_titlebar.append(m_search_entry);

  m_close_btn.set_icon_name("window-close-symbolic");
  m_close_btn.add_css_class("close-btn");
  m_close_btn.set_valign(Gtk::Align::CENTER);
  m_close_btn.signal_clicked().connect([this]() { hide(); });
  m_titlebar.append(m_close_btn);

  handle->set_child(m_titlebar);
  set_titlebar(*handle);
}
void EmojiWindow::set_active_tab(size_t tab_index) {
  if (currentTab == tab_index)
    return;
  currentTab = tab_index;

  for (size_t i = 0; i < m_tab_buttons.size(); ++i) {
    if (i == tab_index)
      m_tab_buttons[i]->add_css_class("category-btn--active");
    else
      m_tab_buttons[i]->remove_css_class("category-btn--active");
  }

  if (tab_index == 0)
    populate_grid_recent();
  else
    populate_grid_group(groups[tab_index - 1].group);
}

void EmojiWindow::create_emoji_button(EmojiManager::EmojiEntry e, size_t col,
                                      size_t row) {
  auto button = Gtk::make_managed<Gtk::Button>(e.character);
  button->set_tooltip_text(e.description);

  m_grid.attach(*button, col, row, 1, 1);
  button->add_css_class("emoji-btn");
  button->signal_clicked().connect([this, e]() { on_emoji_clicked(e); });
  m_buttons.push_back(button);
}

void EmojiWindow::populate_grid_group(EmojiGroup group) {
  clear_grid();

  auto emojiList = EmojiManager::get_instance().get_all_emoji();
  auto [start, end] = EmojiManager::get_instance().get_group_range(group);

  size_t _col = 0;

  for (int i = start; i < end; ++i) {
    auto &e = emojiList[i];
    if (e.skin_tone != 0)
      continue;
    int col = _col % columns;
    int row = _col / columns;
    create_emoji_button(e, col, row);
    _col++;
  }
  if (!m_buttons.empty())
    m_buttons[0]->grab_focus();
}
void EmojiWindow::populate_grid_recent() {
  clear_grid();

  auto recents = EmojiManager::get_instance().get_recents();
  if (recents.empty()) {
    set_active_tab(1);
  }
  size_t _col = 0;
  for (auto &recent : recents) {
    int col = _col % columns;
    int row = _col / columns;
    create_emoji_button(recent, col, row);
    _col++;
  }
  if (!m_buttons.empty())
    m_buttons[0]->grab_focus();
}
void EmojiWindow::clear_grid() {
  for (auto &&btn : m_buttons) {
    btn->unparent();
  }
  m_buttons.clear();
  for (int col = 0; col < 5; ++col) {
    auto spacer = Gtk::make_managed<Gtk::Box>();
    spacer->set_hexpand(true);
    m_grid.attach(*spacer, col, 0, 1, 1);
  }
}
void EmojiWindow::on_emoji_clicked(EmojiManager::EmojiEntry e) {
  EmojiManager::get_instance().save_as_recent(e);
  EmojiManager::get_instance().save_recents();

  static std::string to_paste = "";
  static sigc::connection connection;

  to_paste += e.character;

  auto execute_logic = [this]() {
    const char *session_type = std::getenv("XDG_SESSION_TYPE");

    if (session_type && std::string(session_type) == "wayland") {
      Glib::spawn_async("", {"wl-copy", "--", to_paste},
                        Glib::SpawnFlags::SEARCH_PATH);
    } else {
      Gdk::Display::get_default()->get_clipboard()->set_text(to_paste);
    }

    try {
      auto dbus = Gio::DBus::Connection::get_sync(Gio::DBus::BusType::SESSION);
      dbus->emit_signal("/xyz/riothedev/EmojifyBridge",
                        "xyz.riothedev.EmojifyBridge", "EmojiSelected", "",
                        Glib::VariantContainerBase::create_tuple(
                            {Glib::Variant<Glib::ustring>::create(to_paste)}));
    } catch (const Glib::Error &err) {
      g_warning("%s", err.what());
    }

    hide();
    to_paste = "";
    return false;
  };

  if (connection)
    connection.disconnect();

  if (TIMEOUT_MS <= 0) {
    execute_logic();
  } else {
    connection = Glib::signal_timeout().connect(
        [execute_logic]() { return execute_logic(); }, TIMEOUT_MS);
  }
}
void EmojiWindow::on_search_changed() {
  std::string query = m_search_entry.get_text();
  std::transform(query.begin(), query.end(), query.begin(), ::tolower);

  clear_grid();

  if (query.empty()) {
    if (currentTab == 0)
      populate_grid_recent();
    else
      populate_grid_group(groups[currentTab - 1].group);
    return;
  }

  auto &db = EmojiManager::get_instance().get_all_emoji();
  uint8_t preferred_skin = 0;

  int _col = 0;

  for (auto &e : db) {
    if (e.skin_tone != preferred_skin)
      continue;

    std::string desc = e.description;
    std::string kw = e.keywords;
    std::transform(desc.begin(), desc.end(), desc.begin(), ::tolower);
    std::transform(kw.begin(), kw.end(), kw.begin(), ::tolower);

    if (desc.find(query) == std::string::npos &&
        kw.find(query) == std::string::npos)
      continue;
    int col = _col % columns;
    int row = _col / columns;
    create_emoji_button(e, col, row);
    _col++;
  }
}