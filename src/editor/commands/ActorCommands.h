/**
 * @file ActorCommands.h
 * @brief Commands for actor editing (Command Pattern)
 */
#pragma once

#include "editor/core/IEditorCommand.h"
#include "engine/world/WorldContainer.h"
#include "engine/core/ActorObjectExtended.h"
#include <memory>

/**
 * @brief Delete an actor from any container (World/Level/Scene)
 */
class DeleteActorCommand : public IEditorCommand {
public:
    DeleteActorCommand(engine::WorldContainer* container, engine::ActorObjectExtended* actor);
    
    bool execute() override;
    bool undo() override;
    std::string getDescription() const override;

private:
    engine::WorldContainer* m_container;
    engine::ActorObjectExtended* m_actor;
    std::string m_actorName;
};

/**
 * @brief Add an actor to any container (World/Level/Scene)
 */
class AddActorCommand : public IEditorCommand {
public:
    AddActorCommand(engine::WorldContainer* container, std::unique_ptr<engine::ActorObjectExtended> actor);
    
    bool execute() override;
    bool undo() override;
    std::string getDescription() const override;

private:
    engine::WorldContainer* m_container;
    std::unique_ptr<engine::ActorObjectExtended> m_actor;
    std::string m_actorName;
    bool m_executed = false;
};

/**
 * @brief Move an actor to new position
 */
class MoveActorCommand : public IEditorCommand {
public:
    MoveActorCommand(engine::ActorObjectExtended* actor, 
                    float oldX, float oldY, float newX, float newY);
    
    bool execute() override;
    bool undo() override;
    std::string getDescription() const override;
    
    bool canMergeWith(const IEditorCommand* other) const override;
    void mergeWith(const IEditorCommand* other) override;

private:
    engine::ActorObjectExtended* m_actor;
    float m_oldX, m_oldY;
    float m_newX, m_newY;
};

/**
 * @brief Duplicate an actor in any container (World/Level/Scene)
 */
class DuplicateActorCommand : public IEditorCommand {
public:
    DuplicateActorCommand(engine::WorldContainer* container, 
                         engine::ActorObjectExtended* original);
    
    bool execute() override;
    bool undo() override;
    std::string getDescription() const override;

private:
    engine::WorldContainer* m_container;
    engine::ActorObjectExtended* m_original;
    std::unique_ptr<engine::ActorObjectExtended> m_duplicate;
    std::string m_duplicateName;
    bool m_executed = false;
};
