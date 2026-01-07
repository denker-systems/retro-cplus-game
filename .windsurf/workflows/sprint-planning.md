---
description: Sprint planning session workflow
---

# Sprint Planning Workflow

> Agile sprint planning for game development

## Pre-Planning

### 1. Review Backlog
```powershell
Write-Host "=== Sprint Planning ===" -ForegroundColor Cyan
Write-Host "Review backlog in JIRA/GitHub Projects"
```

### 2. Check Velocity
- Previous sprint velocity
- Team capacity this sprint
- Holidays/PTO

### 3. Review Dependencies
- External dependencies
- Blocked items
- Technical debt

---

## Planning Session

### 1. Sprint Goal
```markdown
## Sprint XX Goal

**Theme:** [Main focus area]

**Success Criteria:**
- [ ] Criterion 1
- [ ] Criterion 2
- [ ] Criterion 3
```

### 2. Story Selection

#### Priority Matrix
| Priority | Criteria |
|----------|----------|
| P0 | Critical, blocking |
| P1 | High value, needed soon |
| P2 | Medium value |
| P3 | Nice to have |

#### Story Points
| Points | Complexity | Time |
|--------|------------|------|
| 1 | Trivial | < 2h |
| 2 | Simple | 2-4h |
| 3 | Medium | 4-8h |
| 5 | Complex | 1-2d |
| 8 | Very complex | 2-3d |
| 13 | Epic | Split it |

### 3. Capacity Planning

```markdown
## Team Capacity

| Developer | Days | Capacity |
|-----------|------|----------|
| Dev 1 | 10 | 40 pts |
| Dev 2 | 8 | 32 pts |
| **Total** | - | **72 pts** |

## Committed Stories

| ID | Title | Points | Assignee |
|----|-------|--------|----------|
| #101 | Feature A | 5 | Dev 1 |
| #102 | Feature B | 3 | Dev 2 |

**Total Committed:** 65 pts (90% capacity)
```

---

## Sprint Backlog

### Create Sprint Document

`docs/sprints/sprint-XX.md`:

```markdown
# Sprint XX

**Duration:** YYYY-MM-DD to YYYY-MM-DD
**Goal:** [Sprint goal]

---

## Committed Stories

### Features
- [ ] #101 - Feature A (5 pts) @dev1
- [ ] #102 - Feature B (3 pts) @dev2

### Bugs
- [ ] #103 - Bug fix (2 pts) @dev1

### Tech Debt
- [ ] #104 - Refactor X (3 pts) @dev2

---

## Capacity

| Developer | Capacity | Committed | Available |
|-----------|----------|-----------|-----------|
| Dev 1 | 40 | 38 | 2 |
| Dev 2 | 32 | 30 | 2 |

---

## Risks

1. **Risk:** [description]
   **Mitigation:** [plan]

---

## Notes

- [Important decisions]
```

---

## Post-Planning

### 1. Update Issue Tracker
- Assign stories to sprint
- Set sprint start date
- Update assignees

### 2. Create Branches
```powershell
# For each feature
git checkout -b feature/XXX-story-title
```

### 3. Notify Team
```
📋 **Sprint XX Started**

**Goal:** [goal]
**Duration:** [dates]

**Committed:**
- #101 - Feature A
- #102 - Feature B
- #103 - Bug fix

**Sprint Board:** [link]
```

---

## Daily Standup Template

```markdown
## Standup YYYY-MM-DD

### [Developer Name]
**Yesterday:**
- Completed X
- Worked on Y

**Today:**
- Continue Y
- Start Z

**Blockers:**
- None / [description]
```

---

## Sprint Review Prep

### Demo Checklist
- [ ] Feature A demo ready
- [ ] Feature B demo ready
- [ ] Test environment prepared
- [ ] Stakeholders invited

---

## Metrics

| Metric | Target |
|--------|--------|
| Velocity | Consistent |
| Commitment | 90% |
| Spillover | < 10% |
| Bug Escape | < 5% |
