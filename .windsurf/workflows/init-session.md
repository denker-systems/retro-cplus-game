---
description: Startar en ny session genom att analysera projektets status
---

# Init Session Workflow

## 1. Kolla git-status
// turbo
`powershell
git status --short
`

## 2. Se senaste commits
// turbo
`powershell
git log -5 --oneline
`

## 3. Läs senaste session-rapport
// turbo
`powershell
Get-ChildItem docs/sessions/*.md | Sort-Object LastWriteTime -Descending | Select-Object -First 1 | Get-Content
`

## 4. Sammanfatta status
Analysera och presentera:
- Vad gjordes senast
- Vilka filer är ändrade
- Vad är nästa steg
