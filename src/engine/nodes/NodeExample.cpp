/**
 * @file NodeExample.cpp
 * @brief Example usage of Node Scene Graph system
 * 
 * This file demonstrates how to use the new Node system.
 * Include this in your game code to see how it works.
 */

#include "core/Node.h"
#include "core/Node2D.h"
#include "nodes/Sprite.h"
#include "world/Camera2D.h"
#include "world/Scene.h"

using namespace engine;

/**
 * Example: Creating a simple scene with sprites
 */
void exampleBasicScene(SDL_Renderer* renderer, SDL_Texture* playerTexture) {
    // Create scene
    auto scene = std::make_unique<Scene>("GameLevel");
    
    // Create camera
    Camera2D* camera = scene->createDefaultCamera();
    camera->setViewportSize(640, 400);
    camera->setZoom(1.0f);
    
    // Create player sprite
    auto player = std::make_unique<Sprite>("Player", playerTexture);
    player->setPosition(320, 200);
    player->centerOrigin();
    
    // Create enemy sprite (child of player for demo)
    auto enemy = std::make_unique<Sprite>("Enemy", playerTexture);
    enemy->setPosition(50, 0);  // 50 pixels to the right of player
    enemy->setTint(255, 100, 100, 255);  // Red tint
    
    // Add enemy as child of player (will move with player)
    player->addChild(std::move(enemy));
    
    // Add player to scene
    scene->addChild(std::move(player));
    
    // Update and render
    scene->update(0.016f);  // ~60 FPS
    scene->render(renderer);
}

/**
 * Example: Camera following a target
 */
void exampleCameraFollow(SDL_Renderer* renderer) {
    auto scene = std::make_unique<Scene>("FollowDemo");
    
    // Create player
    auto player = std::make_unique<Node2D>("Player");
    player->setPosition(100, 100);
    Node2D* playerPtr = player.get();
    scene->addChild(std::move(player));
    
    // Create camera that follows player
    auto camera = std::make_unique<Camera2D>("MainCamera");
    camera->setViewportSize(640, 400);
    camera->setTarget(playerPtr);
    camera->setSmoothing(0.1f);  // Smooth follow
    camera->setOffset(0, -50);   // Look slightly ahead
    
    scene->setActiveCamera(camera.get());
    scene->addChild(std::move(camera));
    
    // Move player
    playerPtr->translate(10, 5);
    
    // Update (camera will smoothly follow)
    scene->update(0.016f);
}

/**
 * Example: Hierarchical transforms
 */
void exampleHierarchy() {
    // Create parent
    auto parent = std::make_unique<Node2D>("Parent");
    parent->setPosition(100, 100);
    parent->setRotation(0.785f);  // 45 degrees
    parent->setScale(2.0f, 2.0f);
    
    // Create child
    auto child = std::make_unique<Node2D>("Child");
    child->setPosition(50, 0);  // Local position
    
    Node2D* childPtr = child.get();
    parent->addChild(std::move(child));
    
    // Child's global position is affected by parent's transform
    Vec2 globalPos = childPtr->getGlobalPosition();
    // globalPos will be rotated and scaled relative to parent
}

/**
 * Example: Z-ordering for layering
 */
void exampleZOrdering(SDL_Renderer* renderer, SDL_Texture* texture) {
    auto scene = std::make_unique<Scene>("LayerDemo");
    
    // Background (z = -10)
    auto bg = std::make_unique<Sprite>("Background", texture);
    bg->setPosition(320, 200);
    scene->addChild(std::move(bg), -10);
    
    // Player (z = 0, default)
    auto player = std::make_unique<Sprite>("Player", texture);
    player->setPosition(320, 200);
    scene->addChild(std::move(player), 0);
    
    // Foreground overlay (z = 10)
    auto fg = std::make_unique<Sprite>("Foreground", texture);
    fg->setPosition(320, 200);
    scene->addChild(std::move(fg), 10);
    
    // Render order: bg -> player -> fg
    scene->render(renderer);
}

/**
 * Example: Screen shake
 */
void exampleScreenShake() {
    auto camera = std::make_unique<Camera2D>("ShakeCamera");
    
    // Trigger shake (intensity, duration)
    camera->shake(10.0f, 0.5f);  // 10 pixels for 0.5 seconds
    
    // Update will apply shake offset
    camera->update(0.016f);
}

/**
 * Example: Finding nodes by name
 */
void exampleNodeSearch() {
    auto scene = std::make_unique<Scene>("SearchDemo");
    
    auto player = std::make_unique<Node2D>("Player");
    scene->addChild(std::move(player));
    
    // Find by name
    Node* found = scene->getChild("Player");
    if (found) {
        // Cast to specific type if needed
        Node2D* player2D = dynamic_cast<Node2D*>(found);
        if (player2D) {
            player2D->setPosition(100, 100);
        }
    }
}
