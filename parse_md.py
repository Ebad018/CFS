import re

def parse_beasts():
    with open('Master Bestiary List.md', 'r', encoding='utf-8') as f:
        lines = f.readlines()

    beasts = []
    
    tier_map = {
        'God': 10,
        'Saint': 9,
        'Heaven': 7,
        'Earth': 5,
        'Human': 3
    }
    
    current_tier = 0
    in_table = False
    headers = []
    
    for line in lines:
        line = line.strip()
        if not line: continue
        
        if line.startswith('**Tier:'):
            tier_str = line.split('Tier:')[1].strip().replace('**', '').split('/')[0].strip()
            current_tier = tier_map.get(tier_str, 5)
            
        if line.startswith('| ID'):
            headers = [h.strip() for h in line.split('|')[1:-1]]
            in_table = True
            continue
            
        if line.startswith('| ---') or line.startswith('|:---') or line.startswith('| -----'):
            continue
            
        if in_table and line.startswith('|'):
            cols = [c.strip().replace('**', '').replace('\\', '') for c in line.split('|')[1:-1]]
            if not cols or not cols[0].strip():
                in_table = False
                continue
                
            if len(cols) >= 4:
                beast_id = cols[0]
                name = cols[1]
                element = cols[2].split('/')[0] if '/' in cols[2] else cols[2]
                biome = cols[3].split('(')[0].strip()
                
                # Try to parse aggressive/behavior
                aggressive = 'false'
                for c in cols:
                    if 'Aggressive' in c or 'Territorial' in c or 'Hunter' in c or 'Ambusher' in c or 'Charge' in c or 'Destroys' in c:
                        aggressive = 'true'
                        
                symbol = name[0].upper()
                
                # Guess a color based on element
                color = '0xFFFFFFFF'
                ele_lower = element.lower()
                if 'fire' in ele_lower: color = '0xFF4500FF'
                elif 'water' in ele_lower or 'ice' in ele_lower: color = '0x00BFFFFF'
                elif 'wood' in ele_lower or 'poison' in ele_lower: color = '0x32CD32FF'
                elif 'earth' in ele_lower: color = '0x8B4513FF'
                elif 'metal' in ele_lower: color = '0xC0C0C0FF'
                elif 'thunder' in ele_lower or 'lightning' in ele_lower: color = '0x9370DBFF'
                elif 'wind' in ele_lower: color = '0xF5F5DCFF'
                elif 'dark' in ele_lower or 'death' in ele_lower or 'blood' in ele_lower: color = '0x8B0000FF'
                
                beasts.append(f'            {{"{beast_id}", "{name}", "{element}", "{biome}", {current_tier}, {aggressive}, \'{symbol}\', {color}}},')
    
    # Write output
    with open('beasts_cpp.txt', 'w', encoding='utf-8') as f:
        for b in beasts:
            f.write(b + '\n')
            
if __name__ == '__main__':
    parse_beasts()
