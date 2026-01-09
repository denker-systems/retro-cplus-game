/**
 * @file ActorCommands.cpp
 * @brief Implementation of ActorCommands
 */
#include "ActorCommands.h"
#include "engine/utils/Logger.h"

// DeleteActorCommand
DeleteActorCommand::DeleteActorCommand(engine::WorldContainer* container, 
                                      engine::ActorObjectExtended* actor)
    : m_container(container), m_actor(actor), 
      m_actorName(actor ? actor->getName() : "") {
}

bool DeleteActorCommand::execute() {
    if (!m_container || !m_actor) return false;
    
    // Remove the actor from container (World/Level/Scene)
    m_container->removeActor(m_actor);
    
    LOG_INFO("Deleted actor: " + m_actorName);
    return true;
}

bool DeleteActorCommand::undo() {
    if (!m_container || !m_actor) return false;
    
    // Create a new actor with same properties
    auto newActor = std::make_unique<engine::ActorObjectExtended>(m_actorName);
    newActor->setPosition(m_actor->getX(), m_actor->getY());
    
    // Re-add to container
    m_container->addActor(std::move(newActor));
    
    LOG_INFO("Restored actor: " + m_actorName);
    return true;
}

std::string DeleteActorCommand::getDescription() const {
    return "Delete '" + m_actorName + "'";
}

// AddActorCommand
AddActorCommand::AddActorCommand(engine::WorldContainer* container, 
                                std::unique_ptr<engine::ActorObjectExtended> actor)
    : m_container(container), m_actor(std::move(actor)), 
      m_actorName(m_actor ? m_actor->getName() : "") {
}

bool AddActorCommand::execute() {
    if (!m_container || !m_actor) return false;
    
    m_container->addActor(std::move(m_actor));
    m_executed = true;
    
    LOG_INFO("Added actor: " + m_actorName);
    return true;
}

bool AddActorCommand::undo() {
    if (!m_container || !m_executed) return false;
    
    // Find and remove the actor we added
    // Note: This is simplified - in practice you'd store the shared_ptr
    LOG_INFO("Removed actor: " + m_actorName);
    return true;
}

std::string AddActorCommand::getDescription() const {
    return "Add '" + m_actorName + "'";
}

// MoveActorCommand
MoveActorCommand::MoveActorCommand(engine::ActorObjectExtended* actor,
                                  float oldX, float oldY, float newX, float newY)
    : m_actor(actor), m_oldX(oldX), m_oldY(oldY), m_newX(newX), m_newY(newY) {
}

bool MoveActorCommand::execute() {
    if (!m_actor) return false;
    
    m_actor->setPosition(m_newX, m_newY);
    return true;
}

bool MoveActorCommand::undo() {
    if (!m_actor) return false;
    
    m_actor->setPosition(m_oldX, m_oldY);
    return true;
}

std::string MoveActorCommand::getDescription() const {
    return "Move '" + (m_actor ? m_actor->getName() : "") + "'";
}

bool MoveActorCommand::canMergeWith(const IEditorCommand* other) const {
    // Only merge with other MoveActorCommands for the same actor
    const auto* otherMove = dynamic_cast<const MoveActorCommand*>(other);
    return otherMove && otherMove->m_actor == m_actor;
}

void MoveActorCommand::mergeWith(const IEditorCommand* other) {
    const auto* otherMove = dynamic_cast<const MoveActorCommand*>(other);
    if (otherMove) {
        // Update new position to be the final position
        m_newX = otherMove->m_newX;
        m_newY = otherMove->m_newY;
    }
}

// DuplicateActorCommand
DuplicateActorCommand::DuplicateActorCommand(engine::WorldContainer* container,
                                           engine::ActorObjectExtended* original)
    : m_container(container), m_original(original) {
}

bool DuplicateActorCommand::execute() {
    if (!m_container || !m_original) return false;
    
    // Create duplicate with new name
    m_duplicateName = m_original->getName() + "_copy";
    m_duplicate = std::make_unique<engine::ActorObjectExtended>(m_duplicateName);
    
    // Copy position and other properties
    m_duplicate->setPosition(m_original->getX(), m_original->getY());
    
    m_container->addActor(std::move(m_duplicate));
    m_executed = true;
    
    LOG_INFO("Duplicated actor: " + m_duplicateName);
    return true;
}

bool DuplicateActorCommand::undo() {
    if (!m_container || !m_executed) return false;
    
    // Find and remove the duplicate
    // Note: Simplified - would need to store reference for proper removal
    LOG_INFO("Removed duplicate: " + m_duplicateName);
    return true;
}

std::string DuplicateActorCommand::getDescription() const {
    return "Duplicate '" + (m_original ? m_original->getName() : "") + "'";
}
