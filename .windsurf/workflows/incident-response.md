---
description: Production incident response workflow
---

# Incident Response Workflow

> Enterprise incident management

## Severity Levels

| Level | Name | Response | Example |
|-------|------|----------|---------|
| SEV1 | Critical | 15 min | Complete outage |
| SEV2 | High | 1 hour | Major feature broken |
| SEV3 | Medium | 4 hours | Minor feature broken |
| SEV4 | Low | 24 hours | Cosmetic issue |

---

## 1. Detection & Alert

### Acknowledge
```powershell
$incident = "INC-$(Get-Date -Format 'yyyyMMdd-HHmm')"
Write-Host "=== INCIDENT $incident ===" -ForegroundColor Red
Write-Host "Time: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
```

### Initial Assessment
- What is broken?
- Who is affected?
- When did it start?
- What changed recently?

---

## 2. Triage

### Severity Assessment
```markdown
## Incident: [ID]

**Detected:** YYYY-MM-DD HH:MM
**Severity:** SEV[X]

### Impact
- Users affected: [number/percentage]
- Features affected: [list]
- Revenue impact: [estimate]

### Initial Assessment
[Brief description]
```

### Escalation
| Severity | Notify |
|----------|--------|
| SEV1 | On-call, Lead, Management |
| SEV2 | On-call, Lead |
| SEV3 | On-call |
| SEV4 | Queue for next day |

---

## 3. Communication

### Status Page
```markdown
**[INVESTIGATING]** [Service Name]

We are investigating reports of [issue].

**Started:** HH:MM UTC
**Impact:** [description]
**Updates:** Every 30 minutes
```

### Internal
```
🚨 **INCIDENT: SEV[X]**
**Issue:** [description]
**Impact:** [who/what affected]
**Status:** Investigating
**Lead:** @[name]
**Channel:** #incident-[id]
```

---

## 4. Investigation

### Gather Information
```powershell
# Check logs
Get-Content logs/error.log -Tail 500 | Select-String "ERROR"

# Check recent deploys
git log --oneline --since="2 hours ago"

# Check metrics
# [monitoring dashboard]
```

### Timeline
```markdown
## Timeline

| Time | Event |
|------|-------|
| HH:MM | First user report |
| HH:MM | Alert triggered |
| HH:MM | Investigation started |
| HH:MM | Root cause identified |
```

---

## 5. Mitigation

### Options
1. **Rollback** - Revert to last known good state
2. **Hotfix** - Quick targeted fix
3. **Feature flag** - Disable broken feature
4. **Scaling** - Add capacity
5. **Manual intervention** - Temporary workaround

### Execute
```powershell
# Example: Rollback
git checkout v1.2.3
cmake --build . --config Release
# Deploy
```

---

## 6. Resolution

### Verify Fix
- [ ] Issue no longer reproducing
- [ ] Metrics returning to normal
- [ ] No new errors
- [ ] Users confirming resolution

### Update Status
```markdown
**[RESOLVED]** [Service Name]

The issue has been resolved. [Brief description of fix].

**Duration:** X hours Y minutes
**Root Cause:** [brief]
```

---

## 7. Post-Incident

### Create Report

`docs/incidents/INC-YYYYMMDD.md`:

```markdown
# Incident Report: INC-YYYYMMDD

## Summary
**Duration:** X hours Y minutes
**Severity:** SEV[X]
**Impact:** [description]

## Timeline
| Time | Event |
|------|-------|
| ... | ... |

## Root Cause
[Detailed explanation]

## Resolution
[What was done to fix]

## Impact Assessment
- Users affected: X
- Revenue impact: $Y
- Data loss: None/[description]

## Lessons Learned
1. [Lesson 1]
2. [Lesson 2]

## Action Items
- [ ] [Action 1] - @owner - Due: YYYY-MM-DD
- [ ] [Action 2] - @owner - Due: YYYY-MM-DD
```

### Schedule Postmortem
- Within 48 hours for SEV1/SEV2
- Within 1 week for SEV3

---

## 8. Postmortem Meeting

### Agenda
1. Timeline review
2. Root cause analysis
3. What went well
4. What could improve
5. Action items

### Blameless Culture
- Focus on systems, not people
- Ask "how" not "who"
- Improve processes

---

## Incident Checklist

### During Incident
- [ ] Acknowledged
- [ ] Severity assigned
- [ ] Stakeholders notified
- [ ] Status page updated
- [ ] Investigation started
- [ ] Regular updates posted

### Resolution
- [ ] Fix implemented
- [ ] Fix verified
- [ ] Status page resolved
- [ ] Stakeholders notified

### Post-Incident
- [ ] Incident report created
- [ ] Postmortem scheduled
- [ ] Action items tracked
- [ ] Documentation updated
