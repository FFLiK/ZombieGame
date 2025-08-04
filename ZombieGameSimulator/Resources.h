#pragma once
#include <SDL.h>
class Resources {
public:
	static SDL_Texture* background;

	static SDL_Texture* tile_normal;
	static SDL_Texture* tile_papal;
	static SDL_Texture* tile_teleport;
	static SDL_Texture* tile_event;
	static SDL_Texture* tile_obstacle;

	static SDL_Texture* player_human[6];
	static SDL_Texture* player_zombie[6];
	static SDL_Texture* player_super_zombie;

	static void InitResources(SDL_Renderer* renderer);
	static void QuitResources();

private:
	static bool initialized;
};

