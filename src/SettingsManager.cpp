#include "SettingsManager.hpp"

SettingsManager::SettingsManager() {
  m_settings = Gio::Settings::create(SCHEMA_ID);
}

int SettingsManager::get_timeout_ms() const {
  return m_settings->get_int(KEY_TIMEOUT);
}
int SettingsManager::get_columns() const {
  return m_settings->get_int(KEY_COLUMNS);
}

int SettingsManager::get_skin_tone() const {
  return m_settings->get_enum(KEY_SKIN_TONE);
}

void SettingsManager::set_timeout_ms(int ms) {
  m_settings->set_int(KEY_TIMEOUT, ms);
}
void SettingsManager::set_skin_tone(int skintone) {
  m_settings->set_int(KEY_SKIN_TONE, skintone);
}

bool SettingsManager::get_paste_on_select() const {
  return m_settings->get_boolean(KEY_PASTE);
}

void SettingsManager::set_paste_on_select(bool value) {
  m_settings->set_boolean(KEY_PASTE, value);
}

bool SettingsManager::get_run_in_background() const {
  return m_settings->get_boolean(KEY_RUN_IN_BG);
}
bool SettingsManager::get_multi_emoji() const {
  return m_settings->get_boolean(KEY_MULTI_EMOJI);
}
void SettingsManager::set_run_in_background(bool value) {
  m_settings->set_boolean(KEY_RUN_IN_BG, value);
}

void SettingsManager::set_multi_emoji(bool value) {
  m_settings->set_boolean(KEY_MULTI_EMOJI, value);
}
