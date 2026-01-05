# Build

<cmake>
Bygga projektet:
`powershell
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
`
</cmake>

<vcpkg>
Installera dependencies:
`powershell
vcpkg install sdl2:x64-windows sdl2-image:x64-windows sdl2-mixer:x64-windows
`
</vcpkg>

<run>
Köra spelet:
`powershell
.\build\Release\RetroAdventure.exe
`
</run>
