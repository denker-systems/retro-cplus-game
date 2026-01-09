// Temporary implementation file for setPlayMode with debug logging
#include "Viewport3DPanel.h"
#include <iostream>

namespace editor {

void Viewport3DPanel::setPlayMode(EditorPlayMode* playMode) {
    std::cout << "[Viewport3DPanel] setPlayMode called - playMode=" 
              << (playMode ? "SET" : "NULL") << std::endl;
    m_playMode = playMode;
}

} // namespace editor
