---
trigger: always_on
---

# Git & Version Control Policy

## KRITISK REGEL: Git-operationer

**ALDRIG göra git-operationer utan explicit instruktion från användaren!**

### Förbjudna operationer (utan tillåtelse)
-  `git commit` - FÖRBJUDET självmant
-  `git push` - FÖRBJUDET självmant  
-  `git add` - FÖRBJUDET självmant
-  GitHub ändringar - FÖRBJUDET utan instruktion
-  Pull Requests - FÖRBJUDET utan instruktion

### Tillåtna operationer (alltid OK)
-  `git status` - Läsa status
-  `git log` - Läsa historik
-  `git diff` - Se ändringar
-  `git branch` - Se branches

---

## Synkat Dokumentationssystem

### Flöde
`
Session Start  Session Report  Work + Commits  DEVLOG  Session End  PR
`

### Tre Synkade Dokument

| Dokument | Syfte | Uppdateras |
|----------|-------|------------|
| `docs/dev/sessions/YYYY-MM-DD.md` | Daglig session-rapport | Under session |
| `docs/dev/DEVLOG.md` | Kronologisk commit-logg | Vid varje commit |
| `docs/CHANGELOG.md` | Versionerade releases | Vid release |
| `docs/ROADMAP.md` | Progress-tracking | Vid feature-completion |

### Commit-format med Session-referens

`powershell
git commit -m "type(scope): beskrivning" -m "- Detalj 1" -m "Session: YYYY-MM-DD"
`

### DEVLOG Entry-format

`markdown
- `hash` type(scope): beskrivning
  - Detalj 1
  - Session: [YYYY-MM-DD](docs/dev/sessions/YYYY-MM-DD.md)
`

---

## Workflows

| Workflow | Trigger | Syfte |
|----------|---------|-------|
| `/init-session` | Session start | Läs status, skapa rapport |
| `/git-commit` | Explicit instruktion | Commit + DEVLOG + session |
| `/end-session` | Session slut | Uppdatera rapport, synka |
| `/generate-pr` | Innan PR | Generera PR description |

---

## När Git-operationer är Tillåtna

Git-operationer får ENDAST köras när:
1. Användaren kallar på `/git-commit`
2. Användaren säger "commit", "push", "git add"
3. Användaren ger explicit instruktion

---

## Checklista vid Commit

- [ ] Commit-hash noterad
- [ ] DEVLOG.md uppdaterad
- [ ] Session report uppdaterad
- [ ] ROADMAP uppdaterad (om feature klar)

---

## Sammanfattning

**Gör ALDRIG git commit/push/add i samband med annat arbete!**
**Håll DEVLOG, session reports och commits synkade!**
