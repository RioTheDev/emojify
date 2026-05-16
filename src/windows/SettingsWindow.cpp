#include "windows/SettingsWindow.hpp"
#include "config.h"
static const std::array<Glib::ustring, 6> SKIN_TONE_LABELS = {
    "👍️", "👍🏻", "👍🏽", "👍🏽", "👍🏾", "👍🏿"};

SettingsWindow::SettingsWindow() {
  set_title("Preferences");
  set_default_size(600, 0);
  set_resizable(false);

  m_main_box.set_margin(0);
  set_child(m_main_box);

  setup_list();
  setup_bindings();
}

void SettingsWindow::setup_list() {
  m_list_box.set_selection_mode(Gtk::SelectionMode::NONE);
  m_list_box.set_margin_start(12);
  m_list_box.set_margin_end(12);
  m_list_box.set_margin_top(12);
  m_list_box.set_margin_bottom(12);
  m_list_box.get_style_context()->add_class("boxed-list");
  setup_shortcut_hint();

  m_paste_switch.set_valign(Gtk::Align::CENTER);
  m_list_box.append(
      *make_row("Paste automatically",
                "Paste the selected emoji directly into the focused window "
                "(Needs the gnome shell extension)",
                m_paste_switch));

  m_multi_emoji_switch.set_valign(Gtk::Align::CENTER);
  m_list_box.append(
      *make_row("Multi-emoji mode",
                "Allow selecting multiple emojis consecutively before pasting",
                m_multi_emoji_switch));

  m_timeout_spin.set_range(50, 5000);
  m_timeout_spin.set_increments(50, 100);
  m_timeout_spin.set_numeric(true);
  m_timeout_spin.set_valign(Gtk::Align::CENTER);
  m_timeout_spin.set_size_request(90, -1);
  m_timeout_row =
      make_row("Paste delay (ms)",
               "Time to wait after the last emoji selection before pasting",
               m_timeout_spin);
  m_list_box.append(*m_timeout_row);

  auto string_list = Gtk::StringList::create(std::vector<Glib::ustring>(
      SKIN_TONE_LABELS.begin(), SKIN_TONE_LABELS.end()));
  m_skin_tone_dropdown.set_model(string_list);
  m_skin_tone_dropdown.set_valign(Gtk::Align::CENTER);
  m_list_box.append(*make_row(
      "Skin tone", "Default skin tone used for emojis that support it",
      m_skin_tone_dropdown));

  m_background_switch.set_valign(Gtk::Align::CENTER);
  m_list_box.append(
      *make_row("Run in background",
                "Keep the app running after closing for faster startup",
                m_background_switch));
  m_column_spin.set_range(3, 10);
  m_column_spin.set_increments(1, 2);
  m_column_spin.set_numeric(true);
  m_column_spin.set_valign(Gtk::Align::CENTER);
  m_column_spin.set_size_request(90, -1);
  m_column_row =
      make_row("Columns", "Amount of columns on the emoji grid", m_column_spin);

  m_list_box.append(*m_column_row);

  m_main_box.append(m_list_box);

  auto *version_label = Gtk::make_managed<Gtk::Label>();

  Glib::ustring version_text = Glib::ustring::compose(
      "Emojify v%1 • Built with Meson %2", APP_VERSION, MESON_BUILD_VERSION);

  version_label->set_text(version_text);
  version_label->set_halign(Gtk::Align::CENTER);
  version_label->set_margin_bottom(16);

  version_label->get_style_context()->add_class("dim-label");
  version_label->get_style_context()->add_class("caption");

  m_main_box.append(*version_label);
}

void SettingsWindow::setup_bindings() {
  auto settings = SettingsManager::get_instance().get_settings();

  settings->bind("paste-on-select", &m_paste_switch, "active");
  settings->bind("multi-emoji", &m_multi_emoji_switch, "active");
  settings->bind("timeout-ms", &m_timeout_spin, "value");
  settings->bind("columns", &m_column_spin, "value");
  settings->bind("run-in-background", &m_background_switch, "active");
  m_paste_switch.set_sensitive(false); // TEMPORARY

  auto sync_timeout_sensitivity = [this]() {
    m_timeout_row->set_sensitive(m_multi_emoji_switch.get_active());
  };

  m_multi_emoji_switch.property_active().signal_changed().connect(
      sync_timeout_sensitivity);
  sync_timeout_sensitivity();

  m_skin_tone_dropdown.set_selected(settings->get_enum("skin-tone"));
  m_skin_tone_dropdown.property_selected().signal_changed().connect(
      [this, settings]() {
        settings->set_enum(
            "skin-tone", static_cast<int>(m_skin_tone_dropdown.get_selected()));
      });
}

Gtk::ListBoxRow *SettingsWindow::make_row(const Glib::ustring &title,
                                          const Glib::ustring &subtitle,
                                          Gtk::Widget &widget) {
  auto *row = Gtk::make_managed<Gtk::ListBoxRow>();
  auto *hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 12);
  hbox->set_margin(12);

  auto *label_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 2);
  label_box->set_hexpand(true);
  label_box->set_valign(Gtk::Align::CENTER);

  auto *title_label = Gtk::make_managed<Gtk::Label>(title);
  title_label->set_halign(Gtk::Align::START);
  label_box->append(*title_label);

  if (!subtitle.empty()) {
    auto *sub_label = Gtk::make_managed<Gtk::Label>(subtitle);
    sub_label->set_halign(Gtk::Align::START);
    sub_label->get_style_context()->add_class("dim-label");
    sub_label->set_wrap(true);
    sub_label->set_max_width_chars(42);
    Pango::AttrList attrs;
    auto attr = Pango::Attribute::create_attr_scale(Pango::SCALE_SMALL);
    attrs.insert(attr);
    sub_label->set_attributes(attrs);
    label_box->append(*sub_label);
  }

  hbox->append(*label_box);
  hbox->append(widget);
  row->set_child(*hbox);
  return row;
}
void SettingsWindow::setup_shortcut_hint() {

  Glib::ustring command;
  if (getenv("FLATPAK_ID") != nullptr)
    command = "flatpak run xyz.riothedev.emojify";
  else if (getenv("APPIMAGE") != nullptr)
    command = getenv("APPIMAGE");
  else
    command = "/usr/bin/emojify";

  auto *entry_box =
      Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);

  auto *entry = Gtk::make_managed<Gtk::Entry>();
  entry->set_text(command);
  entry->set_editable(false);
  entry->set_hexpand(false);
  entry->set_valign(Gtk::Align::CENTER);
  entry->set_width_chars(26);

  auto *copy_btn = Gtk::make_managed<Gtk::Button>();
  copy_btn->set_icon_name("edit-copy-symbolic");
  copy_btn->set_tooltip_text("Copy to clipboard");
  copy_btn->set_valign(Gtk::Align::CENTER);
  copy_btn->signal_clicked().connect(
      [entry]() { entry->get_clipboard()->set_text(entry->get_text()); });

  entry_box->append(*entry);
  entry_box->append(*copy_btn);

  m_list_box.append(*make_row("Keyboard shortcut",
                              "Add this command to your system's keyboard "
                              "settings to open Emojify with a shortcut",
                              *entry_box));
}