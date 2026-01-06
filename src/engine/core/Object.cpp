/**
 * @file Object.cpp
 * @brief Implementation of Object base class
 */
#include "Object.h"

namespace engine {

Object::Object(const std::string& name)
    : m_name(name)
    , m_active(true)
{
}

} // namespace engine
