---
description: Commit och push ändringar - synkar med DEVLOG, CHANGELOG och session reports
---

# Git Commit & Push Workflow

** VIKTIGT:** Denna workflow körs ENDAST när användaren explicit ber om det!

## 1. Kolla datum och branch
// turbo
`powershell
$today = Get-Date -Format 'yyyy-MM-dd'; $branch = git branch --show-current; Write-Host "Datum: $today | Branch: $branch"
`

## 2. Analysera ALLA ändringar sedan senaste commit
// turbo
`powershell
git status --short
`

**KRITISKT:** Kör `git diff --stat` för att se ALLA ändrade filer:
`powershell
git diff --stat HEAD
`

**Identifiera vad som ändrats:**
- Nya filer/mappar?
- Flyttade filer (renames)?
- Arkitekturändringar?
- Nya features?
- Bugfixes?

## 3. Stage alla ändringar
`powershell
git add -A
`

## 4. Commit med konventionellt format

**Format:** `type(scope): beskrivning`

**Commit med session-referens:**
`powershell
git commit -m "type(scope): beskrivning" -m "- Detalj 1" -m "- Detalj 2" -m "Session: YYYY-MM-DD"
`

## 5. Hämta commit-hash
// turbo
`powershell
$hash = git rev-parse --short HEAD; $msg = git log -1 --format="%s"; Write-Host "Commit: $hash - $msg"
`

## 6. Uppdatera dokumentation

**KRITISKT:** Dokumentera ALLA ändringar från steg 2, inte bara det senaste!

### DEVLOG (docs/dev/DEVLOG.md)
Lägg till under rätt dag/tid med ALLA ändringar:
```markdown
- `HASH` type(scope): beskrivning
  - **Arkitektur:** Om filer flyttades/strukturändring
  - **Build:** Om build-system ändrades
  - **Feature 1:** Kort beskrivning
  - **Feature 2:** Kort beskrivning
  - Session: [YYYY-MM-DD](sessions/YYYY-MM-DD.md)
```

### Session Report (docs/dev/sessions/YYYY-MM-DD.md)
Lägg till i "Git Commits" med ALLA ändringar:
```markdown
X. `HASH` type(scope): beskrivning
   - **Arkitektur:** Om relevant
   - **Build:** Om relevant
   - Feature 1
   - Feature 2
```

### CHANGELOG (docs/CHANGELOG.md)
Under [Unreleased], lägg till ALLA ändringar:
- **Added:** Nya features, filer, system
- **Changed:** Strukturändringar, refactorings
- **Fixed:** Bugfixes

### ROADMAP (docs/ROADMAP.md)
Markera features som [x] om klara.

### ADR (docs/adr/)
Skapa ny ADR om arkitekturbeslut togs.

## 7. Push till remote
`powershell
git push origin $(git branch --show-current)
`

---

## Checklista

- [ ] Commit-hash noterad
- [ ] docs/dev/DEVLOG.md uppdaterad
- [ ] docs/dev/sessions/YYYY-MM-DD.md uppdaterad
- [ ] docs/CHANGELOG.md uppdaterad (om ny feature)
- [ ] docs/ROADMAP.md uppdaterad (om feature klar)

---

##  PÅMINNELSE

Kör ENDAST denna workflow när användaren explicit ber om det!
**ALDRIG git-operationer i samband med annat arbete!**
