# ADR 006: Ta bort Hot Reload från Spelet

**Datum:** 2026-01-05  
**Status:** Accepterad  
**Beslutsfattare:** Utvecklingsteam

## Kontext

Vi implementerade hot reload-funktionalitet med efsw för att automatiskt ladda om JSON-assets när de ändras i editorn. Detta visade sig vara problematiskt:

1. **Komplexitet:** Threading-problem, timing-issues, krasch vid re-enter av PlayState
2. **Instabilitet:** Spelet kraschade vid hot reload utan tydliga felmeddelanden
3. **Ovanligt mönster:** Polling i gameloop är inte standard i game engines
4. **Onödig overhead:** För ett litet projekt är restart-workflow tillräckligt

## Beslut

**Ta bort hot reload helt från spelet.** Använd istället:
- **Editor:** Spara ändringar (Ctrl+S) → skapar backup (.bak)
- **Spel:** Starta om för att se ändringar

## Konsekvenser

### Positiva
- ✅ **Enklare arkitektur** - ingen FileWatcher, inga callbacks, ingen threading
- ✅ **Mer stabilt** - inga krascher vid asset-ändringar
- ✅ **Standardiserat** - restart-workflow är beprövat i många engines
- ✅ **Lättare att debugga** - färre rörliga delar

### Negativa
- ❌ **Långsammare iteration** - måste starta om spelet manuellt
- ❌ **Mindre "modern"** - hot reload är nice-to-have

### Mitigering
- Editor har automatisk backup vid sparning
- Spelet startar snabbt (< 1 sekund)
- Kan implementera hot reload senare om verkligen behövs

## Implementation

### Borttaget
- `FileWatcher::update()` från gameloop
- `PlayState::setupHotReload()`
- efsw dependency från CMakeLists.txt
- Hot reload callback-logik

### Behållet
- `FileWatcher` klass (kan användas senare)
- Editor-sparning med backup
- JSON-laddning vid start

## Alternativ som övervägdes

1. **Fixa polling-lösningen** - För komplex, inte värt det
2. **Manuell reload-knapp (F5)** - Övervägs för framtiden
3. **Editor-only reload** - Inte relevant för vårt workflow

## Relaterade beslut

- [ADR 005: ImGui Editor Architecture](005-imgui-editor.md)
- [ADR 004: Separate Game and Editor Builds](004-separate-builds.md)
