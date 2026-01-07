---
description: Start a new development session
---

# Init Session Workflow

> Starta arbetsdagen

## 1. Get Date
// turbo
```powershell
$today = Get-Date -Format 'yyyy-MM-dd'; Write-Host "=== Session $today ===" -ForegroundColor Cyan
```

## 2. Check Git Status
// turbo
```powershell
git status --short
```

## 3. Recent Commits
// turbo
```powershell
git log -5 --oneline --decorate
```

## 4. Latest Session Report
// turbo
```powershell
$latest = Get-ChildItem docs/dev/sessions/*.md -ErrorAction SilentlyContinue | Sort-Object Name -Descending | Select-Object -First 1
if ($latest) { Write-Host "Senaste: $($latest.Name)" -ForegroundColor Yellow; Get-Content $latest.FullName | Select-Object -First 30 }
```

## 5. Check ROADMAP
// turbo
```powershell
Get-Content docs/ROADMAP.md | Select-Object -First 30
```

---

## 6. Create Session Report (if needed)

Om `docs/dev/sessions/YYYY-MM-DD.md` inte finns:

```markdown
# Session YYYY-MM-DD

## Summary
[Fylls i under sessionen]

---

## Completed

### [Area]
- Task 1
- Task 2

---

## Git Commits

| Hash | Type | Description |
|------|------|-------------|

---

## Next Session
- [ ] Task 1
- [ ] Task 2
```

---

## 7. Summarize

Present:
- Senaste session
- Uncommitted changes
- ROADMAP progress
- Förslag på nästa steg
