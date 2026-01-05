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

## 2. Kolla status
// turbo
`powershell
git status --short
`

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

### DEVLOG (docs/dev/DEVLOG.md)
`markdown
- `HASH` type(scope): beskrivning
  - Detalj 1
  - Session: [YYYY-MM-DD](sessions/YYYY-MM-DD.md)
`

### Session Report (docs/dev/sessions/YYYY-MM-DD.md)
Lägg till commit i "Git Commits" sektionen.

### CHANGELOG (docs/CHANGELOG.md)
Lägg till under [Unreleased] om ny feature.

### ROADMAP (docs/ROADMAP.md)
Markera feature som [x] om klar.

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
