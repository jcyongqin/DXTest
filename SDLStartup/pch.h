// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// wrl atl 
#include <wrl.h>
#include <atlbase.h>
#include <atlwin.h>

// d2d d3d
#include <d2d1_1.h>
#include <d3d11_1.h>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "d3d11")

// sdl2 sdl_image
#include "SDL.h"
#include <SDL_image.h>

#pragma comment(lib,"SDL2.lib")
#pragma comment(lib,"SDL2main.lib")
#pragma comment(lib,"SDL2_image.lib")

// TODO: 在此处引用程序需要的其他头文件