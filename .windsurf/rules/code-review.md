---
trigger: always_on
description: Code review standards
---

# Code Review Standards

> Enterprise code review process

## Review Requirements

| Change Type | Approvals | Reviewers |
|-------------|-----------|-----------|
| Standard | 1 | Team member |
| Architecture | 2 | 1 senior |
| Critical | 2 | Core team |

---

## Reviewer Checklist

### Code Quality
- [ ] Follows coding standards
- [ ] Readable and understandable
- [ ] No code duplication
- [ ] Appropriate complexity

### Documentation
- [ ] File headers present
- [ ] Classes documented
- [ ] Methods documented
- [ ] Complex logic explained

### Architecture
- [ ] SOLID principles followed
- [ ] Correct abstraction level
- [ ] No circular dependencies

### Safety
- [ ] Memory management correct
- [ ] Thread safety considered
- [ ] Error handling complete

### Testing
- [ ] Sufficient test coverage
- [ ] Edge cases tested
- [ ] No regressions

---

## Comment Prefixes

| Prefix | Meaning | Blocks |
|--------|---------|--------|
| [BLOCKER] | Must fix | Yes |
| [MAJOR] | Should fix | Usually |
| [MINOR] | Nice to have | No |
| [NIT] | Style | No |
| [QUESTION] | Clarification | Maybe |
| [PRAISE] | Good job! | No |

---

## Response Times

| Priority | First Response |
|----------|----------------|
| Critical | 2 hours |
| High | 4 hours |
| Normal | 24 hours |
| Low | 48 hours |
