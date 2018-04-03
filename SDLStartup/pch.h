// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>

// C ����ʱͷ�ļ�
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

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�