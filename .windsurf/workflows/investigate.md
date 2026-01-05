---
description: Systematisk felsökning av buggar i SDL2-spelet
---

# Investigate Workflow

## 1. Kolla kompileringsfel
`powershell
cd build
cmake --build . --config Release 2>&1
`

## 2. Kolla runtime-fel
Vanliga SDL2-problem:
- SDL_Init misslyckades - kolla SDL_GetError()
- Textur null - fil saknas eller fel sökväg
- Segfault - nullptr-access

## 3. Debug med cout
`cpp
#include <iostream>
std::cout << "Debug: variabel = " << variabel << std::endl;
`

## 4. Vanliga fixar
| Problem | Lösning |
|---------|---------|
| DLL saknas | Kopiera SDL2.dll till exe-mappen |
| Textur laddar inte | Kolla assets/-sökväg |
| Svart skärm | Kolla SDL_RenderPresent() |
| Ingen input | Kolla SDL_PollEvent() loop |
