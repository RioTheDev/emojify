#include "windows/LoadingWindow.hpp"
#include <EmojiManager.hpp>
#include <gtkmm/messagedialog.h>
#include <iostream>
LoadingWindow::LoadingWindow() : m_box(Gtk::Orientation::VERTICAL) {
  set_title("Loading Emojis");
  set_default_size(400, 100);
  set_hide_on_close(true);

  m_box.set_margin(20);
  m_box.set_spacing(10);
  m_box.set_valign(Gtk::Align::CENTER);
  m_box.set_halign(Gtk::Align::CENTER);

  set_child(m_box);

  m_label.set_markup("<span weight='bold'>Starting setup...</span>");

  m_progress.set_size_request(300, 12);
  m_progress.add_css_class("loading-bar");

  m_box.append(m_label);
  m_box.append(m_progress);

  EmojiManager::get_instance().signal_setup_progress().connect(
      [this](EmojiManager::ProgressValue progressData) {
        update_status(progressData.stage);
        m_progress.set_fraction(1);
        if (progressData.fraction >= 1.0) {
          this->m_signal_done.emit();
        }
      });
  EmojiManager::get_instance().signal_setup_error().connect(
      [this](std::string error) { show_fatal_error(error); });

  EmojiManager::get_instance().perform_setup();
}
void LoadingWindow::show_fatal_error(const std::string &message) {

  auto dialog = new Gtk::MessageDialog(*this, "Initialization Failed", false,
                                       Gtk::MessageType::ERROR,
                                       Gtk::ButtonsType::OK, true);

  dialog->set_secondary_text(message + "\n\nThe application will now close.");

  dialog->signal_response().connect([this, dialog](int response_id) {
    delete dialog;

    exit(1);
  });

  dialog->show();
}
void LoadingWindow::update_status(const std::string &text) {
  std::string markup = "<span weight='bold'>" + text + "</span>";
  m_label.set_markup(markup);
}
