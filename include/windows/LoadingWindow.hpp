#pragma once

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/window.h>

class LoadingWindow : public Gtk::Window {
public:
  LoadingWindow();
  virtual ~LoadingWindow() = default;

protected:
  Gtk::Box m_box;
  Gtk::Label m_label;
  Gtk::ProgressBar m_progress;
  sigc::signal<void()> m_signal_done;
  void update_status(const std::string &text);
  void show_fatal_error(const std::string &message);
};