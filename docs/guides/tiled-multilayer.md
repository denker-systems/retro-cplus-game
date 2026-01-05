# Multi-Layer Rendering i Tiled

## Översikt

Spelet stödjer nu **multi-layer rendering** med djup, vilket ger parallax-effekter och objekt som kan vara framför eller bakom spelaren.

---

## Layer-typer

### zIndex (Djup)
- **Negativa värden** (t.ex. -2, -1): Bakom spelaren
- **0**: Samma nivå som spelaren
- **Positiva värden** (t.ex. 1, 2): Framför spelaren

### Exempel
```
zIndex: -2  → Bakgrund (himmel, berg)
zIndex: -1  → Mellangrund (träd, byggnader)
zIndex: 0   → Spelare
zIndex: 1   → Förgrund (grenar, lampor)
zIndex: 2   → Mycket nära förgrund (regn, dimma)
```

---

## Skapa Multi-Layer Rum i Tiled

### 1. Skapa Image Layers
- **Layer → New → Image Layer**
- Namnge varje layer beskrivande (t.ex. "background", "foreground", "trees")
- Lägg till bildfil för varje layer

### 2. Sätt Layer Properties

För varje Image Layer, sätt **Custom Properties**:

| Property | Typ | Beskrivning | Default |
|----------|-----|-------------|---------|
| `zIndex` | int | Djup (-2 till 2) | 0 |
| `parallaxX` | float | Parallax X (0.0-2.0) | 1.0 |
| `parallaxY` | float | Parallax Y (0.0-2.0) | 1.0 |

### 3. Exempel-struktur

```
Layers (uppifrån i Tiled):
├── foreground_close (Image Layer)
│   └── zIndex: 2, opacity: 0.8
├── foreground (Image Layer)
│   └── zIndex: 1
├── hotspots (Object Layer)
├── background (Image Layer)
│   └── zIndex: -1
└── sky (Image Layer)
    └── zIndex: -2, parallaxX: 0.5
```

---

## Parallax Scrolling

### parallaxX / parallaxY
- **< 1.0**: Layer rör sig långsammare (bakgrund)
- **= 1.0**: Normal hastighet (default)
- **> 1.0**: Layer rör sig snabbare (förgrund)

### Exempel
```
Sky layer:     parallaxX: 0.3  (rör sig långsamt)
Trees layer:   parallaxX: 0.7  (rör sig lite långsammare)
Ground layer:  parallaxX: 1.0  (normal)
Grass layer:   parallaxX: 1.2  (rör sig lite snabbare)
```

---

## Opacity (Genomskinlighet)

Sätt **Opacity** direkt i Tiled:
- **1.0** = Helt ogenomskinlig
- **0.5** = Halvgenomskinlig
- **0.0** = Helt genomskinlig

Användbart för:
- Dimma
- Regn
- Ljuseffekter
- Skuggor

---

## Rendering-ordning

Spelet renderar i denna ordning:
1. **Layers med zIndex < 0** (bakgrund)
2. **Spelare och NPCs**
3. **Layers med zIndex > 0** (förgrund)

---

## Exempel: Tavern med Djup

```
assets/backgrounds/
├── tavern_sky.png          (zIndex: -2)
├── tavern_back_wall.png    (zIndex: -1)
├── tavern_tables.png       (zIndex: 0)
├── tavern_chandelier.png   (zIndex: 1)
└── tavern_smoke.png        (zIndex: 2, opacity: 0.3)
```

I Tiled:
```
Layers:
├── smoke (Image Layer)
│   ├── image: tavern_smoke.png
│   ├── zIndex: 2
│   └── opacity: 0.3
├── chandelier (Image Layer)
│   ├── image: tavern_chandelier.png
│   └── zIndex: 1
├── hotspots (Object Layer)
├── tables (Image Layer)
│   ├── image: tavern_tables.png
│   └── zIndex: 0
├── back_wall (Image Layer)
│   ├── image: tavern_back_wall.png
│   └── zIndex: -1
└── sky (Image Layer)
    ├── image: tavern_sky.png
    └── zIndex: -2
```

---

## Tips

### Optimering
- Använd inte för många layers (max 5-6)
- Kombinera layers i Photoshop/GIMP när möjligt

### Konstnärliga tips
- Använd blur på bakgrundslayers för djup
- Lägg till ljuseffekter på förgrundslayers
- Använd opacity för atmosfär (dimma, rök)

### Debugging
- Sätt olika opacity-värden för att se layer-ordning
- Använd Editor → Rooms tab för att se layer-info

---

## Legacy-kompatibilitet

Gamla rum utan layers fungerar fortfarande:
- `background` field används som fallback
- Renderas som ett enda layer med zIndex: 0

För att konvertera gamla rum:
1. Exportera från Editor → Rooms → Export All
2. Öppna i Tiled
3. Lägg till nya Image Layers med zIndex
4. Importera tillbaka med Import Tiled
