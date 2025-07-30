#pragma once
#include <vector>
#include <Hexagon.h>
#include <Player.h>

class Game {
private:
	std::vector<Hexagon> hexagons;
	std::vector<Player> players;

	int current_turn = 0;

	bool Check(double cur_x, double cur_y, double target_x, double target_y, int step, bool first_move, Hexagon* hexagon, Player* player, std::vector<Hexagon*>* path);

	inline static const int SUPER_ZOMBIE_INDEX = 6;

	Player* teleporting_player;

public:
	Game();

	std::vector<Hexagon>* GetHexagons();
	std::vector<Player>* GetPlayers();
	Hexagon* GetHexagon(double x, double y);
	Player* GetPlayer(double x, double y);
	Player* GetCurrentPlayer();
	int GetCurrentTurn() const;

	bool IsMovable(Hexagon* hexagon, Player* player, std::vector<Hexagon*>* path = nullptr);
	void Move(double x, double y);
};

