---
trigger: glob
globs: ["*.cpp", "*.h"]
---

# C++ Code Guidelines

<headers>
- Use #pragma once for include guards
- Forward declare classes when possible
- Include only necessary headers
- Order: standard lib, SDL, project headers
</headers>

<classes>
- Constructor initializer lists preferred
- Rule of five for resource-owning classes
- Virtual destructor for base classes
- Mark methods const when they don't modify state
</classes>

<memory>
- Use std::unique_ptr for sole ownership
- Use std::shared_ptr only when sharing is needed
- Avoid raw new/delete
- SDL resources: wrap in RAII classes or use custom deleters
</memory>

<error_handling>
- Check SDL function returns
- Use std::cerr for error messages
- Include SDL_GetError() in error messages
</error_handling>
