---
description: Avslutar sessionen och skapar/uppdaterar rapport - synkar med DEVLOG och CHANGELOG
---

# End Session Workflow

## 1. Hämta datum
// turbo
`powershell
$today = Get-Date -Format 'yyyy-MM-dd'; Write-Host "=== Avslutar Session $today ===" -ForegroundColor Cyan
`

## 2. Analysera dagens commits
// turbo
`powershell
$today = Get-Date -Format 'yyyy-MM-dd'
git log --oneline --after="$today 00:00" --before="$today 23:59" --format="%h %s"
`

**För varje commit, kör:**
`powershell
git show --stat <HASH>
`

**Identifiera för varje commit:**
- Arkitekturändringar? (flyttade filer, nya mappar)
- Build-system ändringar? (CMakeLists.txt, vcpkg)
- Nya features/system?
- Bugfixes?

## 3. Se git-status (ocommittade ändringar)
// turbo
`powershell
git status --short
`

## 4. Uppdatera Session Report

**KRITISKT:** Dokumentera ALLA commits och ändringar!

Uppdatera `docs/dev/sessions/YYYY-MM-DD.md` med:
- Sammanfattning av dagens arbete
- **Alla commits med hashar OCH detaljerad beskrivning**
- **Arkitekturändringar om relevanta**
- **Build-system ändringar om relevanta**
- Tekniska beslut
- Nästa steg

**Format för commits:**
```markdown
X. `HASH` type(scope): beskrivning
   - **Arkitektur:** Om filer flyttades/strukturändring
   - **Build:** Om build-system ändrades
   - Feature 1
   - Feature 2
```

## 5. Uppdatera DEVLOG

**KRITISKT:** Lägg till ALLA commits med ALLA ändringar!

Lägg till i `docs/dev/DEVLOG.md` under rätt datum/tid:
```markdown
- `HASH` type(scope): beskrivning
  - **Arkitektur:** Om relevant
  - **Build:** Om relevant
  - Feature 1
  - Feature 2
  - Session: [YYYY-MM-DD](sessions/YYYY-MM-DD.md)
```

## 6. Uppdatera CHANGELOG

**KRITISKT:** Lägg till ALLA nya features, ändringar och fixes!

Under [Unreleased] i `docs/CHANGELOG.md`:
- **Added:** Nya features, filer, system, arkitektur
- **Changed:** Strukturändringar, refactorings, build-system
- **Fixed:** Bugfixes

## 7. Uppdatera ROADMAP

Markera klara features som `[x]` i `docs/ROADMAP.md`.
Lägg till nya upptäckta tasks.

## 8. Skapa ADR (om arkitekturbeslut)

Om ett arkitekturbeslut togs, skapa `docs/adr/NNN-beskrivning.md`.

---

## Checklista

- [ ] Session report uppdaterad (docs/dev/sessions/)
- [ ] DEVLOG uppdaterad (docs/dev/DEVLOG.md)
- [ ] CHANGELOG uppdaterad (docs/CHANGELOG.md)
- [ ] ROADMAP progress uppdaterad (docs/ROADMAP.md)
- [ ] ADR skapad (om relevant)
- [ ] Ocommittade ändringar hanterade

---

##  PÅMINNELSE

**Gör INTE automatisk commit/push!**
Be användaren köra `/git-commit` om de vill committa.
