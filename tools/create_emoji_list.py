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
    '\U0001F3FB',  # light
    '\U0001F3FC',  # medium-light
    '\U0001F3FD',  # medium
    '\U0001F3FE',  # medium-dark
    '\U0001F3FF',  # dark
}

def is_skin_tone_variant(char):
    return any(c in SKIN_TONE_MODIFIERS for c in char)


def parse_cldr_keywords(xml_path):
    keywords_map = {}
    try:
        tree = ET.parse(xml_path)
        root = tree.getroot()
        for anno in root.findall(".//annotation"):
            if 'type' not in anno.attrib:
                cp = anno.attrib['cp']
                tags = anno.text.strip().replace(' | ', ', ')
                keywords_map[cp] = tags
    except FileNotFoundError:
        print(f"Warning: {xml_path} not found. Keywords will be empty.")
    return keywords_map

def strip_skin_tones(char):
    """Returns the base form of an emoji by removing all skin tone modifiers."""
    return ''.join(c for c in char if c not in SKIN_TONE_MODIFIERS)

def bake_emoji_binary(txt_path, xml_path, output_path):
    keywords_map = parse_cldr_keywords(xml_path)

    current_group = "Unknown"
    emoji_entries = []
    bases_with_skin_tone = set()

    line_re = re.compile(r'^\s*[^#]+;\s*fully-qualified\s*#\s*(\S+)\s+E\d+\.\d+\s+(.+)$')

    raw_lines = []
    with open(txt_path, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if line.startswith("# group:"):
                current_group = line.split(":", 1)[1].strip()
                raw_lines.append(('group', current_group))
            else:
                match = line_re.search(line)
                if match:
                    raw_lines.append(('emoji', match.group(1), match.group(2), current_group))

    for entry in raw_lines:
        if entry[0] == 'emoji':
            char = entry[1]
            if is_skin_tone_variant(char):
                base = strip_skin_tones(char)
                bases_with_skin_tone.add(base.replace('\uFE0F', '')) 

    seen_chars = set()
    for entry in raw_lines:
        if entry[0] != 'emoji':
            continue
        char, desc, group = entry[1], entry[2], entry[3]

        if is_skin_tone_variant(char):
            continue

        if char in seen_chars:
            continue
        seen_chars.add(char)

        keywords = keywords_map.get(char, "")
        char = char.replace('\uFE0F', '') 

        skin_tone_support = char in bases_with_skin_tone
        emoji_entries.append({
            'char': char,
            'desc': desc,
            'group': group,
            'keywords': keywords,
            'skin_tone_support': skin_tone_support,
        })

    with open(output_path, 'wb') as f:
        f.write(struct.pack('<I', len(emoji_entries)))
        for e in emoji_entries:
            c_bytes = e['char'].encode('utf-8')
            d_bytes = e['desc'].encode('utf-8')
            k_bytes = e['keywords'].encode('utf-8')

            f.write(struct.pack('<H', len(c_bytes)))
            f.write(c_bytes)

            f.write(struct.pack('<H', len(d_bytes)))
            f.write(d_bytes)

            f.write(struct.pack('<B', GROUP_INDEX.get(e['group'], 0xFF)))

            f.write(struct.pack('<H', len(k_bytes)))
            f.write(k_bytes)

            f.write(struct.pack('<B', 1 if e['skin_tone_support'] else 0))

    print(f"Successfully baked {len(emoji_entries)} emojis into {output_path}")

if __name__ == "__main__":
    bake_emoji_binary('emoji-test.txt', 'en.xml', 'emoji_data.bin')