#include <EmojiManager.hpp>
#include <SettingsManager.hpp>
#include <adwaita.h>
#include <gtkmm/application.h>
#include <gtkmm/cssprovider.h>
#include <windows/EmojiWindow.hpp>

void on_app_activate(Glib::RefPtr<Gtk::Application> app) {
  static EmojiWindow *window = nullptr;
  if (!window) {
    EmojiManager::get_instance().load_binary();
    window = new EmojiWindow();
    app->add_window(*window);
  }
  window->reset_data();
  window->present();
}
void apply_styles() {
  auto css_provider = Gtk::CssProvider::create();

  css_provider->load_from_resource("/xyz/riothedev/emojify/data/css/style.css");

  Gtk::StyleContext::add_provider_for_display(
      Gdk::Display::get_default(), css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}
int main(int argc, char *argv[]) {
  adw_init();
  auto app = Gtk::Application::create("xyz.riothedev.emojify");
  apply_styles();
  app->signal_activate().connect([app]() { on_app_activate(app); });

  app->hold();

  SettingsManager::get_instance()
      .get_settings()
      ->signal_changed("run-in-background")
      .connect([app](const Glib::ustring &) {
        static bool is_holding_for_bg = false;
        bool should_run_bg =
            SettingsManager::get_instance().get_run_in_background();

        if (should_run_bg && !is_holding_for_bg) {
          app->hold();
          is_holding_for_bg = true;
        } else if (!should_run_bg && is_holding_for_bg) {
          app->release();
          is_holding_for_bg = false;
        }
      });

  return app->run(argc, argv);
}
