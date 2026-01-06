/**
 * @file ActorObjectExtended.h
 * @brief ActorObject with Component Support
 * 
 * Extended version of ActorObject (Node2D) that supports components.
 * This allows composition-based architecture alongside inheritance.
 */
#pragma once

#include "CoreRedirects.h"
#include "ActorComponent.h"
#include <vector>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace engine {

/**
 * @brief ActorObject with component system support
 * 
 * Extends Node2D (ActorObject) with UE5-style component management.
 * Allows attaching components for composition-based functionality.
 * 
 * Example:
 * @code
 * auto actor = std::make_unique<ActorObjectExtended>("MyActor");
 * auto* sprite = actor->addComponent<SpriteComponent>();
 * sprite->setTexture(myTexture);
 * @endcode
 */
class ActorObjectExtended : public ActorObject {
public:
    ActorObjectExtended(const std::string& name);
    virtual ~ActorObjectExtended() = default;
    
    // ========================================================================
    // COMPONENT MANAGEMENT
    // ========================================================================
    
    /**
     * @brief Add a component to this actor
     * @tparam T Component type
     * @param args Constructor arguments for component
     * @return Pointer to created component
     */
    template<typename T, typename... Args>
    T* addComponent(Args&&... args) {
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        component->setOwner(this);
        component->initialize();
        
        T* ptr = component.get();
        m_components.push_back(std::move(component));
        m_componentMap[std::type_index(typeid(T))] = ptr;
        
        return ptr;
    }
    
    /**
     * @brief Get a component by type
     * @tparam T Component type
     * @return Pointer to component, or nullptr if not found
     */
    template<typename T>
    T* getComponent() {
        auto it = m_componentMap.find(std::type_index(typeid(T)));
        if (it != m_componentMap.end()) {
            return static_cast<T*>(it->second);
        }
        return nullptr;
    }
    
    /**
     * @brief Remove a component by type
     * @tparam T Component type
     */
    template<typename T>
    void removeComponent() {
        auto it = m_componentMap.find(std::type_index(typeid(T)));
        if (it != m_componentMap.end()) {
            ActorComponent* comp = it->second;
            comp->shutdown();
            
            m_components.erase(
                std::remove_if(m_components.begin(), m_components.end(),
                    [comp](const auto& c) { return c.get() == comp; }),
                m_components.end());
            
            m_componentMap.erase(it);
        }
    }
    
    /**
     * @brief Get all components
     * @return Vector of all components
     */
    const std::vector<std::unique_ptr<ActorComponent>>& getComponents() const {
        return m_components;
    }
    
    // ========================================================================
    // LIFECYCLE (Calls components)
    // ========================================================================
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    
private:
    std::vector<std::unique_ptr<ActorComponent>> m_components;
    std::unordered_map<std::type_index, ActorComponent*> m_componentMap;
};

} // namespace engine
