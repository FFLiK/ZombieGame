#pragma once
#include <SDL.h>
enum PlayerState {
	PLAYER_HUMAN,
	PLAYER_ZOMBIE,
	PLAYER_SUPER_ZOMBIE,
};

class Player {
private:
	double x, y;
	PlayerState state = PLAYER_HUMAN;

	inline static const double PLAYER_SIZE = 20;
	inline static const double SUPER_ZOMBIE_SIZE = 25;

public:
	Player(PlayerState state, double x, double y);

	double GetX() const;
	double GetY() const;
	PlayerState GetState() const;

	void DrawPlayer(SDL_Renderer* ren, bool activated, bool draw_left, bool draw_right) const;
	void SetPosition(double x, double y);
	void SetState(PlayerState state);
};

