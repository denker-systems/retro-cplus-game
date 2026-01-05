---
trigger: always_on
---

# C++ SDL2 Game Project Rules

<tech_stack>
- Language: C++17
- Graphics: SDL2, SDL2_image
- Audio: SDL2_mixer
- Build: CMake 3.16+
- Package Manager: vcpkg
- Platform: Windows 11
</tech_stack>

<code_style>
- Use PascalCase for class names (Game, Player, Room)
- Use camelCase for methods (handleEvents, loadTexture)
- Use m_ prefix for member variables (m_window, m_running)
- Use smart pointers (unique_ptr, shared_ptr) for ownership
- RAII for resource management
- Prefer references over pointers when possible
</code_style>

<architecture>
- Game class is the central coordinator
- Each system is a separate class with single responsibility
- SDL resources wrapped in classes for RAII
- Use forward declarations in headers
- Keep headers minimal, implementation in .cpp
</architecture>

<communication>
- Communicate in Swedish
- Explain concepts pedagogically
</communication>
