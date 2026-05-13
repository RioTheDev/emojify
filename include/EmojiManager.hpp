#pragma once
#include <filesystem>
#include <glibmm.h>
#include <string>
#include <vector>

namespace fs = std::filesystem;
enum class EmojiGroup : uint8_t {
  SmileysEmotion = 0,
  PeopleBody,
  Component,
  AnimalsNature,
  FoodDrink,
  TravelPlaces,
  Activities,
  Objects,
  Symbols,
  Flags,
  Unknown = 0xFF
};
class EmojiManager {
public:
  static EmojiManager &get_instance() {
    static EmojiManager instance;
    return instance;
  }

  EmojiManager(const EmojiManager &) = delete;
  EmojiManager &operator=(const EmojiManager &) = delete;

  struct EmojiEntry {
    std::string character;
    std::string description;
    EmojiGroup group;
    std::string keywords;
    bool skin_tone_support;
  };
  struct ProgressValue {
    std::string stage;
    double fraction;
  };
  bool load_binary();

  const std::vector<EmojiEntry> &get_all_emoji() { return emoji_db; }
  EmojiEntry get_emoji_by_character(std::string character);
  static Glib::ustring get_emoji_with_skintone(EmojiEntry emoji);
  std::pair<uint32_t, uint32_t> get_group_range(EmojiGroup group);
  const std::vector<EmojiEntry> &get_recents() { return recents; }
  void save_as_recent(EmojiEntry emoji);
  void save_recents();
  void load_recents();

private:
  EmojiManager();
  std::array<std::pair<uint32_t, uint32_t>, 10> group_ranges;
  std::vector<EmojiEntry> emoji_db;
  std::vector<EmojiEntry> recents;
};