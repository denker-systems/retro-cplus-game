/**
 * @file BaseViewRenderer.cpp
 * @brief Implementation of base view renderer
 */
#include "BaseViewRenderer.h"
#include "editor/core/EditorContext.h"
#include "editor/core/SelectionManager.h"

namespace editor {
namespace viewport {

void BaseViewRenderer::initialize(EditorContext* context, SelectionManager* selectionManager) {
    m_context = context;
    m_selectionManager = selectionManager;
}

void BaseViewRenderer::update(float deltaTime) {
    // Base implementation - can be overridden
}

} // namespace viewport
} // namespace editor
