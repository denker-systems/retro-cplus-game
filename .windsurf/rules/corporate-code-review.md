---
description: Corporate Code Review Standards for Game Engine Development
trigger: always_on
---

# Code Review Standards

> Enterprise-grade review process för 50+ utvecklarteam

## REVIEW PROCESS

```
            
   AUTHOR    ─  REVIEWER     APPROVER      MERGE     
            
                                                            
  Skapa PR            Review             Approve            Squash
  Self-review         Comment            Sign-off           Merge
  Tests pass          Request changes    Final check        Delete branch
```

---

## 1. AUTHOR RESPONSIBILITIES

### 1.1 Före PR skapas
- [ ] Kod bygger lokalt (Debug + Release)
- [ ] Alla tester passerar
- [ ] Self-review genomförd
- [ ] Dokumentation uppdaterad
- [ ] CHANGELOG entry tillagd

### 1.2 PR Description Template
```markdown
## Summary
[Kort beskrivning av ändringen]

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation
- [ ] Refactoring

## Changes Made
- Ändring 1
- Ändring 2

## Testing
- [ ] Unit tests added/updated
- [ ] Integration tests added/updated
- [ ] Manual testing performed

## Documentation
- [ ] Code comments updated
- [ ] API docs updated
- [ ] Architecture docs updated (if applicable)

## Screenshots (if UI changes)
[Bilder här]

## Related Issues
Closes #XXX
```

---

## 2. REVIEWER CHECKLIST

### 2.1 Code Quality
- [ ] Följer coding standards?
- [ ] Läsbar och förståelig?
- [ ] Ingen duplicerad kod?
- [ ] Rimlig komplexitet?
- [ ] Effektiv implementation?

### 2.2 Documentation
- [ ] Fil-header finns?
- [ ] Klasser dokumenterade?
- [ ] Publika metoder dokumenterade?
- [ ] Inline-kommentarer förklarar VARFÖR?
- [ ] Inga självklara kommentarer?

### 2.3 Architecture
- [ ] Följer SOLID-principer?
- [ ] Rätt abstraktionsnivå?
- [ ] Inga circular dependencies?
- [ ] Component pattern används korrekt?

### 2.4 Safety
- [ ] Memory management korrekt?
- [ ] Thread safety beaktat?
- [ ] Error handling komplett?
- [ ] Inga memory leaks?

### 2.5 Testing
- [ ] Tillräcklig test coverage?
- [ ] Edge cases testade?
- [ ] Performance impact beaktat?

---

## 3. REVIEW COMMENTS

### 3.1 Comment Types
```
[BLOCKER] - Måste åtgärdas före merge
[MAJOR] - Bör åtgärdas, kan blockera
[MINOR] - Förslag, blockerar inte
[NIT] - Stilistisk preferens
[QUESTION] - Fråga för förståelse
[PRAISE] - Positiv feedback
```

### 3.2 Comment Examples
```cpp
// [BLOCKER] Memory leak - texture aldrig freed
SDL_Texture* tex = IMG_LoadTexture(renderer, path);
// Bör vara: auto tex = std::unique_ptr<SDL_Texture, ...>

// [MAJOR] Saknar null-check som kan orsaka crash
void process(Actor* actor) {
    actor->update();  // Vad händer om actor är nullptr?
}

// [MINOR] Överväg att använda range-based for
for (int i = 0; i < vec.size(); i++) { ... }
// Alternativ: for (const auto& item : vec) { ... }

// [NIT] Prefer explicit over auto för klarhet
auto x = getValue();  // Vad är typen?
// Förslag: float x = getValue();

// [PRAISE] Bra separation of concerns!
```

---

## 4. APPROVAL REQUIREMENTS

### 4.1 Standard Changes
- **1 approval** från team member
- Alla CI checks gröna
- Inga blockers

### 4.2 Architecture Changes
- **2 approvals** (minst 1 senior)
- ADR dokumenterad
- Team lead sign-off

### 4.3 Critical Systems
- **2 approvals** från core team
- Performance benchmarks
- Security review (om relevant)

---

## 5. MERGE POLICY

### 5.1 Merge Strategy
- **Squash merge** för feature branches
- **Merge commit** för release branches
- **Rebase** tillåtet före PR

### 5.2 Branch Naming
```
feature/XXX-short-description
bugfix/XXX-short-description
hotfix/XXX-short-description
refactor/XXX-short-description
docs/XXX-short-description
```

### 5.3 Post-Merge
- Ta bort feature branch
- Verifiera CI passerar på main
- Uppdatera relaterade issues

---

## 6. RESPONSE TIME SLA

| Priority | First Response | Resolution |
|----------|---------------|------------|
| Critical | 2 hours | 24 hours |
| High | 4 hours | 48 hours |
| Normal | 24 hours | 1 week |
| Low | 48 hours | 2 weeks |
