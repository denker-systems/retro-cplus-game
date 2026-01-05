#pragma once

#include <SDL.h>
#include <string>
#include <vector>

struct Hotspot {
    SDL_Rect rect;
    std::string name;
    int type;
};

class Room {
public:
    Room(const std::string& name);

    void render(SDL_Renderer* renderer);
    void addHotspot(int x, int y, int w, int h, const std::string& name, int type);

    const std::string& getName() const { return m_name; }

private:
    std::string m_name;
    std::vector<Hotspot> m_hotspots;
    SDL_Color m_bgColor = {20, 20, 80, 255};
};
