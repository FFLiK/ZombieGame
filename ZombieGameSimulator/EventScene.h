#pragma once
#include "Scene.h"
#include "Game.h"
#include "Input.h"

class Slide {
public:
	enum Appearance {
		APPEAR,
		DISAPPEAR,
		SEMI_APPEAR
	};

	enum Type {
		TEXT,
		BACKGROUND,
		ROULETTE,
		INPUT
	};

	Slide(Type type, int level, SDL_Texture* texture, Appearance appearance)
		: type(type), level(level), texture(texture), appearance(appearance) {
	}

	Type type;
	int level;
	SDL_Texture* texture = nullptr;
	Appearance appearance;
};

enum class GameEvent {
	INSTANT_EXTRA_MOVE,           // Instantly move one more time : safe
	SWAP_POSITION_WITH_TEAM,      // Instantly swap positions with a chosen team
	MOVE_SUPER_ZOMBIE_TO_TILE,    // Move the super zombie to any chosen tile
	MOVE_SUPER_ZOMBIE_NEARBY,     // Move the super zombie to an adjacent tile
	CHANGE_TEAM_STATE,            // Change the state of a chosen team (Human ¡ê Zombie)
	CHANGE_OWN_STATE,             // Change the state of your own team (Human ¡ê Zombie)
	REVERSE_ALL_STATES,           // Reverse the state of all teams (Human ¡ê Zombie)
	CREATE_OBSTACLE,              // Create an impassable tile (Obstacle)
	SCORE_ROULETTE_EVENT          // Score event (Score roulette)
};

enum Spin {
	NO_SPIN = 0x00,
	NATURAL_NUMBER_ROULETTE_SPIN = 0x01,
	SIGN_ROULETTE_SPIN = 0x02,
};

class RouletteBoard {
public:
	double fraction;
	int data;
	SDL_Texture* texture;

	RouletteBoard(double fraction, int data, SDL_Texture* texture)
		: fraction(fraction), data(data), texture(texture) {
	}
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
	int event_stroy_seed;

	int level;
	const int original_transition_level_delta = 40;
	const int transition_level_delta = original_transition_level_delta * Global::WIN::FRAME_RATE_MULTIPLIER;
	
	vector<Slide> slides;

	SDL_Texture* current_background;
	int background_alpha = 0;
	SDL_Texture* current_text;
	int text_alpha = 0;
	int roulette_alpha = 0;

	vector<RouletteBoard> roulette_board_num;
	vector<RouletteBoard> roulette_board_sign;

	int roulette_active_num = -1;
	int roulette_spin_speed = 7;
	int roulette_angle_num = 0;
	int roulette_angle_sign = 0;
	
	bool is_finished = false;

	Game* game = nullptr;

	int execute_event_level = 0;

	bool pause = false;
	bool enable_event_process = false;
	Hexagon* target_hexagon;

	Input* input;
};

