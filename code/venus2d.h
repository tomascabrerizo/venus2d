/* date = February 22nd 2021 6:06 pm */

#ifndef VENUS2D_H
#define VENUS2D_H

#include <assert.h>
#include "venus2d_math.h"

//NOTE(tomi) Memory DEFINES:
#define KYLOBYTES(N) (N*1024LL)
#define MEGABYTES(N) (N*KYLOBYTES(1024LL))
#define GIGABYTES(N) (N*MEGABYTES(1024LL))

//TODO(tomi) Dont hardcode keyboard in this MACRO
//NOTE(tomi) Keyboard DEFINES:
#define IS_KEY_PRESS(key) (keyboard->keys[KEY_##key].is_down)
#define IS_KEY_RELEASE(key) (!keyboard->keys[KEY_##key].is_down && keyboard->keys[KEY_##key].change)
#define IS_KEY_PRESS_ONES(key) (keyboard->keys[KEY_##key].is_down && keyboard->keys[KEY_##key].change)

#define push_struct(arena, type) \
{\
(type*)push_arena_(arena, sizeof(type))\
}\

struct Memory
{
    void* buffer;
    uint64_t used;
    uint64_t max_size;
    bool initialize;
    
    //NOTE(tomi) Debug flag
    float current_size_in_mb;
};

struct Arena
{
    uint8_t* base;
    uint64_t size;
    uint64_t used;
};

inline
void init_arena(Arena* arena, Memory* memory, uint64_t size)
{
    assert(memory->used + size <= memory->max_size);
    arena->base = (uint8_t*)memory->buffer + memory->used;
    memory->used += size;
    arena->size = size;
    arena->used = 0;
}

inline
void* push_arena_(Arena* arena, uint64_t size)
{
    assert(arena->used + size <= arena->size);
    void* result = ((uint8_t*)arena->base) + arena->used;
    arena->used += size;
    
    return result;
}

struct Backbuffer
{
    void* memory;
    int height;
    int width;
    int pitch;
    int bytes_per_pixels;
};

struct Keys
{
    bool is_down;
    bool change;
};

enum Key_Types
{
    KEY_DOWN,
    KEY_UP,
    KEY_RIGHT,
    KEY_LEFT,
    
    KEYS_COUNT,
};

struct Input
{
    Keys keys[KEYS_COUNT];
};

#pragma pack(push, 1)
struct Bitmap_Header
{
    uint16_t file_type;
    uint32_t file_size;
    uint32_t reserved;
    uint32_t offset_to_pixels;
    uint32_t header_size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t size_of_bitmap;
    int32_t h_res;
    int32_t v_res;
    uint32_t colors_used;
    uint32_t colors_important;
    uint32_t red_mask;
    uint32_t green_mask;
    uint32_t blue_mask;
    
};
#pragma pack(pop)

struct Bitmap
{
    uint32_t* pixels;
    int width;
    int height;
};

struct Game_State
{
    Arena MapMemory;
    
    v2 player_p;
    v2 player_dp;
    v2 player_size;
    
    v2 camera_p;
    v2 camera_dp;
    v2 camera_size;
};

//NOTE Platform to game functios
internal void* read_file(Memory* memory, Arena* arena, const char* path);
internal void* read_file(const char* path);

//NOTE Game functions
internal 
void game_update_and_render(Backbuffer* buffer, Input* keyboard, Memory* memory, float dt);

#endif //VENUS2D_H
