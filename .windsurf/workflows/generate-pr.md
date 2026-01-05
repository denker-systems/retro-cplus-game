---
description: Genererar Pull Request description från DEVLOG och session reports
---

# Generate PR Description Workflow

## 1. Kolla branch och commits
// turbo
`powershell
$branch = git branch --show-current
$commits = git rev-list --count main..HEAD 2>$null
if ($LASTEXITCODE -ne 0) { $commits = git rev-list --count HEAD }
Write-Host "Branch: $branch | Commits: $commits"
`

## 2. Hämta alla commits på branchen
// turbo
`powershell
git log main..HEAD --oneline --reverse 2>$null
if ($LASTEXITCODE -ne 0) { git log --oneline -20 }
`

## 3. Läs DEVLOG för detaljer
// turbo
`powershell
Get-Content docs/DEVLOG.md | Select-Object -First 100
`

## 4. Läs relevanta session reports
// turbo
`powershell
Get-ChildItem docs/sessions/*.md | Sort-Object Name -Descending | Select-Object -First 3 | ForEach-Object { Write-Host "=== $($_.Name) ===" -ForegroundColor Yellow; Get-Content $_.FullName | Select-Object -First 30 }
`

## 5. Generera PR Description

Skapa `PR_DESCRIPTION.md` med följande struktur:

`markdown
# [Branch Name] - Pull Request

##  Overview
Kort sammanfattning av vad denna PR innehåller.

**Branch:** `branch-name`  `main`
**Commits:** X
**Sessions:** YYYY-MM-DD, YYYY-MM-DD

---

##  Key Features

### [Kategori 1]
-  Feature 1
-  Feature 2

### [Kategori 2]
-  Feature 3

---

##  Changes

### New Files
`
path/to/new/files/
`

### Modified Files
- **file.cpp** - Beskrivning

---

##  Commit History

| Hash | Type | Description |
|------|------|-------------|
| `abc123` | feat | description |

---

##  Documentation
- Session reports: [dates]
- Updated: ROADMAP, ARCHITECTURE, README

---

##  Checklist
- [x] Code compiles
- [x] Tests pass
- [x] Documentation updated
`

## 6. Kopiera till clipboard
`powershell
Get-Content PR_DESCRIPTION.md | Set-Clipboard
Write-Host "PR description copied to clipboard!" -ForegroundColor Green
`

## 7. Skapa PR på GitHub

Öppna länken i webbläsare:
`
https://github.com/[owner]/[repo]/compare/main...[branch]
`

Klistra in beskrivningen (Ctrl+V) i PR-formuläret.

---

## Tips

- **Använd DEVLOG** för exakta commit-hashar och beskrivningar
- **Använd session reports** för kontext och tekniska beslut
- **Gruppera commits** efter feature/kategori för läsbarhet
- **Inkludera screenshots** om UI-ändringar gjorts
