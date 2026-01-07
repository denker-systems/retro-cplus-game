---
description: Add new tests to the project
---

# Add Test Workflow

> Creating comprehensive tests

## Test Types

| Type | Purpose | Location |
|------|---------|----------|
| Unit | Single function/class | `tests/unit/` |
| Integration | Multiple systems | `tests/integration/` |
| Performance | Benchmarks | `tests/perf/` |
| Smoke | Quick sanity | `tests/smoke/` |

---

## 1. Unit Test Structure

### Create Test File
`tests/unit/test_[ClassName].cpp`:

```cpp
#include <gtest/gtest.h>
#include "engine/[path]/[ClassName].h"

class [ClassName]Test : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup before each test
    }
    
    void TearDown() override {
        // Cleanup after each test
    }
    
    // Test fixtures
    std::unique_ptr<[ClassName]> m_instance;
};

// Test naming: MethodName_Condition_ExpectedResult
TEST_F([ClassName]Test, Constructor_Default_InitializesCorrectly) {
    auto obj = std::make_unique<[ClassName]>();
    EXPECT_NE(obj, nullptr);
}

TEST_F([ClassName]Test, SetValue_ValidInput_UpdatesValue) {
    m_instance->setValue(42);
    EXPECT_EQ(m_instance->getValue(), 42);
}

TEST_F([ClassName]Test, SetValue_NegativeInput_ThrowsException) {
    EXPECT_THROW(m_instance->setValue(-1), std::invalid_argument);
}
```

---

## 2. Test Patterns

### Arrange-Act-Assert
```cpp
TEST(PhysicsTest, Collision_TwoOverlappingBodies_ReturnsTrue) {
    // Arrange
    PhysicsBody bodyA({0, 0}, {10, 10});
    PhysicsBody bodyB({5, 5}, {10, 10});
    
    // Act
    bool result = PhysicsSystem::checkCollision(bodyA, bodyB);
    
    // Assert
    EXPECT_TRUE(result);
}
```

### Parameterized Tests
```cpp
class DamageTest : public ::testing::TestWithParam<std::tuple<int, int, int>> {};

TEST_P(DamageTest, CalculateDamage_ReturnsExpected) {
    auto [attack, defense, expected] = GetParam();
    EXPECT_EQ(calculateDamage(attack, defense), expected);
}

INSTANTIATE_TEST_SUITE_P(
    DamageValues,
    DamageTest,
    ::testing::Values(
        std::make_tuple(10, 5, 5),
        std::make_tuple(10, 10, 0),
        std::make_tuple(100, 25, 75)
    )
);
```

### Mock Objects
```cpp
class MockAudioSystem : public IAudioSystem {
public:
    MOCK_METHOD(void, playSound, (const std::string& name), (override));
    MOCK_METHOD(void, stopSound, (const std::string& name), (override));
};

TEST(PlayerTest, TakeDamage_PlaysDamageSound) {
    MockAudioSystem mockAudio;
    Player player(&mockAudio);
    
    EXPECT_CALL(mockAudio, playSound("damage"));
    player.takeDamage(10);
}
```

---

## 3. Integration Tests

```cpp
class SceneIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize full system stack
        SDL_Init(SDL_INIT_VIDEO);
        m_window = SDL_CreateWindow("Test", 0, 0, 640, 480, SDL_WINDOW_HIDDEN);
        m_renderer = SDL_CreateRenderer(m_window, -1, 0);
        m_scene = std::make_unique<Scene>("TestScene");
    }
    
    void TearDown() override {
        m_scene.reset();
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }
};

TEST_F(SceneIntegrationTest, AddActor_ActorReceivesUpdates) {
    auto actor = std::make_unique<TestActor>();
    auto* actorPtr = actor.get();
    
    m_scene->addActor(std::move(actor));
    m_scene->update(0.016f);
    
    EXPECT_TRUE(actorPtr->wasUpdated());
}
```

---

## 4. Performance Tests

```cpp
TEST(RenderingPerf, Render1000Sprites_Under16ms) {
    // Setup
    std::vector<SpriteActor> sprites(1000);
    
    // Measure
    auto start = std::chrono::high_resolution_clock::now();
    
    for (auto& sprite : sprites) {
        sprite.render(renderer);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Assert
    EXPECT_LT(duration.count(), 16) << "Rendering too slow for 60 FPS";
}
```

---

## 5. Update CMakeLists.txt

```cmake
# Enable testing
enable_testing()
include(GoogleTest)

# Add test executable
add_executable(Tests
    tests/unit/test_Actor.cpp
    tests/unit/test_Scene.cpp
    tests/integration/test_SceneIntegration.cpp
)

target_link_libraries(Tests PRIVATE
    RetroCore
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
)

# Register tests
gtest_discover_tests(Tests)
```

---

## 6. Run Tests

```powershell
# Build and run all tests
cd build
cmake --build . --config Release
ctest -C Release --output-on-failure

# Run specific test
ctest -R "ActorTest" --output-on-failure

# Verbose output
ctest -V
```

---

## Checklist

- [ ] Test file created
- [ ] CMakeLists.txt updated
- [ ] Tests compile
- [ ] All tests pass
- [ ] Edge cases covered
- [ ] Mocks used where appropriate
- [ ] Performance tests (if applicable)
