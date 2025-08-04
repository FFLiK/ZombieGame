#pragma once
#include "Scene.h"
#include "Game.h"

class Slide {
public:
	enum Appearance {
		APPEAR,
		DISAPPEAR,
		SEMI_APPEAR
	};

	Slide(int level, SDL_Texture* texture, Appearance appearance)
		: level(level), texture(texture), appearance(appearance) {
	}
	int level;
	SDL_Texture* texture = nullptr;
	Appearance appearance;
};

enum class GameEvent {
	INSTANT_EXTRA_MOVE,           // Instantly move one more time
	SWAP_POSITION_WITH_TEAM,      // Instantly swap positions with a chosen team
	MOVE_SUPER_ZOMBIE_TO_TILE,    // Move the super zombie to any chosen tile
	MOVE_SUPER_ZOMBIE_NEARBY,     // Move the super zombie to an adjacent tile
	CHANGE_TEAM_STATE,            // Change the state of a chosen team (Human ¡ê Zombie)
	CHANGE_OWN_STATE,             // Change the state of your own team (Human ¡ê Zombie)
	REVERSE_ALL_STATES,           // Reverse the state of all teams (Human ¡ê Zombie)
	CREATE_OBSTACLE,              // Create an impassable tile (Obstacle)
	SCORE_ROULETTE_EVENT          // Score event (Score roulette)
};

class EventScene : public Scene {
public:
	EventScene(Game* game);
	~EventScene();
	int Rendering() override;

	bool IsEnd();

private:
	int ProcessInit() override;
	int EventProcess(Event& evt) override;
	int NormalProcess() override;

	GameEvent game_event;

	int level;
	const int transition_level_delta = 40;
	vector<Slide> slides;

	int background_alpha = 0;
	SDL_Texture* current_text;
	int text_alpha = 0;
	
	bool is_finished = false;

	Game* game = nullptr;

	int execute_event_level = 0;

	bool pause = false;
	bool enable_event_process = false;
	Hexagon* target_hexagon;
};

