#pragma once

#include "Panel.h"

class WaveNotification : public Panel
{
private:
    bool is_visible;
    float display_timer;
    float fade_alpha;
    
    static constexpr float DISPLAY_DURATION = 3.0f;  // 3 seconds total
    static constexpr float FADE_DURATION = 0.5f;     // 0.5 seconds fade out

public:
    WaveNotification();
    
    void show_notification(const std::string& message);
    void update_notification(float delta);
    void draw_notification(SDL_Renderer* renderer);
    
    bool get_is_visible() const { return is_visible; }
};