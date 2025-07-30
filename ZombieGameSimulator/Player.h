#pragma once
#include <SDL.h>
#include <vector>
#include <Hexagon.h>

enum PlayerState {
	PLAYER_HUMAN,
	PLAYER_ZOMBIE,
	PLAYER_SUPER_ZOMBIE,
};

class Player {
private:
	double x, y;
	double center_x, center_y;
	PlayerState state = PLAYER_HUMAN;
	PlayerState reserved_state = PLAYER_HUMAN;

	inline static const double PLAYER_SIZE = 20;
	inline static const double SUPER_ZOMBIE_SIZE = 25;

	inline static const double PLAYER_MOVING_FRAME_NUM = 15;
	int moving_frame = 0;

	std::vector<Hexagon*> path;

public:
	Player(PlayerState state, double x, double y);

	double GetX() const;
	double GetY() const;
	PlayerState GetState() const;

	void DrawPlayer(SDL_Renderer* ren, bool activated, bool draw_left, bool draw_right) const;
	void SetPosition(double x, double y, std::vector<Hexagon*> *path);
	void SetState(PlayerState state);

	bool IsArrived();
	void Move();
	void UpdateState();

	void MoveSuperZombie();
};

