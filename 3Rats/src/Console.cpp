#include "Console.h"
#include <sstream>
#include <iostream>

static const int CONSOLE_H     = 180;
static const int MAX_HISTORY   = 7;
static const int LINE_H        = 18;
static const int PADDING       = 6;

Console::Console()
    : is_open(false), font(nullptr), renderer(nullptr),
      player_array(nullptr), player_amount(0),
      map_array(nullptr),   map_amount(0),
      topography(nullptr),
      item_array(nullptr),  item_amount(0),
      tile_array(nullptr),  tile_amount(0)
{}

Console::~Console()
{
    if (font) TTF_CloseFont(font);
}

void Console::init(SDL_Renderer* rend,
                   Acteur* players, int p_amount,
                   Map*    maps,    int m_amount,
                   Topography* topo,
                   Item*   items,   int i_amount,
                   Tile*   tiles,   int t_amount)
{
    renderer     = rend;
    player_array = players;  player_amount = p_amount;
    map_array    = maps;     map_amount    = m_amount;
    topography   = topo;
    item_array   = items;    item_amount   = i_amount;
    tile_array   = tiles;    tile_amount   = t_amount;

    font = TTF_OpenFont("../fonts/sans.ttf", 13);
    if (!font) std::cerr << "Console: failed to open font: " << TTF_GetError() << std::endl;

    log("console ready. type 'help' for commands.");
}

void Console::log(const std::string& msg)
{
    history.push_back(msg);
    if ((int)history.size() > MAX_HISTORY)
        history.erase(history.begin());
}

void Console::handle_event(const SDL_Event& e)
{
    if (e.type == SDL_KEYDOWN)
    {
        if (e.key.keysym.scancode == SDL_SCANCODE_GRAVE)
        {
            is_open = !is_open;
            if (is_open)  SDL_StartTextInput();
            else          SDL_StopTextInput();
            return;
        }

        if (!is_open) return;

        if (e.key.keysym.sym == SDLK_RETURN)
        {
            if (!input.empty())
            {
                log("> " + input);
                execute(input);
                input.clear();
            }
        }
        else if (e.key.keysym.sym == SDLK_BACKSPACE && !input.empty())
        {
            input.pop_back();
        }
    }
    else if (e.type == SDL_TEXTINPUT && is_open)
    {
        // ignore the backtick that opened the console
        if (std::string(e.text.text) != "`")
            input += e.text.text;
    }
}

void Console::execute(const std::string& cmd)
{
    std::istringstream ss(cmd);
    std::string token;
    ss >> token;

    // ---- help ----
    if (token == "help")
    {
        log("tp <x> <y>      teleport rat to tile");
        log("map <id>        jump to map id");
        log("give            give mushroom to rat 0");
        log("speed <val>     set rat 0 move speed");
        log("pos             print rat 0 position");
        log("regen <0|1|2>   regen current map (0=maze 1=garden 2=cage)");
    }
    // ---- tp ----
    else if (token == "tp")
    {
        int tx, ty;
        if (ss >> tx >> ty)
        {
            player_array[0].set_cords(tx * 64, ty * 64);
            log("teleported to tile " + std::to_string(tx) + "," + std::to_string(ty));
        }
        else log("usage: tp <tile_x> <tile_y>");
    }
    // ---- map ----
    else if (token == "map")
    {
        int id;
        if (ss >> id)
        {
            if (id >= 0 && id < map_amount)
            {
                topography->set_current_map_id(id);
                map_array[id].set_textures();
                log("switched to map " + std::to_string(id));
            }
            else log("map id out of range (0-" + std::to_string(map_amount - 1) + ")");
        }
        else log("usage: map <id>");
    }
    // ---- give ----
    else if (token == "give")
    {
        player_array[0].debug_give_item(item_array, item_amount);
        log("gave mushroom to rat 0");
    }
    // ---- speed ----
    else if (token == "speed")
    {
        float val;
        if (ss >> val)
        {
            player_array[0].set_move_speed(val);
            log("speed set to " + std::to_string((int)val));
        }
        else log("usage: speed <value>");
    }
    // ---- pos ----
    else if (token == "pos")
    {
        int ox = player_array[0].get_origin_x();
        int oy = player_array[0].get_origin_y();
        log("rat0 px=" + std::to_string(ox) + " py=" + std::to_string(oy)
            + " tile=" + std::to_string(ox / 64) + "," + std::to_string(oy / 64));
    }
    // ---- regen ----
    else if (token == "regen")
    {
        int type;
        if (ss >> type)
        {
            int id = topography->get_current_map_id();
            map_array[id].set_type(type);
            map_array[id].set_textures();
            log("regenerated map " + std::to_string(id) + " type " + std::to_string(type));
        }
        else log("usage: regen <0|1|2>");
    }
    else
    {
        log("unknown command: " + token);
    }
}

void Console::draw_rect_alpha(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

void Console::draw_text(const std::string& text, int x, int y, SDL_Color colour)
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

void Console::draw()
{
    if (!is_open) return;

    int screen_w = 600;
    int screen_h = 420;
    int cy = screen_h - CONSOLE_H;

    // background
    draw_rect_alpha(0, cy, screen_w, CONSOLE_H, 0, 0, 0, 200);
    // top border
    SDL_SetRenderDrawColor(renderer, 80, 200, 80, 255);
    SDL_RenderDrawLine(renderer, 0, cy, screen_w, cy);

    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color green  = {80,  200, 80,  255};
    SDL_Color grey   = {180, 180, 180, 255};

    // history lines
    int line_y = cy + PADDING;
    for (const auto& line : history)
    {
        draw_text(line, PADDING, line_y, grey);
        line_y += LINE_H;
    }

    // input line
    int input_y = cy + CONSOLE_H - LINE_H - PADDING;
    draw_rect_alpha(0, input_y - 2, screen_w, LINE_H + 4, 20, 20, 20, 220);
    draw_text("> " + input + "_", PADDING, input_y, green);
}
