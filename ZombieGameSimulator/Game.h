#pragma once
#include <vector>
#include <Hexagon.h>
#include <Player.h>
#include <Window.h>
#include <Scene.h>

class Game {
private:
	std::vector<Hexagon> hexagons;
	std::vector<Player> players;

	int current_turn = -1;

	bool Check(double cur_x, double cur_y, double target_x, double target_y, int step, bool first_move, Hexagon* hexagon, Player* player, std::vector<Hexagon*>* path);

	Player* teleporting_player;
	Player* event_triggered_player;

	int timer;
	int pause_timer = 0;
	
	int* zombie_infection_score = nullptr;

	std::vector<int> score;

	bool have_to_update = false;

	Window* win;
	Scene* event_scene;

	bool is_started;

public:
	inline static const int SUPER_ZOMBIE_INDEX = 6;

	Game(Window *win);

	std::vector<Hexagon>* GetHexagons();
	std::vector<Player>* GetPlayers();
	Hexagon* GetHexagon(double x, double y);
	Player* GetPlayer(double x, double y);
	Player* GetCurrentPlayer();
	int GetCurrentTurn() const;
	int GetScore(int index) const;

	void Start();

	bool IsMovable(Hexagon* hexagon, Player* player, std::vector<Hexagon*>* path = nullptr);
	void Move(double x, double y);

	bool HaveToUpdate();
	bool IsMoving();
	void UpdateTurn();

	bool IsEventTriggered();
	void ExecuteEvent();

	int LeftTimerTick();

	bool IsStarted() const;

	friend class EventScene;
};

