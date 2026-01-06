"""
Generate placeholder UI elements.
Creates: button.png, panel.png
"""

from PIL import Image, ImageDraw, ImageFont
import os

# Output directory
OUTPUT_DIR = os.path.join("..", "assets", "ui")

def create_rounded_rectangle(draw, xy, radius, fill, outline=None, width=1):
    """Draw a rounded rectangle."""
    x1, y1, x2, y2 = xy
    
    # Draw corners
    draw.ellipse([x1, y1, x1 + radius*2, y1 + radius*2], fill=fill, outline=outline, width=width)
    draw.ellipse([x2 - radius*2, y1, x2, y1 + radius*2], fill=fill, outline=outline, width=width)
    draw.ellipse([x1, y2 - radius*2, x1 + radius*2, y2], fill=fill, outline=outline, width=width)
    draw.ellipse([x2 - radius*2, y2 - radius*2, x2, y2], fill=fill, outline=outline, width=width)
    
    # Draw rectangles
    draw.rectangle([x1 + radius, y1, x2 - radius, y2], fill=fill, outline=outline, width=width)
    draw.rectangle([x1, y1 + radius, x2, y2 - radius], fill=fill, outline=outline, width=width)

def generate_button():
    """Generate button UI element."""
    width, height = 128, 32
    
    # Create image with transparency
    img = Image.new("RGBA", (width, height), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Draw rounded button
    create_rounded_rectangle(
        draw, 
        [2, 2, width-3, height-3], 
        radius=8,
        fill=(80, 120, 200, 255),
        outline=(255, 255, 255, 255),
        width=2
    )
    
    # Draw text
    try:
        font = ImageFont.truetype("arial.ttf", 14)
    except:
        font = ImageFont.load_default()
    
    text = "BUTTON"
    bbox = draw.textbbox((0, 0), text, font=font)
    text_width = bbox[2] - bbox[0]
    text_height = bbox[3] - bbox[1]
    x = (width - text_width) // 2
    y = (height - text_height) // 2
    
    draw.text((x, y), text, fill=(255, 255, 255, 255), font=font)
    
    # Save
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    output_path = os.path.join(OUTPUT_DIR, "button.png")
    img.save(output_path)
    print(f"✓ Created: {output_path}")

def generate_panel():
    """Generate panel UI element."""
    width, height = 200, 150
    
    # Create image with transparency
    img = Image.new("RGBA", (width, height), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Draw panel background
    create_rounded_rectangle(
        draw,
        [5, 5, width-6, height-6],
        radius=12,
        fill=(40, 40, 60, 220),
        outline=(100, 100, 120, 255),
        width=3
    )
    
    # Draw title bar
    create_rounded_rectangle(
        draw,
        [8, 8, width-9, 35],
        radius=8,
        fill=(60, 60, 100, 255)
    )
    
    # Draw text
    try:
        font_title = ImageFont.truetype("arial.ttf", 16)
        font_body = ImageFont.truetype("arial.ttf", 12)
    except:
        font_title = ImageFont.load_default()
        font_body = ImageFont.load_default()
    
    # Title
    draw.text((15, 12), "PANEL", fill=(255, 255, 255, 255), font=font_title)
    
    # Body text
    draw.text((15, 45), "Content area", fill=(200, 200, 200, 255), font=font_body)
    draw.text((15, 65), "Line 2", fill=(200, 200, 200, 255), font=font_body)
    draw.text((15, 85), "Line 3", fill=(200, 200, 200, 255), font=font_body)
    
    # Save
    output_path = os.path.join(OUTPUT_DIR, "panel.png")
    img.save(output_path)
    print(f"✓ Created: {output_path}")

def main():
    """Generate all UI elements."""
    print("Generating UI elements...")
    generate_button()
    generate_panel()
    print("✓ All UI elements generated!")

if __name__ == "__main__":
    main()
