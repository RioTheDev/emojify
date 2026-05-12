import xml.etree.ElementTree as ET
import struct
import re

GROUPS = [
    "Smileys & Emotion", "People & Body", "Component",
    "Animals & Nature", "Food & Drink", "Travel & Places",
    "Activities", "Objects", "Symbols", "Flags"
]
GROUP_INDEX = {g: i for i, g in enumerate(GROUPS)}
SKIN_TONES = {
    '\U0001F3FB': 1,  # light
    '\U0001F3FC': 2,  # medium-light
    '\U0001F3FD': 3,  # medium
    '\U0001F3FE': 4,  # medium-dark
    '\U0001F3FF': 5,  # dark
}

def get_skin_tone(char):
    for c in char:
        if c in SKIN_TONES:
            return SKIN_TONES[c]
    return 0  
def parse_cldr_keywords(xml_path):
    """Parses CLDR annotations to map emoji characters to keywords."""
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

def bake_emoji_binary(txt_path, xml_path, output_path):
    keywords_map = parse_cldr_keywords(xml_path)
    
    current_group = "Unknown"
    emoji_entries = []

    line_re = re.compile(r'^\s*[^#]+;\s*fully-qualified\s*#\s*(\S+)\s+E\d+\.\d+\s+(.+)$')

    with open(txt_path, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            
            if line.startswith("# group:"):
                current_group = line.split(":", 1)[1].strip()

            # Match fully-qualified emojis
            match = line_re.search(line)
            if match:
                char = match.group(1)
                desc = match.group(2)
                keywords = keywords_map.get(char, "")
                
                emoji_entries.append({
                    'char': char,
                    'desc': desc,
                    'group': current_group,
                    'keywords': keywords
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
            
            f.write(struct.pack('<B', GROUP_INDEX.get(e['group'], 0xFF)));
            
            f.write(struct.pack('<H', len(k_bytes)))
            f.write(k_bytes)
            f.write(struct.pack('<B', get_skin_tone(e['char'])))


    print(f"Successfully baked {len(emoji_entries)} emojis into {output_path}")

if __name__ == "__main__":
    bake_emoji_binary('emoji-test.txt', 'en.xml', 'emoji_data.bin')