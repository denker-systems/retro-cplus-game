"""
Generate placeholder background layers for parallax scrolling.
Creates: sky.png, mountains.png, trees.png
"""

from PIL import Image, ImageDraw, ImageFont
import os

# Output directory
OUTPUT_DIR = os.path.join("..", "assets", "backgrounds")

def create_gradient(width, height, color1, color2):
    """Create a vertical gradient from color1 to color2."""
    img = Image.new("RGB", (width, height))
    draw = ImageDraw.Draw(img)
    
    for y in range(height):
        # Interpolate between colors
        ratio = y / height
        r = int(color1[0] * (1 - ratio) + color2[0] * ratio)
        g = int(color1[1] * (1 - ratio) + color2[1] * ratio)
        b = int(color1[2] * (1 - ratio) + color2[2] * ratio)
        
        draw.line([(0, y), (width, y)], fill=(r, g, b))
    
    return img

def generate_sky():
    """Generate sky gradient background."""
    width, height = 640, 480
    
    # Sky gradient (light blue to darker blue)
    img = create_gradient(width, height, (135, 206, 250), (70, 130, 180))
    
    # Add text label
    draw = ImageDraw.Draw(img)
    try:
        font = ImageFont.truetype("arial.ttf", 24)
    except:
        font = ImageFont.load_default()
    
    text = "SKY LAYER (z:-300)"
    bbox = draw.textbbox((0, 0), text, font=font)
    text_width = bbox[2] - bbox[0]
    x = (width - text_width) // 2
    
    draw.text((x, 50), text, fill=(255, 255, 255, 200), font=font)
    
    # Save
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    output_path = os.path.join(OUTPUT_DIR, "sky.png")
    img.save(output_path)
    print(f"✓ Created: {output_path}")

def generate_mountains():
    """Generate mountains silhouette with transparency."""
    width, height = 640, 480
    
    # Create transparent image
    img = Image.new("RGBA", (width, height), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Draw mountain silhouettes
    mountains = [
        # Mountain 1
        [(0, height), (100, 200), (250, height)],
        # Mountain 2
        [(200, height), (320, 150), (450, height)],
        # Mountain 3
        [(400, height), (520, 220), (640, height)],
    ]
    
    for mountain in mountains:
        draw.polygon(mountain, fill=(80, 80, 120, 200))
    
    # Add text label
    try:
        font = ImageFont.truetype("arial.ttf", 20)
    except:
        font = ImageFont.load_default()
    
    text = "MOUNTAINS (z:-200)"
    draw.text((width // 2 - 100, 100), text, fill=(255, 255, 255, 255), font=font)
    
    # Save
    output_path = os.path.join(OUTPUT_DIR, "mountains.png")
    img.save(output_path)
    print(f"✓ Created: {output_path}")

def generate_trees():
    """Generate trees foreground with transparency."""
    width, height = 640, 480
    
    # Create transparent image
    img = Image.new("RGBA", (width, height), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Draw simple tree shapes
    tree_positions = [50, 150, 250, 350, 450, 550]
    
    for x in tree_positions:
        # Tree trunk
        draw.rectangle([x - 5, height - 100, x + 5, height], fill=(101, 67, 33, 255))
        
        # Tree foliage (triangle)
        draw.polygon([
            (x, height - 150),
            (x - 30, height - 100),
            (x + 30, height - 100)
        ], fill=(34, 139, 34, 200))
    
    # Add text label
    try:
        font = ImageFont.truetype("arial.ttf", 20)
    except:
        font = ImageFont.load_default()
    
    text = "TREES (z:-100)"
    draw.text((width // 2 - 70, 50), text, fill=(255, 255, 255, 255), font=font)
    
    # Save
    output_path = os.path.join(OUTPUT_DIR, "trees.png")
    img.save(output_path)
    print(f"✓ Created: {output_path}")

def main():
    """Generate all background layers."""
    print("Generating background layers...")
    generate_sky()
    generate_mountains()
    generate_trees()
    print("✓ All backgrounds generated!")

if __name__ == "__main__":
    main()
