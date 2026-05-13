import xml.etree.ElementTree as ET
import struct
import re

GROUPS = [
    "Smileys & Emotion", "People & Body", "Component",
    "Animals & Nature", "Food & Drink", "Travel & Places",
    "Activities", "Objects", "Symbols", "Flags"
]
GROUP_INDEX = {g: i for i, g in enumerate(GROUPS)}

SKIN_TONE_MODIFIERS = {
    '\U0001F3FB', '\U0001F3FC', '\U0001F3FD', '\U0001F3FE', '\U0001F3FF'
}

def is_skin_tone_variant(char):
    return any(c in SKIN_TONE_MODIFIERS for c in char)

def strip_variation(char):
    return char.replace('\uFE0F', '')

def strip_skin_tones(char):
    return ''.join(c for c in char if c not in SKIN_TONE_MODIFIERS)

def parse_cldr_keywords(xml_path):
    keywords_map = {}
    try:
        root = ET.parse(xml_path).getroot()
        for anno in root.findall(".//annotation"):
            if 'type' not in anno.attrib:
                keywords_map[anno.attrib['cp']] = anno.text.strip().replace(' | ', ', ')
    except FileNotFoundError:
        print(f"Warning: {xml_path} not found. Keywords will be empty.")
    return keywords_map

def bake_emoji_binary(txt_path, xml_path, output_path):
    keywords_map = parse_cldr_keywords(xml_path)
    line_re = re.compile(r'^\s*[^#]+;\s*fully-qualified\s*#\s*(\S+)\s+E\d+\.\d+\s+(.+)$')

    current_group = "Unknown"
    raw_lines = []
    with open(txt_path, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if line.startswith("# group:"):
                current_group = line.split(":", 1)[1].strip()
            else:
                m = line_re.search(line)
                if m:
                    raw_lines.append((m.group(1), m.group(2), current_group))

    bases_with_skin_tone = {
        strip_variation(strip_skin_tones(char))
        for char, _, _ in raw_lines
        if is_skin_tone_variant(char)
    }

    seen = set()
    emoji_entries = []
    for char, desc, group in raw_lines:
        if is_skin_tone_variant(char):
            continue
        key = strip_variation(char)
        if key in seen:
            continue
        seen.add(key)
        emoji_entries.append({
            'char': char,
            'desc': desc,
            'group': group,
            'keywords': keywords_map.get(key, keywords_map.get(char, "")),
            'skin_tone_support': key in bases_with_skin_tone,
        })

    with open(output_path, 'wb') as f:
        f.write(struct.pack('<I', len(emoji_entries)))
        for e in emoji_entries:
            c_bytes = e['char'].encode('utf-8')
            d_bytes = e['desc'].encode('utf-8')
            k_bytes = e['keywords'].encode('utf-8')
            f.write(struct.pack('<H', len(c_bytes))); f.write(c_bytes)
            f.write(struct.pack('<H', len(d_bytes))); f.write(d_bytes)
            f.write(struct.pack('<B', GROUP_INDEX.get(e['group'], 0xFF)))
            f.write(struct.pack('<H', len(k_bytes))); f.write(k_bytes)
            f.write(struct.pack('<B', int(e['skin_tone_support'])))

    print(f"Successfully baked {len(emoji_entries)} emojis into {output_path}")

if __name__ == "__main__":
    bake_emoji_binary('emoji-test.txt', 'en.xml', 'emoji_data.bin')