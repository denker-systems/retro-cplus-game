"""
Generate all placeholder PNG assets.
Runs all generator scripts in the correct order.
"""

import os
import sys

# Add scripts directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "scripts"))

# Import all generators
from generate_actor_sprites import main as generate_actor_sprites
from generate_backgrounds import main as generate_backgrounds
from generate_tilesets import main as generate_tilesets

def main():
    """Generate all placeholder assets."""
    print("=" * 60)
    print("ASSET GENERATOR - Creating Placeholder PNG Files")
    print("=" * 60)
    print()
    
    try:
        # Generate actor sprites (NPCs, player, items)
        generate_actor_sprites()
        print()
        
        generate_backgrounds()
        print()
        
        generate_tilesets()
        print()
        
        print("=" * 60)
        print("✓ ALL ASSETS GENERATED SUCCESSFULLY!")
        print("=" * 60)
        print()
        print("Assets created in: ../assets/")
        print()
        print("Next steps:")
        print("1. Check the generated PNG files")
        print("2. Test loading them in your game")
        print("3. Replace with real assets from:")
        print("   - Kenney.nl: https://kenney.nl/assets")
        print("   - OpenGameArt.org: https://opengameart.org/")
        print("   - itch.io: https://itch.io/game-assets/free")
        print()
        
    except Exception as e:
        print(f"❌ ERROR: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
