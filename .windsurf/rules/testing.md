---
trigger: always_on
description: Testing standards and requirements
---

# Testing Standards

> Enterprise testing requirements

## Test Pyramid

```
       /\
      /  \        UI Tests (10%)
     /────\
    /      \      Integration (20%)
   /────────\
  /          \    Unit Tests (70%)
 /────────────\
```

---

## Coverage Requirements

| Code Type | Minimum | Target |
|-----------|---------|--------|
| Core engine | 80% | 90% |
| Game logic | 60% | 80% |
| UI/Editor | 40% | 60% |
| Critical paths | 95% | 100% |

---

## Unit Test Example

```cpp
TEST(PhysicsBodyTest, AppliesGravity) {
    PhysicsBody body;
    body.setGravity(9.8f);
    body.update(1.0f);
    EXPECT_NEAR(body.getVelocity().y, 9.8f, 0.001f);
}
```

---

## Integration Test Example

```cpp
TEST(SceneTest, ActorsReceiveUpdates) {
    Scene scene;
    auto actor = scene.createActor<TestActor>();
    scene.update(0.016f);
    EXPECT_TRUE(actor->wasUpdated());
}
```

---

## Test Naming

```cpp
TEST(ClassName, MethodName_Condition_ExpectedResult)
TEST(PlayerCharacter, Move_WithCollision_StopsAtWall)
```

---

## Running Tests

```powershell
# All tests
ctest -C Release --output-on-failure

# Specific tests
ctest -C Release -R "Physics" --output-on-failure

# Parallel
ctest -C Release --parallel 8
```

---

## Test Checklist

- [ ] Unit tests for new code
- [ ] Integration tests for systems
- [ ] No test regressions
- [ ] Coverage not decreased
- [ ] Edge cases covered
