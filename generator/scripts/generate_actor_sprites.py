"""
Generate placeholder sprites for all game actors (NPCs, Player, Items).
Reads from JSON files and creates colored placeholder sprites.
"""

from PIL import Image, ImageDraw, ImageFont
import os
import json

# Paths
ASSETS_DIR = os.path.join("..", "assets")
DATA_DIR = os.path.join(ASSETS_DIR, "data")
SPRITES_DIR = os.path.join(ASSETS_DIR, "sprites")

# NPC sprite configurations (sprite_name -> color)
NPC_COLORS = {
    "bartender": (139, 69, 19, 255),      # Saddle brown
    "drunk": (255, 140, 0, 255),          # Dark orange
    "shopkeeper": (85, 107, 47, 255),     # Dark olive
    "stranger": (75, 0, 130, 255),        # Indigo
    "guard": (178, 34, 34, 255),          # Firebrick
    "librarian": (112, 128, 144, 255),    # Slate gray
    "hermit": (107, 142, 35, 255),        # Olive drab
    "cat": (255, 255, 255, 255),          # White
}

def create_sprite(width, height, color, text, outline_color=(0, 0, 0, 255)):
    """Create a single sprite with color, border and text."""
    img = Image.new("RGBA", (width, height), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Draw colored rectangle with black outline
    draw.rectangle([1, 1, width-2, height-2], fill=color, outline=outline_color, width=2)
    
    # Draw text in center
    try:
        font = ImageFont.truetype("arial.ttf", 12)
    except:
        font = ImageFont.load_default()
    
    bbox = draw.textbbox((0, 0), text, font=font)
    text_width = bbox[2] - bbox[0]
    text_height = bbox[3] - bbox[1]
    x = (width - text_width) // 2
    y = (height - text_height) // 2
    
    # Draw text with shadow for better visibility
    draw.text((x+1, y+1), text, fill=(0, 0, 0, 200), font=font)
    draw.text((x, y), text, fill=(255, 255, 255, 255), font=font)
    
    return img

def generate_npc_sprites():
    """Generate sprites for all NPCs from npcs.json."""
    npcs_file = os.path.join(DATA_DIR, "npcs.json")
    
    if not os.path.exists(npcs_file):
        print(f"⚠ Warning: {npcs_file} not found")
        return
    
    with open(npcs_file, 'r') as f:
        data = json.load(f)
    
    npcs = data.get("npcs", [])
    print(f"\n📦 Generating sprites for {len(npcs)} NPCs...")
    
    for npc in npcs:
        sprite_name = npc.get("sprite", "")
        npc_name = npc.get("name", "NPC")
        
        if not sprite_name:
            continue
        
        # Get color for this NPC (or use default)
        color = NPC_COLORS.get(sprite_name, (150, 150, 150, 255))
        
        # Create sprite (32x48 for characters)
        sprite = create_sprite(32, 48, color, npc_name[:3].upper())
        
        # Save
        output_path = os.path.join(SPRITES_DIR, f"{sprite_name}.png")
        sprite.save(output_path)
        print(f"  ✓ {sprite_name}.png - {npc_name}")

def generate_player_sprite():
    """Generate player sprite."""
    print("\n📦 Generating player sprite...")
    
    # Player sprite (32x48, bright blue/cyan)
    color = (0, 191, 255, 255)  # Deep sky blue
    sprite = create_sprite(32, 48, color, "P")
    
    output_path = os.path.join(SPRITES_DIR, "player.png")
    sprite.save(output_path)
    print(f"  ✓ player.png")

def generate_item_sprites():
    """Generate sprites for all items from items.json."""
    items_file = os.path.join(DATA_DIR, "items.json")
    
    if not os.path.exists(items_file):
        print(f"⚠ Warning: {items_file} not found")
        return
    
    with open(items_file, 'r') as f:
        data = json.load(f)
    
    items = data.get("items", [])
    print(f"\n📦 Generating sprites for {len(items)} items...")
    
    # Item color categories
    item_colors = {
        "key": (255, 215, 0, 255),        # Gold
        "coin": (255, 215, 0, 255),       # Gold
        "letter": (245, 245, 220, 255),   # Beige
        "note": (245, 245, 220, 255),     # Beige
        "lantern": (255, 140, 0, 255),    # Dark orange
        "matches": (160, 82, 45, 255),    # Sienna
        "rope": (210, 180, 140, 255),     # Tan
        "hook": (169, 169, 169, 255),     # Dark gray
        "mushroom": (128, 0, 128, 255),   # Purple
        "bottle": (173, 216, 230, 255),   # Light blue
        "potion": (138, 43, 226, 255),    # Blue violet
        "book": (139, 69, 19, 255),       # Saddle brown
        "crystal": (147, 112, 219, 255),  # Medium purple
        "map": (222, 184, 135, 255),      # Burlywood
        "gemstone": (220, 20, 60, 255),   # Crimson
    }
    
    for item in items:
        item_id = item.get("id", "")
        item_name = item.get("name", "Item")
        
        if not item_id:
            continue
        
        # Find matching color
        color = (200, 200, 200, 255)  # Default gray
        for key, col in item_colors.items():
            if key in item_id.lower():
                color = col
                break
        
        # Create small item sprite (24x24)
        sprite = create_sprite(24, 24, color, item_id[:2].upper())
        
        # Save
        output_path = os.path.join(SPRITES_DIR, f"{item_id}.png")
        sprite.save(output_path)
        print(f"  ✓ {item_id}.png - {item_name}")

def main():
    """Generate all actor sprites."""
    print("=" * 60)
    print("🎨 Retro Adventure - Actor Sprite Generator")
    print("=" * 60)
    
    # Create sprites directory if it doesn't exist
    os.makedirs(SPRITES_DIR, exist_ok=True)
    
    # Generate sprites
    generate_player_sprite()
    generate_npc_sprites()
    generate_item_sprites()
    
    print("\n" + "=" * 60)
    print("✅ All actor sprites generated successfully!")
    print("=" * 60)
    print(f"\n📁 Sprites location: {SPRITES_DIR}")
    print("\n💡 Tip: Replace these placeholders with real art later!")

if __name__ == "__main__":
    main()
