import xml.etree.ElementTree as ET
import struct
import re

GROUPS = [
    "Smileys & Emotion", "People & Body", "Component",
    "Animals & Nature", "Food & Drink", "Travel & Places",
    "Activities", "Objects", "Symbols", "Flags"
]
GROUP_INDEX = {g: i for i, g in enumerate(GROUPS)}

SKIN_TONE_MODIFIERS = [
    '\U0001F3FB',  # 0 - light
    '\U0001F3FC',  # 1 - medium-light
    '\U0001F3FD',  # 2 - medium
    '\U0001F3FE',  # 3 - medium-dark
    '\U0001F3FF',  # 4 - dark
]
SKIN_TONE_SET = set(SKIN_TONE_MODIFIERS)

def get_base_key(char):
    """Strip all skin tone modifiers and VS16 to get canonical base key."""
    return ''.join(c for c in char if c not in SKIN_TONE_SET and c != '\uFE0F')

def get_modifier(char):
    """Return first skin tone modifier found, or None."""
    for c in char:
        if c in SKIN_TONE_SET:
            return c
    return None

def parse_cldr_keywords(xml_path):
    keywords_map = {}
    try:
        root = ET.parse(xml_path).getroot()
        for anno in root.findall(".//annotation"):
            if 'type' not in anno.attrib:
                cp = anno.attrib['cp']
                keywords_map[cp] = anno.text.strip().replace(' | ', ', ')
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

    variants: dict[str, dict[int, str]] = {} 
    for char, _, _ in raw_lines:
        mod = get_modifier(char)
        if mod is None:
            continue
        base = get_base_key(char)
        tone_index = SKIN_TONE_MODIFIERS.index(mod)
        variants.setdefault(base, {})[tone_index] = char

    seen = set()
    emoji_entries = []
    for char, desc, group in raw_lines:
        if get_modifier(char) is not None:
            continue
        base = get_base_key(char)
        if base in seen:
            continue
        seen.add(base)

        tone_variants = variants.get(base, {})
        emoji_entries.append({
            'char': char,
            'desc': desc,
            'group': group,
            'keywords': keywords_map.get(base, keywords_map.get(char, "")),
            'variants': tone_variants, 
        })

    # Binary format per emoji:
    #   2B+data  base sequence (utf-8)
    #   2B+data  description (utf-8)
    #   1B       group index
    #   2B+data  keywords (utf-8)
    #   1B       variant count N
    #   N x (1B tone_index + 2B+data sequence utf-8)
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

            ordered = sorted(e['variants'].items()) 
            f.write(struct.pack('<B', len(ordered)))
            for tone_index, seq in ordered:
                seq_bytes = seq.encode('utf-8')
                f.write(struct.pack('<B', tone_index))
                f.write(struct.pack('<H', len(seq_bytes))); f.write(seq_bytes)

    print(f"Baked {len(emoji_entries)} emojis → {output_path}")
    with_variants = sum(1 for e in emoji_entries if e['variants'])
    print(f"  {with_variants} with skin tone variants")

if __name__ == "__main__":
    bake_emoji_binary('emoji-test.txt', 'en.xml', 'emoji_data.bin')