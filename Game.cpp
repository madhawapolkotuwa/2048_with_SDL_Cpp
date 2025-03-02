#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include <tuple>
#include <map>
#include <string>
#include <algorithm>
#include <cmath>
#include <vector>
#include <functional>
#include <random>

const int WIDTH = 800;
const int HEIGHT = 800;
const int ROWS = 4;
const int COLS = 4;

const int RECT_HIGHT = HEIGHT / ROWS;
const int RECT_WIDTH = WIDTH / ROWS;

const int OUTLINE_THIKNESS = 10;

// Constants for FPS control
const int FPS = 60; // Desired FPS
const int FRAME_DELAY = 1000 / FPS; // Time per frame in milliseconds
const int MOVE_VEL = 20;

std::random_device rd;
std::mt19937 gen(rd());
std::bernoulli_distribution dis(0.1); // 10% chance for `true` (4)
std::uniform_int_distribution<int> dist(0, 3); // Range [0, 3]

SDL_Color FONT_COLOR = {119, 110, 101, 255};

char SValue[10];

enum Direction
{
    DOWN,
    LEFT,
    RIGHT,
    UP
};

TTF_Font* font;
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* textSurface;
SDL_Texture* textTexture;

const int COLORS[][3] = {
{237, 229, 218},
{238, 225, 201},
{243, 178, 122},
{246, 150, 101},
{247, 124, 95 },
{247, 95, 59 },
{237, 208, 115},
{237, 204, 99 },
{236, 202, 80 }};

class Tile
{
public:
int value, row, col, x, y;
std::string key;

    Tile(){
    }

    Tile(int value,int row,int col)
    {
        this->value = value;
        this->row = row;
        this->col = col;
        this->x = col * RECT_WIDTH;
        this->y = row * RECT_HIGHT;
        setKey(row,col);
    }

    void setKey(int row, int col){
        this->key = std::to_string(row) + std::to_string(col);
    }

    const int* getColor() const
    {
        int color_index = static_cast<int>(std::log2(value));
        auto color = COLORS[color_index];
        return color;
    }

    void draw()
    {
        auto color = getColor();
        SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 255);
        SDL_FRect rect{ (float)x, (float)y, (float)RECT_WIDTH, (float)RECT_HIGHT};
        SDL_RenderFillRect(renderer,&rect);
        sprintf(SValue,"%d",value);
        textSurface = TTF_RenderText_Solid(font, SValue, strlen(SValue), FONT_COLOR);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_DestroySurface(textSurface);

        SDL_FRect textRect;
        textRect.x = x + (RECT_WIDTH / 2 - textSurface->w / 2);
        textRect.y = y + (RECT_HIGHT / 2 - textSurface->h / 2);
        textRect.w = textSurface->w;
        textRect.h = textSurface->h;
        SDL_RenderTexture(renderer, textTexture, nullptr, &textRect);
    }

    void set_pos(bool ceil=false)
    {
        if(ceil)
        {
            row = std::ceil(y / RECT_HIGHT);
            col = std::ceil(x / RECT_WIDTH);
            setKey(row,col);
        }
        else
        {
            row = std::floor(y / RECT_HIGHT);
            col = std::floor(x / RECT_WIDTH);
            setKey(row,col);
        }
    }

    void move(int deltaX, int deltaY)
    {
        x += deltaX;
        y += deltaY;
    }

};

std::tuple<int, int> GetRandomPosition(std::vector<Tile> tiles)
{
int row = 0;
int col = 0;
std::string key;
while(true)
{
row = dist(gen);
col = dist(gen);
key = std::to_string(row)+std::to_string(col);
if(tiles.size() != 0)
{
if(std::find_if(tiles.begin(),tiles.end(),[&](const Tile& t){return t.key == key; }) != tiles.end()) continue;
else return { row, col };
}
else
break;
}
return { row, col };
}

std::string EndMove(std::vector<Tile>& tiles)
{
if(tiles.size() == 16){
return "Lost";
}
auto [row, col] = GetRandomPosition(tiles);
tiles.emplace_back(Tile(dis(gen) ? 4 : 2, row, col));
return "continue";
}

void DrawGrid()
{
SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);

    for(int row{0}; row <= ROWS; row++)
    {
        int y = row * RECT_WIDTH;
        SDL_FRect line;
        line.x = 0;
        row == 0 ? line.y = y : ( row == ROWS ? line.y = y - 10 : line.y = y - 5);
        line.w = WIDTH;
        line.h = OUTLINE_THIKNESS;
        SDL_RenderFillRect(renderer,&line);
    }

    for(int col{0}; col <= COLS; col++)
    {
        int x = col * RECT_HIGHT;
        SDL_FRect line;
        col == 0 ? line.x = x : ( col == COLS ? line.x = x - 10 : line.x = x - 5);
        line.y = 0;
        line.w = OUTLINE_THIKNESS;
        line.h = HEIGHT;
        SDL_RenderFillRect(renderer,&line);
    }

}

void DrawMain(std::vector<Tile>& tiles)
{
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 205, 192, 180, 255);
    SDL_RenderClear(renderer);

    for(auto tile : tiles)
    {
        tile.draw();
    }
    DrawGrid();
    // Display
    SDL_RenderPresent(renderer);

}

void GenerateTiles(std::vector<Tile>& tiles)
{
for(int _{0}; _< 2; _++)
{
auto [row, col] = GetRandomPosition(tiles);
tiles.emplace_back(Tile(2,row,col));
}
}

void SortTiles(std::vector<Tile>& tiles,std::function<bool (const Tile&, const Tile&)> sort_func)
{
std::sort(tiles.begin(),tiles.end(),sort_func);
}

void UpdateTiles(std::vector<Tile>& tiles, std::vector<Tile>& sorted_tiles)
{
    tiles.clear();
    for(auto& tile : sorted_tiles)
    {
        tiles.emplace_back(tile);
    }
    DrawMain(tiles);
}

std::string MoveTiles(std::vector<Tile>& tiles, Direction key, Uint32& frameStart, Uint32& frameTime)
{
bool update = true;
bool ceil = false;
std::vector<Tile> blocks;
int deltaX = 0;
int deltaY = 0;

    std::function<bool (const Tile&, const Tile&)> sort_func;
    std::function<bool(const Tile&)> boundary_check;
    std::function<Tile* (const Tile&)> get_next_tile;
    std::function<bool(const Tile&, const Tile&)> mergr_check;
    std::function<bool(const Tile&, const Tile&)> move_check;


    if( key == LEFT)
    {
        sort_func = [](const Tile& a, const Tile& b){return  a.col < b.col; }; // descending
        boundary_check = [](const Tile& tile){return tile.col == 0; };
        get_next_tile = [&](const Tile& tile)->Tile*
                            {
                                // tiles[std::to_string(tile.row)+std::to_string(tile.col-1)];
                                for(auto& t: tiles){
                                    if(t.key == (std::to_string(tile.row) + std::to_string(tile.col-1))) return &t;
                                }
                                return nullptr;
                            };
        mergr_check = [](const Tile& tile,const Tile& next_tile){return tile.x > (next_tile.x + MOVE_VEL);};
        move_check = [](const Tile& tile,const Tile& next_tile){return tile.x > (next_tile.x + RECT_WIDTH + MOVE_VEL);};
        ceil = true;
        deltaX = -MOVE_VEL;
        deltaY = 0;
    }
    else if( key == RIGHT)
    {
        sort_func = [](const Tile& a, const Tile& b){return  a.col > b.col; }; //
        boundary_check = [](const Tile& tile){return tile.col == (COLS - 1); };
        get_next_tile = [&](const Tile& tile)->Tile*
                            {
                                // tiles[std::to_string(tile.row)+std::to_string(tile.col-1)];
                                for(auto& t: tiles){
                                    if(t.key == (std::to_string(tile.row) + std::to_string(tile.col+1))) return &t;
                                }
                                return nullptr;
                            };
        mergr_check = [](const Tile& tile,const Tile& next_tile){return tile.x < (next_tile.x - MOVE_VEL);};
        move_check = [](const Tile& tile,const Tile& next_tile){return (tile.x + RECT_WIDTH + MOVE_VEL) < next_tile.x;};
        ceil = false;
        deltaX = MOVE_VEL;
        deltaY = 0;
    }
    else if(key == UP)
    {
        sort_func = [](const Tile& a, const Tile& b){return  a.row < b.row; }; //
        boundary_check = [](const Tile& tile){return tile.row == 0; };
        get_next_tile = [&](const Tile& tile)->Tile*
                            {
                                // tiles[std::to_string(tile.row)+std::to_string(tile.col-1)];
                                for(auto& t: tiles){
                                    if(t.key == (std::to_string(tile.row-1) + std::to_string(tile.col))) return &t;
                                }
                                return nullptr;
                            };
        mergr_check = [](const Tile& tile,const Tile& next_tile){return tile.y > (next_tile.y + MOVE_VEL);};
        move_check = [](const Tile& tile,const Tile& next_tile){return  tile.y > (next_tile.y + RECT_HIGHT + MOVE_VEL);};
        ceil = true;
        deltaX = 0;
        deltaY = -MOVE_VEL;
    }
    else if( key == DOWN)
    {
        sort_func = [](const Tile& a, const Tile& b){return  a.row > b.row; }; //
        boundary_check = [](const Tile& tile){return tile.row == (ROWS - 1); };
        get_next_tile = [&](const Tile& tile)->Tile*
                            {
                                // tiles[std::to_string(tile.row)+std::to_string(tile.col-1)];
                                for(auto& t: tiles){
                                    if(t.key == (std::to_string(tile.row+1) + std::to_string(tile.col))) return &t;
                                }
                                return nullptr;
                            };
        mergr_check = [](const Tile& tile,const Tile& next_tile){return tile.y < (next_tile.y - MOVE_VEL);};
        move_check = [](const Tile& tile,const Tile& next_tile){return (tile.y + RECT_HIGHT + MOVE_VEL) < next_tile.y;};
        ceil = false;
        deltaX = 0;
        deltaY = MOVE_VEL;
    }

    while(update)
    {
        // Calculate frame time and introduce a delay if necessary
        frameTime = SDL_GetTicks() - frameStart; // Time taken to render the frame
        if (FRAME_DELAY > frameTime) {
            SDL_Delay(FRAME_DELAY - frameTime); // Wait for the remaining time
        }

        update = false;
        std::vector<Tile> sorted_tiles = tiles;
        SortTiles(sorted_tiles, sort_func);
        int i = 0;

        for(auto it = sorted_tiles.begin(); it != sorted_tiles.end();)
        {
            Tile& tile = *it;
            if(boundary_check(tile)) {
                ++it;
                ++i;
                continue;
            }
            Tile* next_tile = get_next_tile(tile);
            if(!next_tile)
            {
                tile.move(deltaX,deltaY);
                ++it;
                ++i;
            }
            else if(tile.value == next_tile->value
                && std::find_if(blocks.begin(),blocks.end(),[tile](const Tile& t){return t.key == tile.key; }) == blocks.end()
                && std::find_if(blocks.begin(),blocks.end(),[next_tile](const Tile& t){return t.key == next_tile->key; }) == blocks.end())
            {
                if(mergr_check(tile, *next_tile)){
                    tile.move(deltaX, deltaY);
                    ++it;
                    ++i;
                }
                else{
                     auto original_tile_it = std::find_if(sorted_tiles.begin(), sorted_tiles.end(), [next_tile](const Tile& t) { return t.key == next_tile->key; });
                     if (original_tile_it != tiles.end())
                     {
                        original_tile_it->value *= 2; // Update the value in the original `tiles` vector
                     }
                     next_tile->value *= 2;
                     blocks.push_back(*next_tile);
                     sorted_tiles.erase(sorted_tiles.begin() + i);
                }
            }
            else if(move_check(tile, *next_tile))
            {
                tile.move(deltaX,deltaY);
                ++it;
                ++i;
            }
            else{
                ++it;
                ++i;
                continue;
            }
            tile.set_pos(ceil);
            update = true;
        }

        UpdateTiles(tiles, sorted_tiles);
    }

return EndMove(tiles);
}

int main()
{
window = nullptr;
renderer = nullptr;
font = nullptr;
textSurface = nullptr;
textTexture = nullptr;

    SDL_Init(SDL_INIT_VIDEO);
    if(!SDL_CreateWindowAndRenderer("2048", WIDTH, HEIGHT, 0, &window, &renderer)){
        std::cerr << "Window Creation Failed: " << std::endl;
        SDL_Quit();
        return -1;
    }

    if(!TTF_Init())
    {
        std::cerr << "SDL_ttf Init Failed: " << std::endl;
        SDL_Quit();
        return -1;
    }


    font = TTF_OpenFont("ComicSansMS.ttf", 50);
    if (!font) {
        std::cerr << "Failed to Load Font: " << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Event e;
    bool running = true;
    Direction dir = RIGHT;

    // Variables for FPS calculation
    Uint32 frameStart;
    Uint32 frameTime;

    std::vector<Tile> tiles;
    tiles.reserve(16);
    GenerateTiles(tiles);

    while(running)
    {
        frameStart = SDL_GetTicks(); // Start time of the frame
        // Check input
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_EVENT_QUIT) { running = false; }
            if(e.type == SDL_EVENT_KEY_DOWN)
            {
                if(e.key.key == SDLK_LEFT)
                {
                    MoveTiles(tiles, LEFT, frameStart, frameTime);
                }
                if(e.key.key == SDLK_RIGHT)
                {
                    MoveTiles(tiles, RIGHT, frameStart, frameTime);
                }
                if(e.key.key == SDLK_UP)
                {
                    MoveTiles(tiles, UP, frameStart, frameTime);
                }
                if(e.key.key == SDLK_DOWN)
                {
                    MoveTiles(tiles, DOWN, frameStart, frameTime);
                }
            }
        }

        DrawMain(tiles);

        // Calculate frame time and introduce a delay if necessary
        frameTime = SDL_GetTicks() - frameStart; // Time taken to render the frame
        if (FRAME_DELAY > frameTime) {
            SDL_Delay(FRAME_DELAY - frameTime); // Wait for the remaining time
        }
    }



    return 0;

}

/*_
in the above code, the SDL frame rate and Tile moving rate are fast. Some merges are not correct.
as example | 8 | 8 | 8 | 8 | move Right -> result | | | |16| , but correct result | | |16|16|
_*/
