/**
 * @file EventBus.cpp
 * @brief Implementation av central event bus
 */
#include "EventBus.h"
#include "utils/Logger.h"

EventBus& EventBus::instance() {
    static EventBus inst;
    return inst;
}

void EventBus::logEvent(const char* eventName) {
    if (m_loggingEnabled) {
        LOG_DEBUG(std::string("Event: ") + eventName);
    }
}
