#include <EmojiManager.hpp>
#include <SettingsManager.hpp>
#include <cstring>
#include <fstream>
#include <giomm.h>
EmojiManager::EmojiManager() {}
static std::filesystem::path get_cache_path() {
  return "/xyz/riothedev/emojify/data/emoji/emoji_data.bin";
}
static std::filesystem::path get_recents_path() {
  const char *xdg = std::getenv("XDG_DATA_HOME");
  if (xdg)
    return std::filesystem::path(xdg) / "emojify" / "recents.txt";

  const char *home = std::getenv("HOME");
  if (home)
    return std::filesystem::path(home) / ".local/share/emojify/recents.txt";

  g_warning("Neither XDG_DATA_HOME nor HOME is set");
  return "";
}

bool EmojiManager::load_binary() {
  try {
    auto bytes = Gio::Resource::lookup_data_global(get_cache_path());

    gsize total_size = 0;
    const uint8_t *data_ptr =
        static_cast<const uint8_t *>(bytes->get_data(total_size));

    if (!data_ptr || total_size == 0) {
      return false;
    }

    const uint8_t *cursor = data_ptr;
    const uint8_t *end = data_ptr + total_size;

    auto read_raw = [&](void *dest, size_t size) {
      if (cursor + size > end)
        return false;
      std::memcpy(dest, cursor, size);
      cursor += size;
      return true;
    };

    uint32_t numElements = 0;

    if (!read_raw(&numElements, sizeof(numElements)))
      return false;
    auto readString = [&](auto &str) {
      uint16_t len = 0;
      if (!read_raw(&len, sizeof(len)))
        return false;

      if (len > 0) {
        if (cursor + len > end)
          return false;
        str.assign(reinterpret_cast<const char *>(cursor), len);
        cursor += len;
      }
      return true;
    };

    emoji_db.reserve(numElements);
    for (uint32_t i = 0; i < numElements; ++i) {
      EmojiEntry emoji;

      if (!readString(emoji.character)) {
        g_print("FAIL at character, entry %d\n", i);
        break;
      }
      if (!readString(emoji.description)) {
        g_print("FAIL at description, entry %d\n", i);
        break;
      }
      uint8_t g = 0;
      if (!read_raw(&g, 1)) {
        g_print("FAIL at group, entry %d\n", i);
        break;
      }
      emoji.group = static_cast<EmojiGroup>(g);

      if (!readString(emoji.keywords)) {
        g_print("FAIL at keywords, entry %d\n", i);
        break;
      }
      uint8_t variant_count = 0;
      if (!read_raw(&variant_count, 1))
        break;

      for (uint8_t v = 0; v < variant_count; v++) {
        uint8_t tone_index = 0;
        if (!read_raw(&tone_index, 1))
          break;
        std::string seq;
        if (!readString(seq))
          break;
        emoji.variants.push_back({tone_index, seq});
      }

      emoji_db.push_back(std::move(emoji));
    }

    return true;

  } catch (const Glib::Error &ex) {
    g_warning("Resource Load Error: %s", ex.what());
    return false;
  }
}
void EmojiManager::save_as_recent(EmojiEntry emoji) {
  for (size_t i = 0; i < recents.size(); i++) {
    if (recents[i].character == emoji.character) {
      recents.erase(recents.begin() + i);
      break;
    }
  }
  if (recents.size() > 40)
    recents.pop_back();
  recents.insert(recents.begin(), emoji);
}

void EmojiManager::save_recents() {
  auto recents_path = get_recents_path();
  std::filesystem::create_directories(recents_path.parent_path());
  std::ofstream f(recents_path, std::ios::out | std::ios::trunc);
  if (!f)
    return;
  for (auto &e : recents)
    f << e.character << "\n";
}
void EmojiManager::load_recents() {
  auto recents_path = get_recents_path();

  std::ifstream f(recents_path);
  if (!f)
    return;

  std::string line;
  while (std::getline(f, line) && recents.size() < 40) {
    if (line.empty())
      continue;
    auto it =
        std::find_if(emoji_db.begin(), emoji_db.end(),
                     [&](const EmojiEntry &e) { return e.character == line; });
    if (it != emoji_db.end())
      recents.push_back(*it);
  }
}

std::pair<uint32_t, uint32_t> EmojiManager::get_group_range(EmojiGroup group) {
  uint8_t g = (uint8_t)group;

  if (group_ranges[g].second == 0) { // uncomputed sentinel
    auto start = std::lower_bound(emoji_db.begin(), emoji_db.end(), g,
                                  [](const EmojiEntry &e, uint8_t val) {
                                    return static_cast<uint8_t>(e.group) < val;
                                  });
    auto end = std::upper_bound(emoji_db.begin(), emoji_db.end(), g,
                                [](uint8_t val, const EmojiEntry &e) {
                                  return val < static_cast<uint8_t>(e.group);
                                });
    group_ranges[g] = {(uint32_t)(start - emoji_db.begin()),
                       (uint32_t)(end - emoji_db.begin())};
  }

  return group_ranges[(uint32_t)group];
}
EmojiManager::EmojiEntry
EmojiManager::get_emoji_by_character(std::string character) {
  for (auto emoji : emoji_db) {
    if (emoji.character == character) {
      return emoji;
    }
  }
}
std::string EmojiManager::get_display_character(const EmojiEntry &emoji) {
  uint8_t preferred = SettingsManager::get_instance().get_skin_tone();
  if (preferred == 0 || emoji.variants.empty())
    return emoji.character;
  for (auto &[tone_index, seq] : emoji.variants)
    if (tone_index == preferred - 1)
      return seq;
  return emoji.character;
}

std::vector<EmojiManager::EmojiEntry>
EmojiManager::find_by_query(std::string query) {
  std::transform(query.begin(), query.end(), query.begin(), ::tolower);
  std::vector<WeightedEmoji> matched_emojis;

  std::vector<std::string> query_words;
  std::istringstream iss(query);
  std::string word;
  while (iss >> word)
    query_words.push_back(word);

  for (auto &e : emoji_db) {
    std::string desc = e.description;
    std::string kw = e.keywords;
    std::transform(desc.begin(), desc.end(), desc.begin(), ::tolower);
    std::transform(kw.begin(), kw.end(), kw.begin(), ::tolower);

    std::vector<std::string> kw_tokens;
    std::istringstream kw_stream(kw);
    std::string token;
    while (std::getline(kw_stream, token, '|')) {
      token.erase(0, token.find_first_not_of(" \t"));
      token.erase(token.find_last_not_of(" \t") + 1);
      if (!token.empty())
        kw_tokens.push_back(token);
    }

    int weight = 0;

    for (const auto &kw_tok : kw_tokens) {
      if (kw_tok == query) {
        weight += 100;
        break;
      }
    }

    if (desc == query)
      weight += 90;

    for (const auto &kw_tok : kw_tokens) {
      if (kw_tok.rfind(query, 0) == 0) {
        weight += 60;
        break;
      }
    }

    if (desc.rfind(query, 0) == 0)
      weight += 50;

    for (const auto &kw_tok : kw_tokens) {
      if (kw_tok.find(query) != std::string::npos) {
        weight += 30;
        break;
      }
    }

    if (desc.find(query) != std::string::npos)
      weight += 20;

    if (query_words.size() > 1) {
      int words_matched_in_desc = 0;
      int words_matched_in_kw = 0;

      for (const auto &qw : query_words) {
        if (desc.find(qw) != std::string::npos)
          words_matched_in_desc++;

        for (const auto &kw_tok : kw_tokens) {
          if (kw_tok.find(qw) != std::string::npos) {
            words_matched_in_kw++;
            break;
          }
        }
      }

      weight +=
          (words_matched_in_kw * 15 * words_matched_in_kw) / query_words.size();
      weight += (words_matched_in_desc * 10 * words_matched_in_desc) /
                query_words.size();
    }

    if (weight == 0)
      continue;

    matched_emojis.push_back({&e, weight});
  }

  std::stable_sort(matched_emojis.begin(), matched_emojis.end(),
                   [](const WeightedEmoji &a, const WeightedEmoji &b) {
                     return a.weight > b.weight;
                   });

  std::vector<EmojiEntry> final_emojis;
  final_emojis.reserve(matched_emojis.size());
  for (const auto &we : matched_emojis)
    final_emojis.push_back(*we.emoji);

  return final_emojis;
}