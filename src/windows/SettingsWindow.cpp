#include "windows/SettingsWindow.hpp"

static const std::array<Glib::ustring, 6> SKIN_TONE_LABELS = {
    "👍️", "👍🏻", "👍🏽", "👍🏽", "👍🏾", "👍🏿"};

SettingsWindow::SettingsWindow() {
  set_title("Preferences");
  set_default_size(420, 0);
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

  m_paste_switch.set_valign(Gtk::Align::CENTER);
  m_list_box.append(
      *make_row("Paste automatically",
                "Paste the selected emoji directly into the focused window",
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
      make_row("Paste adelay (ms)",
               "Time to wait after the last emoji selection before pasting",
               m_column_spin);

  m_list_box.append(*m_column_row);

  m_main_box.append(m_list_box);
}

void SettingsWindow::setup_bindings() {
  auto settings = SettingsManager::get_instance().get_settings();

  settings->bind("paste-on-select", &m_paste_switch, "active");
  settings->bind("multi-emoji", &m_multi_emoji_switch, "active");
  settings->bind("timeout-ms", &m_timeout_spin, "value");
  settings->bind("columns", &m_column_spin, "value");
  settings->bind("run-in-background", &m_background_switch, "active");

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