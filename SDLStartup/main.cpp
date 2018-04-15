#include "pch.h"
#include "main.h"
#include <vector>


void mmain()
{
	CWindow wnd;

}
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 960;
//We'll just be using square tiles for now
const int TILE_SIZE = 80;

/**
 * \brief cleanup递归终止函数
 * \param window
 * \param render
 */
void cleanup(SDL_Window* window, SDL_Renderer* render)
{
	if (render != nullptr)
	{
		SDL_DestroyRenderer(render);
	}
	if (window != nullptr)
	{
		SDL_DestroyWindow(window);
	}	
}

/**
 * \brief 递归展开函数
 * \tparam T 
 * \tparam Args 
 * \param window 
 * \param render 
 * \param texture 
 * \param args 
 */
template<class T, class ...Args>
void cleanup(SDL_Window* window, SDL_Renderer* render, T* texture, Args... args)
{
	if (texture != nullptr)
	{
		SDL_DestroyTexture(texture);
	}
	cleanup(window, render, args...);
}


/**
 * \brief
 * \param window
 * \param renderer
 * \return 0:Succeed, 1:false
 */
int CreateWindowAndRenderer( SDL_Window **window,  SDL_Renderer **renderer)
{
	// Creating the Window and Renderer
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		logSDLError(std::cout, "SDL_Init");
		return 1;
	}
	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		logSDLError(std::cout, "IMG_Init");
		SDL_Quit();
		return 1;
	}

	*window = SDL_CreateWindow("Lesson 2", 100, 100, SCREEN_WIDTH,
		SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		logSDLError(std::cout, "CreateWindow");
		SDL_Quit();
		return 1;
	}
	*renderer = SDL_CreateRenderer(*window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		logSDLError(std::cout, "CreateRenderer");
		cleanup(*window, nullptr);
		SDL_Quit();
		return 1;
	}
	return 0;
}



/**
 * \brief 
 * \param hInstance 
 * \param hPrevInstance 
 * \param lpCmdLine 
 * \param nCmdShow 
 * \return 
 */
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	SDL_Window *window = nullptr;
	SDL_Renderer *renderer = nullptr;

	CreateWindowAndRenderer(&window, &renderer);

	// loading Texture
	SDL_Texture *background = loadTexture("res\\bg.bmp", renderer);
	SDL_Texture *wang = loadTexture("res\\1.bmp", renderer);

	if (background == nullptr || wang == nullptr) {
		cleanup(window, renderer, background, wang);
		SDL_Quit();
		return 1;
	}

	//Determine how many tiles we'll need to fill the screen
	int xTiles = SCREEN_WIDTH / TILE_SIZE;
	int yTiles = SCREEN_HEIGHT / TILE_SIZE;

	//Draw the tiles by calculating their positions
	for (int i = 0; i < xTiles * yTiles; ++i) {
		int x = i % xTiles;
		int y = i / xTiles;
		renderTexture(background, renderer, x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE,
			TILE_SIZE);
	}

	// Drawing the Foreground Image
	int iW, iH;
	SDL_QueryTexture(wang, NULL, NULL, &iW, &iH);
	int x = SCREEN_WIDTH / 2 - iW / 2;
	int y = SCREEN_HEIGHT / 2 - iH / 2;
	renderTexture(wang, renderer, x, y);

	SDL_RenderPresent(renderer);
	//SDL_Delay(10000);

	//Our event structure
	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_WaitEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
				break;
			}
			if (e.type == SDL_KEYDOWN) {
				quit = true;
				break;
			}
			if (e.type == SDL_MOUSEBUTTONDOWN) {
				quit = true;
				break;
			}
		}
		//Render the scene
		/*SDL_RenderClear(renderer);
		renderTexture(image, renderer, x, y);
		SDL_RenderPresent(renderer);*/
	}

	// cleanup
	cleanup(window, renderer, background, wang);
	SDL_Quit();

	return 0;
}
 