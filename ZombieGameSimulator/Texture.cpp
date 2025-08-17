#include "Texture.h"
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <cstdlib> 
#include <cmath>
#include <vector>
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
	std::filesystem::path p = IMAGE(filename);
	std::string pUtf8 = p.u8string();
	surface = IMG_Load(pUtf8.c_str());
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
	if (!renderer || !str || !fontfile_name) {
		Log::FormattedDebug("Texture", "LoadText", "Invalid parameter(s).");
		return nullptr;
	}

	const int scaledSize = size * Global::WIN::SIZE_MULTIPLIER;
	std::filesystem::path p = FONT(fontfile_name);
	std::string pUtf8 = p.u8string();
	TTF_Font* font = TTF_OpenFont(pUtf8.c_str(), scaledSize);
	if (!font) {
		Log::FormattedDebug("Texture", "LoadText", std::string("Failed to open font: ") + TTF_GetError());
		return nullptr;
	}

	SDL_Color color;
	color.r = static_cast<Uint8>(r);
	color.g = static_cast<Uint8>(g);
	color.b = static_cast<Uint8>(b);
	color.a = 255;

	std::vector<std::string> lines;
	{
		const char* p = str;
		const char* lineStart = p;
		for (; *p; ++p) {
			if (*p == '\n') {
				lines.emplace_back(lineStart, p - lineStart);
				lineStart = p + 1;
			}
		}
		lines.emplace_back(lineStart, p - lineStart);
		for (auto& ln : lines) {
			if (!ln.empty() && ln.back() == '\r') ln.pop_back();
		}
	}

	struct LineItem {
		SDL_Surface* surf{ nullptr };
		int w{ 0 };
		int h{ 0 };
	};

	std::vector<LineItem> items;
	items.reserve(lines.size());

	int max_w = 0;
	int total_h = 0;
	const int lineSkip = TTF_FontLineSkip(font);

	for (const auto& ln : lines) {
		LineItem item;

		if (ln.empty()) {
			item.w = 0;
			item.h = lineSkip > 0 ? lineSkip : scaledSize;
			items.push_back(item);
			max_w = std::max(max_w, item.w);
			total_h += item.h;
			continue;
		}

		Uint16 unicode[32768];
		han2unicode(ln.c_str(), unicode);

		SDL_Surface* surface = TTF_RenderUNICODE_Blended(font, unicode, color);
		if (!surface) {
			Log::FormattedDebug("Texture", "LoadText", std::string("Failed to render line: ") + TTF_GetError());
			for (auto& it : items) {
				if (it.surf) SDL_FreeSurface(it.surf);
			}
			TTF_CloseFont(font);
			return nullptr;
		}

		item.w = surface->w;
		item.h = surface->h;
		item.surf = surface;

		items.push_back(item);

		max_w = std::max(max_w, item.w);
		total_h += item.h > 0 ? item.h : (lineSkip > 0 ? lineSkip : scaledSize);
	}

	if (max_w <= 0) max_w = 1;
	if (total_h <= 0) total_h = lineSkip > 0 ? lineSkip : scaledSize;

	total_h += 20 * Global::WIN::SIZE_MULTIPLIER * (lines.size() - 1);

	SDL_Surface* finalSurface = SDL_CreateRGBSurfaceWithFormat(
		0, max_w, total_h, 32, SDL_PIXELFORMAT_RGBA32
	);
	if (!finalSurface) {
		Log::FormattedDebug("Texture", "LoadText", std::string("Failed to create final surface: ") + SDL_GetError());
		for (auto& it : items) {
			if (it.surf) SDL_FreeSurface(it.surf);
		}
		TTF_CloseFont(font);
		return nullptr;
	}

	SDL_FillRect(finalSurface, nullptr, SDL_MapRGBA(finalSurface->format, 0, 0, 0, 0));

	int y = 0;
	for (const auto& it : items) {
		const int drawH = (it.h > 0 ? it.h : (lineSkip > 0 ? lineSkip : scaledSize)) + 20 * Global::WIN::SIZE_MULTIPLIER;
		if (it.surf && it.w > 0 && it.h > 0) {
			SDL_Rect dst;
			dst.w = it.w;
			dst.h = it.h;
			dst.x = (max_w - it.w) / 2;
			dst.y = y;
			SDL_BlitSurface(it.surf, nullptr, finalSurface, &dst);
		}
		y += drawH;
	}

	SDL_Texture* finalTexture = SDL_CreateTextureFromSurface(renderer, finalSurface);
	if (!finalTexture) {
		Log::FormattedDebug("Texture", "LoadText", std::string("Failed to create texture from surface: ") + SDL_GetError());
		SDL_FreeSurface(finalSurface);
		for (auto& it : items) {
			if (it.surf) SDL_FreeSurface(it.surf);
		}
		TTF_CloseFont(font);
		return nullptr;
	}
	SDL_SetTextureBlendMode(finalTexture, SDL_BLENDMODE_BLEND);

	SDL_FreeSurface(finalSurface);
	for (auto& it : items) {
		if (it.surf) SDL_FreeSurface(it.surf);
	}
	TTF_CloseFont(font);

	Log::FormattedDebug("Texture", "LoadText", "Loaded multiline text: " + std::string(str));
	return finalTexture;
}
