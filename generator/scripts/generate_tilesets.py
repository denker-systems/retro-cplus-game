"""
Generate placeholder tileset for TileMap.
Creates: dungeon.png (8x3 grid = 24 tiles of 32x32)
"""

from PIL import Image, ImageDraw, ImageFont
import os

# Output directory
OUTPUT_DIR = os.path.join("..", "assets", "tilesets")

def create_tile(tile_id, color):
    """Create a single tile with ID and color."""
    size = 32
    img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Draw colored square with border
    draw.rectangle([1, 1, size-2, size-2], fill=color, outline=(50, 50, 50, 255), width=2)
    
    # Draw tile ID
    try:
        font = ImageFont.truetype("arial.ttf", 12)
    except:
        font = ImageFont.load_default()
    
    text = str(tile_id)
    bbox = draw.textbbox((0, 0), text, font=font)
    text_width = bbox[2] - bbox[0]
    text_height = bbox[3] - bbox[1]
    x = (size - text_width) // 2
    y = (size - text_height) // 2
    
    draw.text((x, y), text, fill=(255, 255, 255, 255), font=font)
    
    return img

def generate_dungeon_tileset():
    """Generate dungeon tileset (8x3 = 24 tiles)."""
    tile_size = 32
    cols = 8
    rows = 3
    
    # Create tileset image
    tileset = Image.new("RGBA", (tile_size * cols, tile_size * rows), (0, 0, 0, 0))
    
    # Tile colors (different types)
    tile_types = [
        # Row 1: Floor tiles
        (100, 100, 100, 255),  # 1: Stone floor
        (120, 90, 70, 255),    # 2: Dirt floor
        (80, 80, 120, 255),    # 3: Blue tile
        (120, 120, 80, 255),   # 4: Yellow tile
        (100, 100, 100, 255),  # 5: Stone variant
        (120, 90, 70, 255),    # 6: Dirt variant
        (80, 80, 120, 255),    # 7: Blue variant
        (120, 120, 80, 255),   # 8: Yellow variant
        
        # Row 2: Wall tiles
        (70, 70, 70, 255),     # 9: Dark stone
        (60, 50, 40, 255),     # 10: Dark brick
        (80, 60, 60, 255),     # 11: Red brick
        (60, 80, 60, 255),     # 12: Green brick
        (70, 70, 70, 255),     # 13: Stone variant
        (60, 50, 40, 255),     # 14: Brick variant
        (80, 60, 60, 255),     # 15: Red variant
        (60, 80, 60, 255),     # 16: Green variant
        
        # Row 3: Special tiles
        (150, 150, 0, 255),    # 17: Gold
        (200, 50, 50, 255),    # 18: Danger
        (50, 200, 50, 255),    # 19: Safe
        (50, 50, 200, 255),    # 20: Water
        (150, 100, 50, 255),   # 21: Wood
        (100, 50, 100, 255),   # 22: Magic
        (200, 200, 200, 255),  # 23: Ice
        (255, 140, 0, 255),    # 24: Lava
    ]
    
    # Generate tiles
    tile_id = 1
    for row in range(rows):
        for col in range(cols):
            color = tile_types[tile_id - 1]
            tile = create_tile(tile_id, color)
            tileset.paste(tile, (col * tile_size, row * tile_size))
            tile_id += 1
    
    # Save
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    output_path = os.path.join(OUTPUT_DIR, "dungeon.png")
    tileset.save(output_path)
    print(f"✓ Created: {output_path} (8x3 = 24 tiles)")

def main():
    """Generate all tilesets."""
    print("Generating tilesets...")
    generate_dungeon_tileset()
    print("✓ All tilesets generated!")

if __name__ == "__main__":
    main()
