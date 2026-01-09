# Retro Engine Icons

## Required Icon Sizes

Place your exported PNG icons here with the following names:

- `retro_engine_256.png` - 256x256 (Vista+, high DPI)
- `retro_engine_128.png` - 128x128 (Large icons)
- `retro_engine_64.png` - 64x64 (Medium icons)
- `retro_engine_48.png` - 48x48 (Standard desktop)
- `retro_engine_32.png` - 32x32 (Taskbar)
- `retro_engine_24.png` - 24x24 (Small icons)
- `retro_engine_16.png` - 16x16 (Tiny icons)

## Creating .ico File

### Option 1: Online Tool
1. Go to https://convertio.co/png-ico/ or https://www.icoconverter.com/
2. Upload all PNG files
3. Select "Combine into one icon"
4. Download as `retro_engine.ico`

### Option 2: ImageMagick (Command Line)
```bash
magick convert retro_engine_256.png retro_engine_128.png retro_engine_64.png retro_engine_48.png retro_engine_32.png retro_engine_24.png retro_engine_16.png retro_engine.ico
```

### Option 3: GIMP
1. Open 256x256 PNG
2. Add other sizes as layers
3. Export as .ico
4. Check "Compressed (PNG)" for larger sizes

## Current Status

- [ ] Export PNG files from your design
- [ ] Place PNG files in this folder
- [ ] Create retro_engine.ico with all sizes
- [ ] Build project to embed icon in .exe

## Notes

- Use transparent background for all PNGs
- Keep the pixel art style consistent across all sizes
- Smaller icons (16x16, 24x24) may need manual pixel adjustments for clarity
