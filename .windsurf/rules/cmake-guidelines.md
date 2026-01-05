---
trigger: glob
globs: ["CMakeLists.txt"]
---

# CMake Guidelines

<structure>
- cmake_minimum_required(VERSION 3.16)
- project() with VERSION and LANGUAGES
- set(CMAKE_CXX_STANDARD 17)
- find_package for dependencies
- target_link_libraries with PRIVATE
</structure>

<vcpkg>
Use vcpkg toolchain:
`cmake
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
`
</vcpkg>

<assets>
Copy assets to build directory:
`cmake
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
`
</assets>
