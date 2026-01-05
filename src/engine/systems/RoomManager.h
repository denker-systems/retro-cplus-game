/**
 * @file RoomManager.h
 * @brief Hanterar rum och rumsövergångar
 */
#pragma once

#include "Room.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

/**
 * @brief Singleton för rumhantering
 */
class RoomManager {
public:
    static RoomManager& instance();
    
    /** @brief Registrera ett rum */
    void addRoom(std::unique_ptr<Room> room);
    
    /** @brief Rensa alla rum (för hot reload) */
    void clearRooms();
    
    /** @brief Hämta rum med ID */
    Room* getRoom(const std::string& roomId);
    
    /** @brief Byt till annat rum */
    bool changeRoom(const std::string& roomId);
    
    /** @brief Hämta aktuellt rum */
    Room* getCurrentRoom() { return m_currentRoom; }
    std::string getCurrentRoomId() const { return m_currentRoom ? m_currentRoom->getId() : ""; }
    
    /** @brief Sätt callback för rumsbyte */
    void setOnRoomChange(std::function<void(const std::string&)> callback) {
        m_onRoomChange = callback;
    }
    
    /** @brief Hämta spelarens startposition i aktuellt rum */
    void getSpawnPosition(float& x, float& y) const {
        x = m_spawnX;
        y = m_spawnY;
    }
    
    /** @brief Sätt spawn-position för nästa rumsbyte */
    void setSpawnPosition(float x, float y) {
        m_spawnX = x;
        m_spawnY = y;
    }

private:
    RoomManager() = default;
    
    std::unordered_map<std::string, std::unique_ptr<Room>> m_rooms;
    Room* m_currentRoom = nullptr;
    std::function<void(const std::string&)> m_onRoomChange;
    
    float m_spawnX = 320.0f;
    float m_spawnY = 300.0f;
};
