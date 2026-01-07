---
description: Generate Pull Request description from commits and session reports
---

# Generate PR Workflow

> Skapa PR-beskrivning från commits och dokumentation

## 1. Check Branch
// turbo
```powershell
$branch = git branch --show-current
$commits = git rev-list --count main..HEAD 2>$null
if ($LASTEXITCODE -ne 0) { $commits = git rev-list --count HEAD }
Write-Host "Branch: $branch | Commits: $commits"
```

## 2. List Commits
// turbo
```powershell
git log main..HEAD --oneline --reverse 2>$null
if ($LASTEXITCODE -ne 0) { git log --oneline -10 }
```

## 3. Read DEVLOG
// turbo
```powershell
Get-Content docs/dev/DEVLOG.md | Select-Object -First 50
```

---

## 4. Generate PR Description

```markdown
# [Branch Name]

## Summary
Kort beskrivning av PRen.

**Branch:** `feature/xxx`  `main`
**Commits:** X

---

## Changes

### Added
- Feature 1
- Feature 2

### Changed
- Change 1

### Fixed
- Fix 1

---

## Commits

| Hash | Type | Description |
|------|------|-------------|
| `abc123` | feat | Description |

---

## Testing
- [ ] Builds successfully
- [ ] Manual testing done

## Documentation
- [ ] CHANGELOG updated
- [ ] ROADMAP updated
```

---

## 5. Copy to Clipboard
```powershell
Get-Content PR_DESCRIPTION.md | Set-Clipboard
Write-Host "Copied to clipboard!" -ForegroundColor Green
```

## 6. Create PR on GitHub
Öppna: `https://github.com/[owner]/[repo]/compare/main...[branch]`

Klistra in beskrivningen.
