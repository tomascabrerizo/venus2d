#include <Windows.h>
#include <stdint.h>

#include "win32_venus2d.h"

internal
void* read_file(Memory* memory, Arena* arena, const char* path)
{
    HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    LARGE_INTEGER file_size;
    if(GetFileSizeEx(file, &file_size))
    {
        //TODO(tomi) Alloc read files in game memory
        init_arena(arena, memory, file_size.QuadPart);
        void* buffer = push_arena_(arena, file_size.QuadPart);
        if(!ReadFile(file, buffer, file_size.QuadPart, 0, 0))
        {
            OutputDebugString("Error allocating BMP in Arena\n");
            return 0;
        }
        return buffer;
    }
    OutputDebugString("Error reading file\n");
    return 0;
}

internal
void* read_file(const char* path)
{
    HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    LARGE_INTEGER file_size;
    if(GetFileSizeEx(file, &file_size))
    {
        //TODO(tomi) Alloc read files in game memory
        void* buffer = VirtualAlloc(0, file_size.QuadPart, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
        if(!ReadFile(file, buffer, file_size.QuadPart, 0, 0))
        {
            VirtualFree(buffer, 0, MEM_RELEASE);
            return 0;
        }
        return buffer;
    }
    OutputDebugString("Error reading file\n");
    return 0;
}

internal
void init_dibsection(HDC device, Win32_Backbuffer* backbuffer, int width, int height)
{
    if(backbuffer->memory)
    {
        VirtualFree(backbuffer->memory, 0, MEM_RELEASE);
    }
    
    backbuffer->width = width;
    backbuffer->height = height;
    backbuffer->bytes_per_pixels = 4;
    backbuffer->pitch = backbuffer->width * backbuffer->bytes_per_pixels;
    
    backbuffer->bitmap_info = {};
    backbuffer->bitmap_info.bmiHeader.biSize = sizeof(backbuffer->bitmap_info);
    backbuffer->bitmap_info.bmiHeader.biWidth = backbuffer->width;
    backbuffer->bitmap_info.bmiHeader.biHeight = -backbuffer->height;
    backbuffer->bitmap_info.bmiHeader.biPlanes = 1;
    backbuffer->bitmap_info.bmiHeader.biBitCount = 32;
    backbuffer->bitmap_info.bmiHeader.biCompression = BI_RGB;
    backbuffer->bitmap_info.bmiHeader.biSizeImage = 0;
    
    backbuffer->bitmap = {};
    backbuffer->bitmap = CreateDIBSection(device, &backbuffer->bitmap_info, DIB_RGB_COLORS, &backbuffer->memory, 0, 0);
}

LRESULT callback_window_proc(HWND window, UINT msg, WPARAM w_param, LPARAM l_param)
{
    switch(msg)
    {
        case WM_CLOSE:
        {
            quit = true;
        }break;
        case WM_CREATE:
        {
            HDC device = GetDC(window);
            init_dibsection(device, &backbuffer, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT);
        }break;
        case WM_SIZE:
        {
        }break;
    }
    return DefWindowProcA(window, msg, w_param, l_param);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    OutputDebugString("Welcome to 2dengine!\n");
    
    WNDCLASSA window_class = {};
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = callback_window_proc;
    window_class.hInstance = hInstance;
    window_class.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    window_class.lpszClassName = "2dengine_window";
    
    HWND window = 0;
    if(RegisterClassA(&window_class))
    {
        window = CreateWindowA(
                               window_class.lpszClassName,
                               "2dengine",
                               WS_OVERLAPPEDWINDOW  | WS_VISIBLE,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               WINDOW_WIDTH, WINDOW_HEIGHT, 
                               0,0,
                               hInstance,
                               0 
                               );
    }
    
    HDC device = GetDC(window); 
    HDC src_device = CreateCompatibleDC(device);
    Input keyboard;
    
    //NOTE(tomi) Alloc memory for the entired game
    Memory memory = {};
    memory.max_size =  MEGABYTES(512);
    memory.used = 0;
    memory.buffer = VirtualAlloc(0, memory.max_size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    memory.initialize = false;
    
    while(!quit)
    {
        for(int i = 0; i < array_count(keyboard.keys); ++i)
        {
            keyboard.keys[i].change = false;
        }
        
        MSG msg;
        while(PeekMessageA(&msg, window, 0, 0, PM_REMOVE))
        {
            switch(msg.message)
            {
                case WM_SYSKEYUP:
                case WM_SYSKEYDOWN:
                case WM_KEYDOWN:
                case WM_KEYUP:
                {
                    int vk_code  = (int)msg.wParam;
                    bool was_down = (msg.lParam & (1 << 30)) != 0;
                    bool is_down = (msg.lParam & (1 << 31)) == 0;
                    
#define PROCESS_KEY(CODE)\
if(vk_code == VK_##CODE)\
{\
keyboard.keys[KEY_##CODE].change = is_down != keyboard.keys[KEY_##CODE].is_down;\
keyboard.keys[KEY_##CODE].is_down = is_down;\
}\
                    
                    PROCESS_KEY(UP);
                    PROCESS_KEY(DOWN);
                    PROCESS_KEY(LEFT);
                    PROCESS_KEY(RIGHT);
                    
#undef PROCESS_KEY
                    
                }break;
                default:
                {
                    TranslateMessage(&msg); 
                    DispatchMessage(&msg);
                }break;
            }
            
        }
        
        Backbuffer buffer = {};
        buffer.memory = backbuffer.memory;
        buffer.width = backbuffer.width;
        buffer.height = backbuffer.height;
        buffer.pitch = backbuffer.pitch;
        buffer.bytes_per_pixels = backbuffer.bytes_per_pixels;
        
        game_update_and_render(&buffer, &keyboard,&memory, 0.016f);
        
        SelectObject(src_device, backbuffer.bitmap);
        BitBlt(device, 20, 20, backbuffer.width, backbuffer.height, src_device, 0, 0, SRCCOPY);
    }
    
    return 0;
}
