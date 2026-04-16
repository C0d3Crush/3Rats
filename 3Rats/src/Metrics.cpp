#include "Metrics.h"
#include <sstream>
#include <iomanip>
#include <mach/mach.h>

static const int W       = 200;
static const int LINE_H  = 16;
static const int PADDING = 6;

Metrics::Metrics()
    : is_open(false), font(nullptr), renderer(nullptr),
      player_array(nullptr), topography(nullptr),
      last_tick(0), fps(0.0f), frame_count(0)
{}

Metrics::~Metrics()
{
    if (font) TTF_CloseFont(font);
}

void Metrics::init(SDL_Renderer* rend, Acteur* players, Topography* topo)
{
    renderer     = rend;
    player_array = players;
    topography   = topo;
    last_tick    = SDL_GetTicks();

    font = TTF_OpenFont("../fonts/sans.ttf", 12);
}

size_t Metrics::get_memory_mb()
{
    task_vm_info_data_t info;
    mach_msg_type_number_t count = TASK_VM_INFO_COUNT;
    if (task_info(mach_task_self(), TASK_VM_INFO, (task_info_t)&info, &count) == KERN_SUCCESS)
        return info.phys_footprint / (1024 * 1024);
    return 0;
}

void Metrics::update()
{
    frame_count++;
    Uint32 now = SDL_GetTicks();
    Uint32 elapsed = now - last_tick;
    if (elapsed >= 500)
    {
        fps        = frame_count / (elapsed / 1000.0f);
        frame_count = 0;
        last_tick   = now;
    }
}

void Metrics::draw_rect_alpha(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

void Metrics::draw_text(const std::string& text, int x, int y, SDL_Color colour)
{
    if (!font || text.empty()) return;
    SDL_Surface* surf = TTF_RenderText_Solid(font, text.c_str(), colour);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    if (!tex) return;
    int tw, th;
    SDL_QueryTexture(tex, nullptr, nullptr, &tw, &th);
    SDL_Rect dst = {x, y, tw, th};
    SDL_RenderCopy(renderer, tex, nullptr, &dst);
    SDL_DestroyTexture(tex);
}

void Metrics::draw()
{
    if (!is_open) return;

    int map_id = topography->get_current_map_id();
    int px     = player_array[0].get_origin_x();
    int py     = player_array[0].get_origin_y();
    int tx     = px / 64;
    int ty     = py / 64;

    size_t mem = get_memory_mb();

    std::ostringstream oss;

    // build lines
    struct Line { std::string label; std::string value; };
    Line lines[] = {
        { "FPS",    std::to_string((int)fps) },
        { "MEM",    std::to_string(mem) + " MB" },
        { "MAP",    std::to_string(map_id) },
        { "TILE",   std::to_string(tx) + ", " + std::to_string(ty) },
        { "PX",     std::to_string(px) + ", " + std::to_string(py) },
    };
    int n = sizeof(lines) / sizeof(lines[0]);
    int panel_h = PADDING * 2 + n * LINE_H;

    draw_rect_alpha(0, 0, W, panel_h, 0, 0, 0, 180);

    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color yellow = {255, 220, 50,  255};

    for (int i = 0; i < n; i++)
    {
        int y = PADDING + i * LINE_H;
        draw_text(lines[i].label, PADDING,        y, yellow);
        draw_text(lines[i].value, PADDING + 50,   y, white);
    }
}
