#include "DamageNumberManager.h"
#include "Logger.h"

DamageNumberManager::DamageNumberManager() : font(nullptr)
{
}

DamageNumberManager::~DamageNumberManager()
{
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}

bool DamageNumberManager::init_font(const std::string& font_path, int font_size)
{
    font = TTF_OpenFont(font_path.c_str(), font_size);
    if (!font) {
        Logger::warn(Logger::SYS, "DamageNumberManager: Failed to load font " + font_path + " - " + TTF_GetError());
        return false;
    }
    
    Logger::info(Logger::SYS, "DamageNumberManager: Font loaded successfully from " + font_path);
    return true;
}

void DamageNumberManager::add_damage_number(int damage, float x, float y, SDL_Color color)
{
    damage_numbers.emplace_back(damage, x, y, color);
    
    Logger::debug(Logger::SYS, "DamageNumberManager: Added damage number -" 
        + std::to_string(damage) + " at (" + std::to_string((int)x) 
        + "," + std::to_string((int)y) + ")");
}

void DamageNumberManager::update(float delta)
{
    // Update all damage numbers
    for (auto& damage_num : damage_numbers) {
        damage_num.update(delta);
    }
    
    // Remove expired numbers
    damage_numbers.erase(
        std::remove_if(damage_numbers.begin(), damage_numbers.end(),
            [](const DamageNumber& num) { return num.is_expired(); }),
        damage_numbers.end()
    );
}

void DamageNumberManager::draw(SDL_Renderer* renderer)
{
    if (!font) return;
    
    for (auto& damage_num : damage_numbers) {
        damage_num.draw(renderer, font);
    }
}

void DamageNumberManager::clear()
{
    damage_numbers.clear();
    Logger::debug(Logger::SYS, "DamageNumberManager: Cleared all damage numbers");
}