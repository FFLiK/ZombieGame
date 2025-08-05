#include "Texture.h"
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <cstdlib> 
#include "Global.h"
#include "Log.h"
#include "han2unicode.h"
using namespace std;

SDL_Texture* circle, * star;

#define IMAGE(name) ((Global::SYSTEM::USE_APPDATA ? (string)std::getenv("APPDATA") + "\\" + Global::SYSTEM::NAME + "\\" : (string)"") + Global::SYSTEM::RESOURCE_PATH + name + Global::SYSTEM::IMAGE_EXTENSION).c_str()
#define FONT(name) ((Global::SYSTEM::USE_APPDATA ? (string)std::getenv("APPDATA") + "\\" + Global::SYSTEM::NAME + "\\" : (string)"") + (string)Global::SYSTEM::RESOURCE_PATH + name + Global::SYSTEM::FONT_EXTENSION).c_str()

void InitLoadTextureLibrary(SDL_Renderer* renderer) {
	circle = LoadImage("circle", renderer);
	star = LoadImage("star", renderer);
	SDL_SetTextureBlendMode(circle, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(star, SDL_BLENDMODE_BLEND);
}

void QuitLoadTextureLibrary() {
	SDL_DestroyTexture(circle);
	SDL_DestroyTexture(star);
}


void DrawCircle(SDL_Point point, SDL_Renderer* renderer, int circle_size, int r, int g, int b, int a) {
	SDL_SetTextureColorMod(star, r, g, b);
	SDL_Rect dst;
	dst.w = circle_size;
	dst.h = circle_size;
	dst.x = point.x - circle_size / 2;
	dst.y = point.y - circle_size / 2;
	SDL_SetTextureBlendMode(star, SDL_BLENDMODE_ADD);
	SDL_SetTextureAlphaMod(star, a / 2);
	SDL_RenderCopy(renderer, star, NULL, &dst);
	SDL_SetTextureBlendMode(star, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(star, a);
	SDL_RenderCopy(renderer, star, NULL, &dst);
}

void DrawPoint(SDL_Point point, SDL_Renderer* renderer, int circle_size, int r, int g, int b, int a) {
	SDL_SetTextureColorMod(circle, r, g, b);
	SDL_Rect dst;
	dst.w = circle_size;
	dst.h = circle_size;
	dst.x = point.x - circle_size / 2;
	dst.y = point.y - circle_size / 2;
	SDL_SetTextureBlendMode(circle, SDL_BLENDMODE_ADD);
	SDL_SetTextureAlphaMod(circle, a / 2);
	SDL_RenderCopy(renderer, circle, NULL, &dst);
	SDL_SetTextureBlendMode(circle, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(circle, a);
	SDL_RenderCopy(renderer, circle, NULL, &dst);
}

void DrawLine(SDL_Point begin, SDL_Point end, SDL_Renderer* renderer, int circle_size, int r, int g, int b, int a) {
	SDL_SetTextureColorMod(circle, r, g, b);
	int addX, addY;
	int counter = 0;
	int dx = end.x - begin.x;
	int dy = end.y - begin.y;
	if (dx < 0) {
		addX = -1;
		dx = -dx;
	}
	else {
		addX = 1;
	}
	if (dy < 0) {
		addY = -1;
		dy = -dy;
	}
	else {
		addY = 1;
	}

	int x = begin.x;
	int y = begin.y;

	double n = sqrt(circle_size) / 2 + 1;
	addX *= n;
	addY *= n;

	if (dx >= dy) {
		for (int i = 0; i < dx; i += n) {
			x += addX;
			counter += dy;
			if (counter >= dx) {
				y += addY;
				counter -= dx;
			}
			SDL_Rect dst;
			dst.w = circle_size;
			dst.h = circle_size;
			dst.x = x - circle_size / 2;
			dst.y = y - circle_size / 2;
			SDL_SetTextureBlendMode(circle, SDL_BLENDMODE_ADD);
			SDL_SetTextureAlphaMod(circle, a / 2);
			SDL_RenderCopy(renderer, circle, NULL, &dst);
			SDL_SetTextureBlendMode(circle, SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(circle, a);
			SDL_RenderCopy(renderer, circle, NULL, &dst);
		}
	}
	else {
		for (int i = 0; i < dy; i += n) {
			y += addY;
			counter += dx;
			if (counter >= dy) {
				x += addX;
				counter -= dy;
			}
			SDL_Rect dst;
			dst.w = circle_size;
			dst.h = circle_size;
			dst.x = x - circle_size / 2;
			dst.y = y - circle_size / 2;
			SDL_SetTextureBlendMode(circle, SDL_BLENDMODE_ADD);
			SDL_SetTextureAlphaMod(circle, a / 2);
			SDL_RenderCopy(renderer, circle, NULL, &dst);
			SDL_SetTextureBlendMode(circle, SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(circle, a);
			SDL_RenderCopy(renderer, circle, NULL, &dst);
		}
	}
}

SDL_Texture* LoadImage(const char* filename, SDL_Renderer* renderer) {
	SDL_Surface* surface;
	SDL_Texture* texture;
	surface = IMG_Load(IMAGE(filename));
	if (!surface) {
		Log::FormattedDebug("Texture", "LoadImage", IMG_GetError());
		return nullptr;
	} 
	else {
		Log::FormattedDebug("Texture", "LoadImage", "Loaded image: " + string(filename));
	}
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}

SDL_Texture* LoadText(const char* str, SDL_Renderer* renderer, int size, const char* fontfile_name, int r, int g, int b) {
	TTF_Font* font;
	SDL_Surface* surface;
	SDL_Texture* texture;
	font = TTF_OpenFont(FONT(fontfile_name), size);

	size = Global::WIN::SCREEN_WIDTH * size / 1280.0;

	SDL_Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = 255;

	Uint16 unicode[32768];
	han2unicode(str, unicode);

	surface = TTF_RenderUNICODE_Blended(font, unicode, color);

	texture = SDL_CreateTextureFromSurface(renderer, surface);

	if (!texture) {
		Log::FormattedDebug("Texture", "LoadText", SDL_GetError());
		return nullptr;
	}
	else {
		Log::FormattedDebug("Texture", "LoadText", "Loaded text: " + string(str));
	}

	SDL_FreeSurface(surface);

	//delete font;
	TTF_CloseFont(font);
	return texture;
}