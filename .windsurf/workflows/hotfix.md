---
description: Emergency production fix workflow
---

# Hotfix Workflow

> Emergency fixes for production issues

## When to Use

- Production crash
- Security vulnerability
- Data corruption
- Critical functionality broken
- Revenue-impacting bugs

---

## 1. Incident Response

### Acknowledge
```powershell
Write-Host "[INCIDENT] Hotfix initiated at $(Get-Date -Format 'yyyy-MM-dd HH:mm')" -ForegroundColor Red
```

### Create Incident Ticket
- Priority: P0/Critical
- Assign: On-call developer
- Notify: Team lead, stakeholders

---

## 2. Create Hotfix Branch

```powershell
git fetch origin
git checkout main
git pull origin main
git checkout -b hotfix/XXX-brief-description
```

**Naming:** `hotfix/XXX-description` where XXX is incident number

---

## 3. Reproduce Issue

### Check Logs
```powershell
Get-Content build/Release/assets/logs/game.log -Tail 100
```

### Identify Root Cause
- Stack trace
- Error messages
- User reports
- Monitoring data

---

## 4. Implement Fix

### Guidelines
- **Minimal change** - Fix only the bug
- **No refactoring** - Not the time
- **No new features** - Focus on fix
- **Document reasoning** - Why this fix

### Code
```cpp
// HOTFIX-XXX: Brief description of fix
// Root cause: [explanation]
// This fixes: [what it fixes]
if (ptr != nullptr) {  // Added null check
    ptr->process();
}
```

---

## 5. Test Thoroughly

### Build
```powershell
cd build
cmake --build . --config Release
```

### Run Tests
```powershell
ctest -C Release --output-on-failure
```

### Manual Verification
- [ ] Bug no longer reproduces
- [ ] No regression in related features
- [ ] Performance not impacted

---

## 6. Create PR

### Fast-Track Review
```markdown
## 🚨 HOTFIX

**Incident:** #XXX
**Severity:** P0/Critical
**Impact:** [description]

### Root Cause
[explanation]

### Fix
[description of fix]

### Testing
- [x] Unit tests pass
- [x] Manual verification
- [x] No regression

### Rollback Plan
[how to rollback if needed]

**Reviewers:** @senior-dev (URGENT)
```

---

## 7. Review & Merge

### Expedited Review
- 1 senior approval required
- Skip normal queue
- 30-minute SLA

### Merge
```powershell
git checkout main
git pull origin main
git merge --no-ff hotfix/XXX-description
git tag -a v1.2.3-hotfix -m "Hotfix for incident XXX"
git push origin main --tags
```

---

## 8. Deploy

### Production Deploy
```powershell
# Follow deployment process
# Verify fix in production
# Monitor for issues
```

### Verification
- [ ] Fix deployed successfully
- [ ] Issue resolved in production
- [ ] No new issues introduced
- [ ] Monitoring normal

---

## 9. Post-Incident

### Update Incident
- Resolution time
- Root cause
- Fix applied
- Lessons learned

### Communicate
```
✅ **Incident Resolved**
**Issue:** [description]
**Impact:** [duration, affected users]
**Root Cause:** [brief]
**Fix:** [brief]
**Status:** Resolved
```

### Schedule Postmortem
- What happened?
- Why did it happen?
- How do we prevent it?
- Action items

---

## 10. Cleanup

### Delete Branch
```powershell
git branch -d hotfix/XXX-description
git push origin --delete hotfix/XXX-description
```

### Update Documentation
- Incident report
- Runbook (if needed)
- Monitoring alerts (if needed)

---

## Checklist

- [ ] Incident acknowledged
- [ ] Hotfix branch created
- [ ] Root cause identified
- [ ] Minimal fix implemented
- [ ] Tests pass
- [ ] PR created and reviewed
- [ ] Merged to main
- [ ] Deployed to production
- [ ] Verified in production
- [ ] Incident closed
- [ ] Postmortem scheduled
