#pragma once
#include <SDL.h>
#undef LoadImage

void DrawCircle(SDL_Point point, SDL_Renderer* renderer, int circle_size = 15, int r = 255, int g = 255, int b = 255, int a = 255);
void DrawPoint(SDL_Point point, SDL_Renderer* renderer, int circle_size = 15, int r = 255, int g = 255, int b = 255, int a = 255);
void DrawLine(SDL_Point begin, SDL_Point end, SDL_Renderer* renderer, int circle_size = 15, int r = 255, int g = 255, int b = 255, int a = 255);
SDL_Texture* LoadImage(const char* filename, SDL_Renderer* renderer);
SDL_Texture* LoadText(const char* str, SDL_Renderer* renderer, int size, const char* fontfile_name, int r, int g, int b, int align = 0, int spacing = 4);

void InitLoadTextureLibrary(SDL_Renderer* renderer);
void QuitLoadTextureLibrary();