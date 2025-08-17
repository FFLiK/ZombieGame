#pragma once
#include <vector>
#include <Hexagon.h>
#include <Player.h>
#include <Window.h>
#include <Scene.h>
#include <random>
#include <stack>

class Game {
private:
	std::vector<Hexagon> hexagons;
	std::vector<Player> players;

	int current_turn = -1;

	std::vector<int> score;

	std::stack<vector<Hexagon>> hexagon_history;
	std::stack<vector<Player>> player_history;
	std::stack<vector<int>> score_history;
	std::stack<int> turn_history;
	std::stack<vector<Hexagon>> hexagon_after_history;
	std::stack<vector<Player>> player_after_history;
	std::stack<vector<int>> score_after_history;
	std::stack<int> turn_after_history;

	bool Check(double cur_x, double cur_y, double target_x, double target_y, int step, bool first_move, Hexagon* hexagon, Player* player, std::vector<Hexagon*>* path);

	Player* teleporting_player;
	Player* event_triggered_player;
	Player* minigame_triggered_player;
	Player* final_player;

	int timer;
	int pause_timer = 0;
	int pause_timer_i_dont_want_to_use_it_but_HWI_said_it_is_necessary_bull_shit = 0;
	
	bool have_to_update = false;

	Window* win;
	Scene* event_scene, * minigame_scene, * final_human_scene;

	bool is_started;

	std::mt19937 rng;

public:
	inline static const int SUPER_ZOMBIE_INDEX = 6;

	Game(Window *win);
	~Game();

	std::vector<Hexagon>* GetHexagons();
	std::vector<Player>* GetPlayers();
	Hexagon* GetHexagon(double x, double y);
	std::vector<Player*> GetPlayers(double x, double y);
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
	bool IsMinigameTriggerd();
	void ExecuteMinigame();
	bool IsFinalhumanTriggerd();
	void ExecuteFinalHuman();

	int LeftTimerTick();

	bool IsStarted() const;

	void Undo();
	void Redo();
	void Save();

	void PauseAndResume();

	friend class EventScene;
	friend class MinigameScene;
	friend class FinalHumanScene;
};