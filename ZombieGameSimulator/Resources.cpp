#include "Resources.h"
#include "Texture.h"
#include <Log.h>
#include <string>
using namespace std;

bool Resources::initialized = false;

SDL_Texture* Resources::background = nullptr;

SDL_Texture* Resources::tile_normal = nullptr;
SDL_Texture* Resources::tile_papal = nullptr;
SDL_Texture* Resources::tile_teleport = nullptr;
SDL_Texture* Resources::tile_event = nullptr;
SDL_Texture* Resources::tile_obstacle = nullptr;

SDL_Texture* Resources::player_human[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
SDL_Texture* Resources::player_zombie[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
SDL_Texture* Resources::player_super_zombie = nullptr;

SDL_Texture* Resources::minigame_background = nullptr;

SDL_Texture* Resources::event_roulette_num = nullptr;
SDL_Texture* Resources::event_roulette_sign = nullptr;
SDL_Texture* Resources::minigame_roulette = nullptr;

SDL_Texture* Resources::roulette_pin = nullptr;

PdfViewer* Resources::event_pdf = nullptr;
PdfViewer* Resources::minigame_unified_voice_game_pdf = nullptr;
PdfViewer* Resources::minigame_emoji_game_pdf = nullptr;

void Resources::InitResources(SDL_Renderer* renderer) {
	if (initialized) {
		return;
	}
	background = LoadImage("background", renderer);
	tile_normal = LoadImage("tile_normal", renderer);
	tile_papal = LoadImage("tile_papallado", renderer);
	tile_teleport = LoadImage("tile_teleport", renderer);
	tile_event = LoadImage("tile_event", renderer);
	tile_obstacle = LoadImage("tile_obstacle", renderer);

	for (char i = 0; i < 6; ++i) {
		string human_name = "player_" + std::string(1, i + 'a');
		string zombie_name = "zombie_" + std::string(1, i + 'a');
		human_name = "player";
		zombie_name = "zombie";
		player_human[i] = LoadImage(human_name.c_str(), renderer);
		player_zombie[i] = LoadImage(zombie_name.c_str(), renderer);
	}
	player_super_zombie = LoadImage("superzombie", renderer);

	minigame_background = LoadImage("minigame_background", renderer);
	event_roulette_num = LoadImage("event_roulette_num", renderer);
	event_roulette_sign = LoadImage("event_roulette_sign", renderer);
	minigame_roulette = LoadImage("minigame_roulette", renderer);

	roulette_pin = LoadImage("roulette_pin", renderer);

	event_pdf = new PdfViewer("event", renderer);
	minigame_unified_voice_game_pdf = new PdfViewer("minigame_unified_voice_game", renderer);
	minigame_emoji_game_pdf = new PdfViewer("minigame_emoji_game", renderer);
}

void Resources::QuitResources() {
	if (!initialized) {
		return;
	}
	SDL_DestroyTexture(background);
	SDL_DestroyTexture(tile_normal);
	SDL_DestroyTexture(tile_papal);
	SDL_DestroyTexture(tile_teleport);
	SDL_DestroyTexture(tile_event);
	SDL_DestroyTexture(tile_obstacle);
	for (int i = 0; i < 6; ++i) {
		SDL_DestroyTexture(player_human[i]);
		SDL_DestroyTexture(player_zombie[i]);
	}
	SDL_DestroyTexture(player_super_zombie);
	delete event_pdf;
	delete minigame_unified_voice_game_pdf;
	delete minigame_emoji_game_pdf;

	background = nullptr;
	tile_normal = nullptr;
	tile_papal = nullptr;
	tile_teleport = nullptr;
	tile_event = nullptr;
	for (int i = 0; i < 6; ++i) {
		player_human[i] = nullptr;
		player_zombie[i] = nullptr;
	}
	player_super_zombie = nullptr;
	event_pdf = nullptr;
	minigame_background = nullptr;
	minigame_roulette = nullptr;

	initialized = false;
}
