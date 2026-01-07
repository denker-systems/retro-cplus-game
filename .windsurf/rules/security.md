---
trigger: always_on
description: Security guidelines
---

# Security Guidelines

> Security best practices

## Input Validation

```cpp
// Always validate external input
if (userId.length() > MAX_USER_ID_LENGTH) {
    return Error::InvalidInput;
}

// Sanitize file paths
if (path.find("..") != std::string::npos) {
    return Error::InvalidPath;
}
```

---

## Memory Safety

- Use smart pointers
- Bounds check arrays
- Initialize variables
- Avoid raw pointers for ownership

---

## File Access

```cpp
// Only access allowed paths
bool isValidAssetPath(const std::string& path) {
    return path.find("assets/") == 0 &&
           path.find("..") == std::string::npos;
}
```

---

## Secrets

- ❌ Never hardcode API keys
- ❌ Never commit secrets
- ✓ Use environment variables
- ✓ Use secure config files

---

## Save Data

- Validate loaded data
- Handle corrupted saves gracefully
- Consider save file integrity checks
