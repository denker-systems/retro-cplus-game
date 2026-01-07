---
description: Sprint retrospective workflow
---

# Retrospective Workflow

> Continuous improvement through reflection

## When to Run

- End of each sprint (2 weeks)
- After major milestones
- After incidents
- When team requests

---

## 1. Preparation

### Schedule
- Duration: 1-1.5 hours
- Attendees: Full team
- Facilitator: Rotates

### Gather Data
```powershell
# Sprint metrics
git log --oneline --since="2 weeks ago" | wc -l
git log --format="%an" --since="2 weeks ago" | sort | uniq -c
```

### Review
- Sprint goals: Achieved?
- Velocity: On target?
- Bugs: Escaped to production?
- Incidents: Any downtime?

---

## 2. Retrospective Format

### Start-Stop-Continue

| Category | Description |
|----------|-------------|
| **Start** | New things to try |
| **Stop** | Things not working |
| **Continue** | Things going well |

### 4Ls Format

| L | Question |
|---|----------|
| **Liked** | What went well? |
| **Learned** | What did we learn? |
| **Lacked** | What was missing? |
| **Longed for** | What do we wish we had? |

---

## 3. Discussion Topics

### Process
- [ ] Sprint planning effective?
- [ ] Daily standups useful?
- [ ] Code reviews timely?
- [ ] Documentation adequate?

### Technical
- [ ] Build times acceptable?
- [ ] Test coverage sufficient?
- [ ] Tech debt manageable?
- [ ] Tools adequate?

### Team
- [ ] Communication clear?
- [ ] Collaboration smooth?
- [ ] Workload balanced?
- [ ] Support available?

---

## 4. Document Outcomes

Create `docs/retrospectives/YYYY-MM-DD.md`:

```markdown
# Retrospective YYYY-MM-DD

## Sprint: XX
## Attendees: [names]

---

## What Went Well
- Item 1
- Item 2

## What Could Improve
- Item 1
- Item 2

## Action Items

| Action | Owner | Due |
|--------|-------|-----|
| Action 1 | @name | Date |
| Action 2 | @name | Date |

## Metrics

| Metric | Target | Actual |
|--------|--------|--------|
| Velocity | 60 pts | 55 pts |
| Bug escape | < 3 | 2 |
| Coverage | 80% | 78% |

---

## Notes
- Discussion notes
```

---

## 5. Action Items

### Create Issues
```markdown
**Type:** Process Improvement
**From:** Retro YYYY-MM-DD

### Problem
[Description of issue identified]

### Proposed Solution
[What we agreed to do]

### Success Criteria
- [ ] Criterion 1
- [ ] Criterion 2

### Owner
@assigned_person
```

### Track Progress
- Add to sprint backlog
- Review in next retro
- Celebrate improvements

---

## 6. Follow-Up

### Next Sprint Planning
- Review action items
- Adjust process
- Communicate changes

### Metrics Tracking
- Compare sprint-over-sprint
- Identify trends
- Celebrate wins

---

## Facilitation Tips

- ✓ Create safe space
- ✓ Everyone participates
- ✓ Focus on improvement
- ✓ Blameless culture
- ✓ Actionable outcomes
- ✓ Time-boxed discussions
