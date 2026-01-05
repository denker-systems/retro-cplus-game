---
description: Avslutar sessionen och skapar rapport
---

# End Session Workflow

## 1. Hämta datum
// turbo
`powershell
$today = Get-Date -Format 'yyyy-MM-dd'; Write-Host "Datum: $today"
`

## 2. Se git-status
// turbo
`powershell
git status --short
`

## 3. Skapa session-rapport
Skapa docs/sessions/YYYY-MM-DD.md med:
- Sammanfattning
- Ändringar
- Problem lösta
- Nästa steg

## 4. Commit (valfritt)
`powershell
git add .
git commit -m "Session YYYY-MM-DD: [sammanfattning]"
`
