"""
Generate placeholder sprite sheets for player animations.
Creates: idle.png, walk.png, jump.png
"""

from PIL import Image, ImageDraw, ImageFont
import os

# Output directory
OUTPUT_DIR = os.path.join("..", "assets", "sprites", "player")

def create_sprite_frame(width, height, color, text):
    """Create a single sprite frame with color and text."""
    img = Image.new("RGBA", (width, height), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Draw colored rectangle with border
    draw.rectangle([2, 2, width-3, height-3], fill=color, outline=(255, 255, 255, 255), width=2)
    
    # Draw text in center
    try:
        font = ImageFont.truetype("arial.ttf", 10)
    except:
        font = ImageFont.load_default()
    
    bbox = draw.textbbox((0, 0), text, font=font)
    text_width = bbox[2] - bbox[0]
    text_height = bbox[3] - bbox[1]
    x = (width - text_width) // 2
    y = (height - text_height) // 2
    
    draw.text((x, y), text, fill=(255, 255, 255, 255), font=font)
    
    return img

def generate_idle_sprite():
    """Generate idle animation sprite sheet (4 frames, 32x32 each)."""
    frame_width = 32
    frame_height = 32
    num_frames = 4
    
    # Create sprite sheet
    sheet = Image.new("RGBA", (frame_width * num_frames, frame_height), (0, 0, 0, 0))
    
    # Color variations for animation effect
    colors = [
        (100, 150, 255, 255),  # Light blue
        (80, 130, 235, 255),   # Medium blue
        (60, 110, 215, 255),   # Darker blue
        (80, 130, 235, 255),   # Medium blue
    ]
    
    for i in range(num_frames):
        frame = create_sprite_frame(frame_width, frame_height, colors[i], f"I{i}")
        sheet.paste(frame, (i * frame_width, 0))
    
    # Save
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    output_path = os.path.join(OUTPUT_DIR, "idle.png")
    sheet.save(output_path)
    print(f"✓ Created: {output_path}")

def generate_walk_sprite():
    """Generate walk animation sprite sheet (8 frames, 32x32 each)."""
    frame_width = 32
    frame_height = 32
    num_frames = 8
    
    # Create sprite sheet
    sheet = Image.new("RGBA", (frame_width * num_frames, frame_height), (0, 0, 0, 0))
    
    # Color variations for animation effect
    colors = [
        (100, 200, 100, 255),  # Green
        (90, 190, 90, 255),
        (80, 180, 80, 255),
        (70, 170, 70, 255),
        (80, 180, 80, 255),
        (90, 190, 90, 255),
        (100, 200, 100, 255),
        (90, 190, 90, 255),
    ]
    
    for i in range(num_frames):
        frame = create_sprite_frame(frame_width, frame_height, colors[i], f"W{i}")
        sheet.paste(frame, (i * frame_width, 0))
    
    # Save
    output_path = os.path.join(OUTPUT_DIR, "walk.png")
    sheet.save(output_path)
    print(f"✓ Created: {output_path}")

def generate_jump_sprite():
    """Generate jump animation sprite sheet (2 frames, 32x32 each)."""
    frame_width = 32
    frame_height = 32
    num_frames = 2
    
    # Create sprite sheet
    sheet = Image.new("RGBA", (frame_width * num_frames, frame_height), (0, 0, 0, 0))
    
    # Color variations
    colors = [
        (255, 150, 50, 255),   # Orange (jump up)
        (255, 100, 0, 255),    # Dark orange (jump down)
    ]
    
    labels = ["↑", "↓"]
    
    for i in range(num_frames):
        frame = create_sprite_frame(frame_width, frame_height, colors[i], labels[i])
        sheet.paste(frame, (i * frame_width, 0))
    
    # Save
    output_path = os.path.join(OUTPUT_DIR, "jump.png")
    sheet.save(output_path)
    print(f"✓ Created: {output_path}")

def main():
    """Generate all player sprite sheets."""
    print("Generating player sprite sheets...")
    generate_idle_sprite()
    generate_walk_sprite()
    generate_jump_sprite()
    print("✓ All player sprites generated!")

if __name__ == "__main__":
    main()
