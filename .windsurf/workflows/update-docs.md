---
description: Uppdaterar projektdokumentation (README, ARCHITECTURE, ROADMAP)
---

# Update Docs Workflow

## 1. Analysera ALLA ändringar sedan senaste dokumenterade commit
// turbo
`powershell
git diff --stat HEAD~1
`

**KRITISKT:** Identifiera ALLA typer av ändringar:
- Nya filer/mappar?
- Flyttade filer (arkitekturändringar)?
- Nya features/system?
- Build-system ändringar?
- Bugfixes?

## 2. Uppdatera README.md
- Ny feature? Lägg till i Features
- Nya dependencies? Uppdatera Installation
- Nya kommandon? Uppdatera Usage

## 3. Uppdatera ARCHITECTURE.md
När nya system/klasser läggs till:
- Lägg till i modulstruktur
- Uppdatera klassdiagram om relevant
- Dokumentera nya design patterns

## 4. Uppdatera ROADMAP.md
Efter varje implementation:
- Markera feature som klar: `[x]`
- Uppdatera progress-procent
- Lägg till nya upptäckta tasks

## 5. Commit docs separat
`powershell
git add docs/
git commit -m "docs: update documentation for [feature]"
`

## Checklista per session
- [ ] README uppdaterad med nya features
- [ ] ARCHITECTURE uppdaterad med nya klasser
- [ ] ROADMAP progress uppdaterad
- [ ] Alla nya filer har fil-header kommentarer
