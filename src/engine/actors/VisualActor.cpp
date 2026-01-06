/**
 * @file VisualActor.cpp
 * @brief Visual Actor Implementation
 */
#include "VisualActor.h"

namespace engine {

VisualActor::VisualActor(const std::string& name)
    : ActorObjectExtended(name)
    , m_opacity(1.0f)
    , m_zIndex(0)
{
}

} // namespace engine
