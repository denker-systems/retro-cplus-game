/**
 * @file ActorCommands.h
 * @brief Commands for actor editing (Command Pattern)
 */
#pragma once

#include "editor/core/IEditorCommand.h"
#include "engine/world/Scene.h"
#include "engine/core/ActorObjectExtended.h"
#include <memory>

/**
 * @brief Delete an actor from scene
 */
class DeleteActorCommand : public IEditorCommand {
public:
    DeleteActorCommand(engine::Scene* scene, engine::ActorObjectExtended* actor);
    
    bool execute() override;
    bool undo() override;
    std::string getDescription() const override;

private:
    engine::Scene* m_scene;
    engine::ActorObjectExtended* m_actor;
    std::string m_actorName;
};

/**
 * @brief Add an actor to scene
 */
class AddActorCommand : public IEditorCommand {
public:
    AddActorCommand(engine::Scene* scene, std::unique_ptr<engine::ActorObjectExtended> actor);
    
    bool execute() override;
    bool undo() override;
    std::string getDescription() const override;

private:
    engine::Scene* m_scene;
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
 * @brief Duplicate an actor
 */
class DuplicateActorCommand : public IEditorCommand {
public:
    DuplicateActorCommand(engine::Scene* scene, 
                         engine::ActorObjectExtended* original);
    
    bool execute() override;
    bool undo() override;
    std::string getDescription() const override;

private:
    engine::Scene* m_scene;
    engine::ActorObjectExtended* m_original;
    std::unique_ptr<engine::ActorObjectExtended> m_duplicate;
    std::string m_duplicateName;
    bool m_executed = false;
};
