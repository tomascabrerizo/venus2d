/* date = February 22nd 2021 6:03 pm */

#ifndef WIN32_VENUS2D_H
#define WIN32_VENUS2D_H

#define global static
#define internal static

#define WINDOW_WIDTH 1280 
#define WINDOW_HEIGHT 720
#define BACKBUFFER_WIDTH 960 
#define BACKBUFFER_HEIGHT 540 

#define array_count(array) (sizeof(array)/sizeof(array[0]))

#include "venus2d.h"
#include "venus2d.cpp"

struct Win32_Backbuffer
{
    BITMAPINFO bitmap_info;
    HBITMAP bitmap;
    void* memory;
    int width;
    int height;
    int pitch;
    int bytes_per_pixels;
};

//NOTE(tomi): Global variables
global bool quit = false;
global Win32_Backbuffer backbuffer;

#endif //WIN32_VENUS2D_H
