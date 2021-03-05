#include "venus2d.h"

internal
Bitmap load_bitmap(Memory* memory, Arena* arena, const char* path)
{
    Bitmap_Header* header = (Bitmap_Header*)read_file(memory, arena, path);
    Bitmap result = {};
    if(header)
    {
        result.width = header->width;
        result.height = header->height;
        result.pixels = (uint32_t*)((uint8_t*)header + header->offset_to_pixels);
    }
    return result;
}


internal
Bitmap load_bitmap(const char* path)
{
    Bitmap_Header* header = (Bitmap_Header*)read_file(path);
    Bitmap result = {};
    if(header)
    {
        result.width = header->width;
        result.height = header->height;
        result.pixels = (uint32_t*)((uint8_t*)header + header->offset_to_pixels);
    }
    return result;
}

internal
void draw_pixel(Backbuffer* backbuffer, int x, int y, uint32_t color)
{
    if(x < 0 || x >= backbuffer->width || y < 0 || y >= backbuffer->height) return;
    ((uint32_t*)backbuffer->memory)[y*backbuffer->width+x] = color;
}

internal
uint32_t get_pixel(Backbuffer* backbuffer, int x, int y)
{
    if(x < 0 || x >= backbuffer->width || y < 0 || y >= backbuffer->height) return 0;
    return ((uint32_t*)backbuffer->memory)[y*backbuffer->width+x];
}

internal 
void clear_backbuffer(Backbuffer* backbuffer, uint32_t color)
{
    for(int y = 0; y < backbuffer->height; ++y)
    {
        for(int x = 0; x < backbuffer->width; ++x)
        {
            draw_pixel(backbuffer, x, y, color);
        }
    }
}

internal
void draw_bitmap(Backbuffer* backbuffer, Bitmap bitmap, int x, int y)
{
    int min_x = x;
    int max_x = x + bitmap.width;
    int min_y = y;
    int max_y = y + bitmap.height;
    
    int bitmap_y = bitmap.height-1;
    for(int y = min_y; y < max_y; ++y)
    {
        int bitmap_x = 0;
        for(int x = min_x; x < max_x; ++x)
        {
            uint32_t color = bitmap.pixels[bitmap_y*bitmap.width+bitmap_x];
            draw_pixel(backbuffer, x, y, color);
            bitmap_x++;
        }
        bitmap_y--;
    }
}

internal
void draw_bitmap_alpha(Backbuffer* backbuffer, Bitmap bitmap, int x, int y)
{
    int min_x = x;
    int max_x = x + bitmap.width;
    int min_y = y;
    int max_y = y + bitmap.height;
    
    int bitmap_y = bitmap.height-1;
    for(int y = min_y; y < max_y; ++y)
    {
        int bitmap_x = 0;
        for(int x = min_x; x < max_x; ++x)
        {
            uint32_t bitmap_color = bitmap.pixels[bitmap_y*bitmap.width+bitmap_x];
            
            //TODO(tomi) Premultiply alpha
            float src_a = ((bitmap_color  >> 24) & 0xFF) / 255.0f;
            uint8_t src_r = (bitmap_color >> 16) & 0xFF;
            uint8_t src_g = (bitmap_color >>  8) & 0xFF;
            uint8_t src_b = (bitmap_color >>  0) & 0xFF;
            
            uint32_t buffer_color = get_pixel(backbuffer, x, y);
            uint8_t des_r = (buffer_color >> 16) & 0xFF;
            uint8_t des_g = (buffer_color >>  8) & 0xFF;
            uint8_t des_b = (buffer_color >>  0) & 0xFF;
            //TODO(tomi) Create lerp function
            uint32_t color =
                (uint8_t)(src_r*src_a + des_r*(1-src_a)) << 16 |
                (uint8_t)(src_g*src_a + des_g*(1-src_a)) << 8 |
                (uint8_t)(src_b*src_a + des_b*(1-src_a)) << 0;
            
            draw_pixel(backbuffer, x, y, color);
            bitmap_x++;
        }
        bitmap_y--;
    }
}

internal
void draw_rect_alpha(Backbuffer* backbuffer, v2 pos, v2 dim, uint32_t color)
{
    int min_y = pos.y;
    int max_y = pos.y + dim.y;
    int min_x = pos.x;
    int max_x = pos.x + dim.x;
    
    for(int y = min_y; y < max_y; ++y)
    {
        for(int x = min_x; x < max_x; ++x)
        {
            //TODO(tomi) Premultiply alpha
            float src_a = ((color  >> 24) & 0xFF) / 255.0f;
            uint8_t src_r = (color >> 16) & 0xFF;
            uint8_t src_g = (color >>  8) & 0xFF;
            uint8_t src_b = (color >>  0) & 0xFF;
            
            uint32_t buffer_color = get_pixel(backbuffer, x, y);
            uint8_t des_r = (buffer_color >> 16) & 0xFF;
            uint8_t des_g = (buffer_color >>  8) & 0xFF;
            uint8_t des_b = (buffer_color >>  0) & 0xFF;
            //TODO(tomi) Create lerp function
            uint32_t color =
                (uint8_t)(src_r*src_a + des_r*(1-src_a)) << 16 |
                (uint8_t)(src_g*src_a + des_g*(1-src_a)) << 8 |
                (uint8_t)(src_b*src_a + des_b*(1-src_a)) << 0;
            
            draw_pixel(backbuffer, x, y, color);
        }
    }
}

internal
void draw_rect(Backbuffer* backbuffer, v2 pos, v2 dim, uint32_t color)
{
    int min_y = pos.y;
    int max_y = pos.y + dim.y;
    
    int min_x = pos.x;
    int max_x = pos.x + dim.x;
    
    for(int y = min_y; y < max_y; ++y)
    {
        for(int x = min_x; x < max_x; ++x)
        {
            draw_pixel(backbuffer, x, y, color);
        }
    }
}

internal
void draw_rect(Backbuffer* backbuffer, int min_x, int min_y, int max_x, int max_y, uint32_t color)
{
    for(int y = min_y; y < max_y; ++y)
    {
        for(int x = min_x; x < max_x; ++x)
        {
            draw_pixel(backbuffer, x, y, color);
        }
    }
}

struct Basis
{
    v2 origin;
    v2 x_axis;
    v2 y_axis;
};

internal
void draw_basis(Backbuffer* backbuffer, Basis base, uint32_t color)
{
    int rec_size = 10;
    v2 origin = base.origin;
    v2 x_axis = base.origin + base.x_axis;
    v2 y_axis = base.origin + base.y_axis;
    draw_rect(backbuffer, origin.x, origin.y, origin.x+rec_size, origin.y+rec_size, color);
    draw_rect(backbuffer, x_axis.x, x_axis.y, x_axis.x+rec_size, x_axis.y+rec_size, color);
    draw_rect(backbuffer, y_axis.x, y_axis.y, y_axis.x+rec_size, y_axis.y+rec_size, color);
    
}

internal
void draw_rect_basis(Backbuffer* backbuffer, Basis base, v2 pos, v2 dim, uint32_t color)
{
    v2 wp = base.origin + base.x_axis * pos.x + base.y_axis * pos.y;
    draw_rect(backbuffer, wp.x, wp.y, wp.x+dim.x, wp.y+dim.y, color);
}


internal
void draw_rect_slow(Backbuffer* backbuffer, v2 pos, v2 dim, Bitmap bitmap)
{
    static float a = 0;
    Basis base = {};
    base.x_axis = new_v2(cosf(a), sinf(a));
    base.y_axis = v2_perp(base.x_axis);
    base.x_axis *= dim.x;
    base.y_axis *= dim.y;
    base.origin = pos - base.x_axis/2 - base.y_axis/2;
    a += 0.0;
    
    
    v2 perp_x = v2_normalize(v2_perp(base.x_axis));
    v2 perp_y = v2_normalize(v2_perp(base.y_axis));
    
    v2 A = base.origin;
    v2 B = base.origin + base.x_axis;
    v2 C = base.origin + base.y_axis;
    v2 D = base.origin + base.x_axis + base.y_axis;
    
    int min_x = fminf(fminf(A.x, B.x), fminf(C.x, D.x));
    int max_x = fmaxf(fmaxf(A.x, B.x), fmaxf(C.x, D.x));
    int min_y = fminf(fminf(A.y, B.y), fminf(C.y, D.y));
    int max_y = fmaxf(fmaxf(A.y, B.y), fmaxf(C.y, D.y));
    
    for(int y = min_y; y < max_y; ++y)
    {
        for(int x = min_x; x < max_x; ++x)
        {
            v2 p = (new_v2(x, y) - base.origin);
            float edge_l = v2_dot(p, perp_y);
            float edge_b = v2_dot(p-base.x_axis, -perp_x);
            float edge_r = v2_dot(p-base.x_axis-base.y_axis, -perp_y);
            float edge_t = v2_dot(p-base.y_axis, perp_x);
            
            if(edge_b <= 0 && edge_r <= 0 && edge_t <= 0 && edge_l <= 0)
            {
                float u = v2_dot(p, v2_normalize(base.x_axis)) / (float)dim.x;
                float v = v2_dot(p, v2_normalize(base.y_axis)) / (float)dim.y;
                
                u *= (bitmap.width - 1);
                v *= (bitmap.height - 1);
                
                uint32_t bitmap_color = bitmap.pixels[(bitmap.height-(int)v)*bitmap.width+(int)u];
                
                //TODO(tomi) Premultiply alpha
                float src_a = ((bitmap_color  >> 24) & 0xFF) / 255.0f;
                uint8_t src_r = (bitmap_color >> 16) & 0xFF;
                uint8_t src_g = (bitmap_color >>  8) & 0xFF;
                uint8_t src_b = (bitmap_color >>  0) & 0xFF;
                
                uint32_t buffer_color = get_pixel(backbuffer, x, y);
                uint8_t des_r = (buffer_color >> 16) & 0xFF;
                uint8_t des_g = (buffer_color >>  8) & 0xFF;
                uint8_t des_b = (buffer_color >>  0) & 0xFF;
                
                //TODO(tomi) Create lerp function
                uint32_t color = 
                    (uint8_t)(src_r*src_a + des_r*(1-src_a)) << 16 |
                    (uint8_t)(src_g*src_a + des_g*(1-src_a)) << 8 |
                    (uint8_t)(src_b*src_a + des_b*(1-src_a)) << 0;
                
                draw_pixel(backbuffer, x, y, color);
            }
            else
            {
                //draw_pixel(backbuffer, x, y, 0xFFFF00FF);
            }
        }
    }
    
    draw_basis(backbuffer, base, 0xFF00FF00);
}

#define MAP_SIZE_X 16
#define MAP_SIZE_Y 16
#define MAP_SIZE (MAP_SIZE_Y*MAP_SIZE_X)
#define TILE_SIZE 100
#define TILE_COLOR0 0xFFCFDBD5
#define TILE_COLOR1 0xFFE8EDDF

#define FLOOR_COLOR 0XFF333533

#define PLAYER_SIZE 60
#define PLAYER_COLOR 0xFFF5CB5C

#define CAMERA_COLOR 0xFF242423

inline 
v2 world_to_screen(v2 pos)
{
    return (pos + new_v2(BACKBUFFER_WIDTH/2, BACKBUFFER_HEIGHT/2));
}

internal
void game_update_and_render(Backbuffer* buffer, Input* keyboard, Memory* memory, float dt)
{
    
    assert(sizeof(Game_State) < memory->max_size);
    
    //TODO(tomi) Create a method to push stuff into 
    Game_State* gs  = (Game_State*)memory->buffer;
    
    if(!memory->initialize)
    {
        memory->initialize = true;
        memory->used = sizeof(Game_State);
        
        //NOTE(tomi) Initiallize game 
        gs->player_dp = {};
        gs->player_size = new_v2(PLAYER_SIZE, PLAYER_SIZE);
        gs->player_p = new_v2(620,150);
        
        gs->camera_p = new_v2(200, 200);
        gs->camera_size = new_v2(BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT);
        
        //NOTE(tomi) generate grid tile map
        uint32_t color0 = TILE_COLOR0;
        uint32_t color1 = TILE_COLOR1;
        uint32_t map_size_in_bytes =  MAP_SIZE*sizeof(uint32_t);
        init_arena(&gs->MapMemory, memory, map_size_in_bytes);
        
        uint32_t tile_map[MAP_SIZE] = 
        {
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1,
            1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        };
        
        bool start_color = false;
        for(int y = 0; y < MAP_SIZE_Y; ++y)
        {
            uint32_t color = start_color == true ? color0 : color1;
            bool change_color = false;
            for(int x = 0; x < MAP_SIZE_X; ++x)
            {
                if(color == color0) color = color1;
                else if(color == color1) color = color0;
                uint32_t* map_color push_struct(&gs->MapMemory, uint32_t);
                if(tile_map[y*MAP_SIZE_X+x] == 1)
                {
                    *map_color = 1;
                }
                else
                {
                    *map_color = color;
                }
            }
            start_color = !start_color;
        }
        
        
    }
    
    float speed = 40;
    if(IS_KEY_PRESS(LEFT))
    {
        gs->player_dp += new_v2(-1.0f, 0.0f) * speed;
    }
    if(IS_KEY_PRESS(RIGHT))
    {
        gs->player_dp += new_v2(1.0f, 0.0f) * speed;
    }
    if(IS_KEY_PRESS(UP))
    {
        gs->player_dp += new_v2(0.0f, -1.0f) * speed;
    }
    if(IS_KEY_PRESS(DOWN))
    {
        gs->player_dp += new_v2(0.0f, 1.0f) * speed;
    };
    
    //NOTE(tomi) Calculate player position
    v2 player_half = gs->player_size * 0.5f;
    
    v2 gravity = new_v2(0.0f, 14.0f);
    gs->player_dp += gravity;
    gs->player_dp += -gs->player_dp * 0.04f;
    
    //NOTE(tomi) Make all calculations with player_p at center
    v2 player_p = gs->player_p + player_half;
    v2 test_player_p = player_p + gs->player_dp * dt;
    
    int min_tile_to_test_x = (fminf(player_p.x-player_half.x, test_player_p.x) / TILE_SIZE) - 1;
    int max_tile_to_test_x = (fmaxf(player_p.x+player_half.x, test_player_p.x) / TILE_SIZE) + 1;
    int min_tile_to_test_y = (fminf(player_p.y-player_half.y, test_player_p.y) / TILE_SIZE) - 1;
    int max_tile_to_test_y = (fmaxf(player_p.y+player_half.x, test_player_p.y) / TILE_SIZE) + 1;
    
    v2 wall_normal = {};
    v2 player_delta = test_player_p - player_p;
    float t_min = 1.0f;
    for(int y = min_tile_to_test_y; y <= max_tile_to_test_y; ++y)
    {
        for(int x = min_tile_to_test_x; x <= max_tile_to_test_x; ++x)
        {
            uint32_t color = *(((uint32_t*)gs->MapMemory.base) + (y * MAP_SIZE_X) + x);
            if(color == 1)
            {
                v2 p0 = (new_v2(x, y) * TILE_SIZE);
                p0 -= player_half; 
                v2 p1 = (new_v2(x, y) * TILE_SIZE) + new_v2(TILE_SIZE, TILE_SIZE);
                p1 += player_half;
                
                if(player_delta.x != 0)
                {
                    float t_result = (p0.x - player_p.x) / player_delta.x;
                    float y = player_p.y + t_result * player_delta.y;
                    if(y >= p0.y && y <= p1.y)
                    {
                        if(t_result < t_min && t_result >= 0) 
                        {
                            t_min = t_result;
                            wall_normal = new_v2(-1, 0);
                        };
                    }
                    
                    t_result = (p1.x - player_p.x) / player_delta.x;
                    y = player_p.y + t_result * player_delta.y;
                    if(y >= p0.y && y <= p1.y)
                    {
                        if(t_result < t_min && t_result >= 0)
                        {
                            t_min = t_result;
                            wall_normal = new_v2(1, 0);
                        }
                    }
                }
                
                if(player_delta.y != 0)
                {
                    float t_result = (p0.y - player_p.y) / player_delta.y;
                    float x = player_p.x + t_result * player_delta.x;
                    if(x >= p0.x && x <= p1.x)
                    {
                        if(t_result < t_min && t_result >= 0) 
                        {
                            t_min = t_result;
                            wall_normal = new_v2(0, -1);
                        };
                    }
                    
                    t_result =  (p1.y - player_p.y) / player_delta.y;
                    x = player_p.x + t_result * player_delta.x;
                    if(x >= p0.x && x <= p1.x)
                    {
                        if(t_result < t_min && t_result >= 0)
                        {
                            t_min = t_result;
                            wall_normal = new_v2(0, -1);
                        }
                    }
                }
                
            }
        }
    }
    
    gs->player_dp = gs->player_dp - 1*v2_dot(gs->player_dp, wall_normal)*wall_normal;
    gs->player_p += (0.3f)*t_min*gs->player_dp * dt;
    
    //NOTE(tomi) calculate camera position
    gs->camera_dp = (player_p - gs->camera_p) * 0.07f;
    gs->camera_p += gs->camera_dp;
    
    clear_backbuffer(buffer, 0xFF222222);
    
    v2 camera_offset = gs->camera_size + new_v2(400, 400);
    v2 camera_min = (gs->camera_p - (camera_offset / 2.0f)) / (float)TILE_SIZE;
    v2 camera_max = (gs->camera_p + (camera_offset / 2.0f)) / (float)TILE_SIZE;
    
    if(camera_min.x < 0) camera_min.x = 0.0f;
    if(camera_min.y < 0) camera_min.y = 0.0f;
    if(camera_max.x > MAP_SIZE_X) camera_max.x = MAP_SIZE_X;
    if(camera_max.y > MAP_SIZE_Y) camera_max.y = MAP_SIZE_Y;
    
    //NOTE(tomi) Draw map here!
    for(int y = camera_min.y; y < camera_max.y; ++y)
    {
        for(int x = camera_min.x; x < camera_max.x; ++x)
        {
            uint32_t color = *(((uint32_t*)gs->MapMemory.base) + (y * MAP_SIZE_X) + x);
            v2 tile_pos = world_to_screen(new_v2(x, y) * TILE_SIZE - gs->camera_p);
            //tile_pos -= new_v2(TILE_SIZE/2, TILE_SIZE/2);
            if(color == 1)
            {
                color = FLOOR_COLOR;
            }
            draw_rect(buffer, tile_pos, new_v2(TILE_SIZE, TILE_SIZE), color);
        }
    }
    //-------------------------
    
    
    v2 player_pos = gs->player_p;// - player_half;
    draw_rect(buffer, world_to_screen(player_pos - gs->camera_p), gs->player_size, PLAYER_COLOR);
    
    v2 camera_pos_h = new_v2(0.0f, 0.0f) - new_v2(5.0f/2.0f, 30.0f/2.0f);
    v2 camera_pos_v = new_v2(0.0f, 0.0f) - new_v2(30.0f/2.0f, 5.0f/2.0f);
    draw_rect(buffer, world_to_screen(camera_pos_h), new_v2(5, 30), CAMERA_COLOR);
    draw_rect(buffer, world_to_screen(camera_pos_v), new_v2(30, 5), CAMERA_COLOR);
    
    //NOTE(tomi) Debug memoryy flag
    memory->current_size_in_mb = (float)memory->used / (float)MEGABYTES(1);
}