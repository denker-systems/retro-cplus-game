---
description: Avslutar sessionen och skapar/uppdaterar rapport - synkar med DEVLOG och CHANGELOG
---

# End Session Workflow

## 1. Hämta datum
// turbo
`powershell
$today = Get-Date -Format 'yyyy-MM-dd'; Write-Host "=== Avslutar Session $today ===" -ForegroundColor Cyan
`

## 2. Se dagens commits
// turbo
`powershell
$today = Get-Date -Format 'yyyy-MM-dd'
git log --oneline --after="$today 00:00" --before="$today 23:59" --format="%h %s"
`

## 3. Se git-status (ocommittade ändringar)
// turbo
`powershell
git status --short
`

## 4. Uppdatera Session Report

Uppdatera `docs/dev/sessions/YYYY-MM-DD.md` med:
- Sammanfattning av dagens arbete
- Alla commits med hashar
- Tekniska beslut
- Nästa steg

## 5. Uppdatera DEVLOG

Lägg till commits i `docs/dev/DEVLOG.md` under rätt datum.

## 6. Uppdatera CHANGELOG (om release-ready)

Om nya features är klara, lägg till under [Unreleased] i `docs/CHANGELOG.md`.

## 7. Uppdatera ROADMAP

Markera klara features som `[x]` i `docs/ROADMAP.md`.

## 8. Skapa ADR (om stort beslut)

Om ett stort arkitekturbeslut togs, skapa `docs/dev/decisions/NNN-beskrivning.md`.

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
