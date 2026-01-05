---
description: Startar en ny session genom att analysera projektets status
---

# Init Session Workflow

## 1. Hämta datum
// turbo
`powershell
$today = Get-Date -Format 'yyyy-MM-dd'; Write-Host "=== Session $today ===" -ForegroundColor Cyan
`

## 2. Kolla git-status
// turbo
`powershell
git status --short
`

## 3. Se senaste commits
// turbo
`powershell
git log -5 --oneline --decorate
`

## 4. Läs senaste session-rapport
// turbo
`powershell
$latest = Get-ChildItem docs/dev/sessions/*.md | Sort-Object Name -Descending | Select-Object -First 1
Write-Host "Senaste session: $($latest.Name)" -ForegroundColor Yellow
Get-Content $latest.FullName | Select-Object -First 50
`

## 5. Kolla CHANGELOG
// turbo
`powershell
Get-Content docs/CHANGELOG.md | Select-Object -First 40
`

## 6. Kolla ROADMAP progress
// turbo
`powershell
Get-Content docs/ROADMAP.md | Select-Object -First 20
`

## 7. Skapa session-rapport för idag

Om `docs/dev/sessions/YYYY-MM-DD.md` inte finns, skapa med mall:

`markdown
# Session YYYY-MM-DD

## Sammanfattning
[Fylls i under sessionen]

---

## Genomfört

### [Område 1]
- Feature/fix beskrivning

---

## Git Commits

| Hash | Typ | Beskrivning |
|------|-----|-------------|
| `abc123` | feat | beskrivning |

---

## Nästa Session
- [ ] Task 1
- [ ] Task 2
`

## 8. Sammanfatta status

Presentera:
- Vad gjordes senast (från session report)
- Vilka filer är ändrade (från git status)
- ROADMAP progress
- Förslag på nästa steg
