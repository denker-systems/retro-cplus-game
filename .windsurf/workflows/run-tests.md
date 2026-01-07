---
description: Execute test suite with various configurations
---

# Run Tests Workflow

> Enterprise testing pipeline

## Quick Test

### Run All Tests
// turbo
```powershell
cd build; ctest -C Release --output-on-failure
```

### Run with Verbose Output
```powershell
ctest -C Release -V
```

---

## Test Categories

### Unit Tests
```powershell
ctest -C Release -R "Unit" --output-on-failure
```

### Integration Tests
```powershell
ctest -C Release -R "Integration" --output-on-failure
```

### Performance Tests
```powershell
ctest -C Release -R "Perf" --output-on-failure
```

---

## Test Filtering

### Run Specific Test
```powershell
ctest -C Release -R "TestPhysics" --output-on-failure
```

### Exclude Tests
```powershell
ctest -C Release -E "Slow|Flaky" --output-on-failure
```

### Run Tests Matching Pattern
```powershell
ctest -C Release -R "Actor.*" --output-on-failure
```

---

## Test Execution Options

### Parallel Execution
```powershell
ctest -C Release --parallel 8 --output-on-failure
```

### Stop on First Failure
```powershell
ctest -C Release --stop-on-failure --output-on-failure
```

### Rerun Failed Tests
```powershell
ctest -C Release --rerun-failed --output-on-failure
```

---

## Test Output

### Generate JUnit XML
```powershell
ctest -C Release --output-junit test-results.xml
```

### Generate Coverage Report
```powershell
# With coverage build
cmake --build . --config Debug --target coverage
```

---

## Test Debugging

### Run Single Test with Output
```powershell
ctest -C Release -R "SpecificTest" -V
```

### Run Test in Debugger
```powershell
# Get test command
ctest -C Release -R "SpecificTest" -N

# Run manually in VS debugger
```

---

## GoogleTest Integration

### List All Tests
```powershell
.\Release\TestRunner.exe --gtest_list_tests
```

### Run with Filter
```powershell
.\Release\TestRunner.exe --gtest_filter="Physics*"
```

### Run with Repeat
```powershell
.\Release\TestRunner.exe --gtest_repeat=10
```

---

## CI Test Script

```powershell
$ErrorActionPreference = "Stop"

Write-Host "=== Running Test Suite ===" -ForegroundColor Cyan

# Build tests
cmake --build . --config Release

# Run tests
$result = ctest -C Release --output-on-failure --output-junit test-results.xml

# Check results
if ($LASTEXITCODE -ne 0) {
    Write-Host "=== TESTS FAILED ===" -ForegroundColor Red
    exit 1
}

Write-Host "=== ALL TESTS PASSED ===" -ForegroundColor Green
```

---

## Test Coverage

### Generate Coverage
```powershell
# Configure with coverage
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCODE_COVERAGE=ON

# Build
cmake --build . --config Debug

# Run tests
ctest -C Debug

# Generate report
gcovr --html coverage.html
```

### Coverage Targets
- **Minimum:** 60% line coverage
- **Target:** 80% line coverage
- **Critical paths:** 95% coverage

---

## Flaky Test Handling

### Identify Flaky Tests
```powershell
# Run multiple times
for ($i = 0; $i -lt 10; $i++) {
    ctest -C Release --output-on-failure
}
```

### Mark as Flaky
```cmake
set_tests_properties(FlakyTest PROPERTIES LABELS "Flaky")
```

### Skip Flaky in CI
```powershell
ctest -C Release -LE "Flaky" --output-on-failure
```

---

## Test Checklist

- [ ] All unit tests passing
- [ ] Integration tests passing
- [ ] No new test failures
- [ ] Coverage not decreased
- [ ] Flaky tests documented
- [ ] Performance tests within limits
