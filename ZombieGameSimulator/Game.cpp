#include "Game.h"
#include <Log.h>
#include <Global.h>
#include <Resources.h>
#include <Texture.h>
#include <Input.h>
#include <EventScene.h>
#include <MinigameScene.h>
#include <FinalHumanScene.h>

Game::Game(Window *win) : rng(std::random_device{}()) {
	Log::System("Initializing game...");

	Log::System("Setting up the game board...");
	for (int i = 6; i < 17; i++) {
		double cnt = 11 - abs(i - 11);
		for (int j = 0; j < cnt; j++) {
			double x = -(cnt - 1) * 0.5 + (double)j;
			double y = (i - 11) * 0.5;
			hexagons.emplace_back(x, y);
		}
	}

	GetHexagon(0, 0)->SetProperty(HEXAGON_EVENT);
	GetHexagon(4, 0)->SetProperty(HEXAGON_PAPAL);
	GetHexagon(-2, 2)->SetProperty(HEXAGON_PAPAL);
	GetHexagon(-2, -2)->SetProperty(HEXAGON_PAPAL);
	GetHexagon(-4, 0)->SetProperty(HEXAGON_TELEPORT);
	GetHexagon(2, 2)->SetProperty(HEXAGON_TELEPORT);
	GetHexagon(2, -2)->SetProperty(HEXAGON_TELEPORT);

	Log::System("Setting up players...");
	players.emplace_back(PLAYER_HUMAN, -2, -2, 0);
	players.emplace_back(PLAYER_HUMAN, 2, -2, 1);
	players.emplace_back(PLAYER_HUMAN, 4, 0, 2);
	players.emplace_back(PLAYER_HUMAN, 2, 2, 3);
	players.emplace_back(PLAYER_HUMAN, -2, 2, 4);
	players.emplace_back(PLAYER_HUMAN, -4, 0, 5);
	players.emplace_back(PLAYER_SUPER_ZOMBIE, 0, 0, -1);

	this->current_turn = 0;

	this->teleporting_player = nullptr;
	this->event_triggered_player = nullptr;
	this->minigame_triggered_player = nullptr;
	this->final_player = nullptr;

	this->win = win;
	this->event_scene = nullptr;
	this->minigame_scene = nullptr;
	this->final_human_scene = nullptr;
	this->is_started = false;

	score.clear();
	for (int i = 0; i < players.size(); ++i) {
		score.push_back(0);
	}

	// Build base directory
	namespace fs = std::filesystem;

	std::string dir;
	if (Global::SYSTEM::USE_APPDATA) {
		if (const char* appdata = std::getenv("APPDATA")) {
#if defined(_WIN32)
			dir = std::string(appdata) + "\\" + Global::SYSTEM::NAME + "\\";
#else
			dir = std::string(appdata) + "/" + Global::SYSTEM::NAME + "/";
#endif
		}
	}
	this->save_file_path = dir + "save.dat";
	Log::System("Game setup completed.");
	Log::System("Press SPACE to start the game.");
}

Game::~Game() {
	Log::System("Game resources destroyed.");
}

std::vector<Hexagon>* Game::GetHexagons() {
	return &(this->hexagons);
}

std::vector<Player>* Game::GetPlayers() {
	return &(this->players);
}

Hexagon* Game::GetHexagon(double x, double y) {
	for (auto& hexagon : hexagons) {
		if (hexagon.GetX() == x && hexagon.GetY() == y) {
			return &hexagon;
		}
	}
	return nullptr;
}

std::vector<Player*> Game::GetPlayers(double x, double y) {
	std::vector<Player*> result;
	for (int i = 0; i < players.size(); ++i) {
		if (players[i].GetX() == x && players[i].GetY() == y) {
			if (i != this->current_turn) {
				result.push_back(&players[i]);
			}
		}
	}
	return result;
}

Player* Game::GetCurrentPlayer() {
	if (current_turn >= 0 && current_turn < players.size()) {
		return &players[current_turn];
	}
	return nullptr;
}

int Game::GetCurrentTurn() const {
	return current_turn;
}

int Game::GetScore(int index) const {
	return (index >= 0 && index < score.size()) ? score[index] : 0;
}

void Game::Start() {
	this->timer = clock();
	this->is_started = true;

	this->pause_timer_i_dont_want_to_use_it_but_HWI_said_it_is_necessary_bull_shit = -1;

	Log::System("Game started. Current turn: " + std::to_string(current_turn));
}

bool Game::Check(double cur_x, double cur_y, double target_x, double target_y, int step, bool first_move, Hexagon* hexagon, Player* player, std::vector<Hexagon*>* path) {
	auto p = this->GetPlayers(cur_x, cur_y);
	auto h = this->GetHexagon(cur_x, cur_y);

	if (h == nullptr) {
		return false; // Invalid hexagon
	}

	// Constraint 1 : Human players or super zombie cannot move or crossover to papal hexagons
	if (!first_move && (player->GetState() == PLAYER_HUMAN || player->GetState() == PLAYER_SUPER_ZOMBIE) && h->GetProperty() == HEXAGON_PAPAL) {
		return false;
	}
	// Constraint 2 : Zombie cannot crossover to papal hexagons, but can move to them if there is other player.
	if (!first_move && player->GetState() == PLAYER_ZOMBIE && h->GetProperty() == HEXAGON_PAPAL) {
		if (step > 0 || !p.empty()) {
			return false; // Cannot crossover, but can move to it
		}
	}
	// Constraint 3 : Human players cannot move or crossover the other player's hexagon
	if (!first_move && !p.empty() && !(player->GetState() != PLAYER_HUMAN && p[0]->GetState() == PLAYER_HUMAN && step == 0)) {
		return false;
	}
	// Constraint 4 : Players cannot visit the same hexagon twice in the same turn
	if (h->visited) {
		return false;
	}
	// Constraint 5 : Every players cannot move or crossover the obstacle hexagons
	if (h->GetProperty() == HEXAGON_OBSTACLE) {
		return false;
	}

	h->visited = true;
	
	if (step == 0) {
		if (cur_x == target_x && cur_y == target_y) {
			if (path) {
				path->push_back(this->GetHexagon(cur_x, cur_y));
			}
			h->visited = false;
			return true;
		}
	}
	else {
		if (Check(cur_x - 1.0, cur_y, target_x, target_y, step - 1, false, hexagon, player, path)) {
			if (path) {
				path->push_back(this->GetHexagon(cur_x, cur_y));
			}			
			h->visited = false;
			return true;
		}
		else if (Check(cur_x + 1.0, cur_y, target_x, target_y, step - 1, false, hexagon, player, path)) {
			if (path) {
				path->push_back(this->GetHexagon(cur_x, cur_y));
			}
			h->visited = false;
			return true;
		}
		else if (Check(cur_x + 0.5, cur_y - 0.5, target_x, target_y, step - 1, false, hexagon, player, path)) {
			if (path) {
				path->push_back(this->GetHexagon(cur_x, cur_y));
			}			
			h->visited = false;
			return true;
		}
		else if (Check(cur_x - 0.5, cur_y + 0.5, target_x, target_y, step - 1, false, hexagon, player, path)) {
			if (path) {
				path->push_back(this->GetHexagon(cur_x, cur_y));
			}			
			h->visited = false;
			return true;
		}
		else if (Check(cur_x - 0.5, cur_y - 0.5, target_x, target_y, step - 1, false, hexagon, player, path)) {
			if (path) {
				path->push_back(this->GetHexagon(cur_x, cur_y));
			}			
			h->visited = false;
			return true;
		}
		else if (Check(cur_x + 0.5, cur_y + 0.5, target_x, target_y, step - 1, false, hexagon, player, path)) {
			if (path) {
				path->push_back(this->GetHexagon(cur_x, cur_y));
			}			
			h->visited = false;
			return true;
		}
	}
	h->visited = false;
	return false;
}

bool Game::IsMovable(Hexagon* hexagon, Player* player, std::vector<Hexagon*>* path) {
	if (teleporting_player != nullptr) {
		if (hexagon->GetProperty() == HEXAGON_TELEPORT
			&& (hexagon->GetX() != teleporting_player->GetX() || hexagon->GetY() != teleporting_player->GetY())) {
			return true;
		}
		else {
			return false;
		}
	}

	if (event_triggered_player != nullptr) {
		if (!GetPlayers(hexagon->GetX(), hexagon->GetY()).empty()
			|| (this->GetCurrentPlayer()->GetX() == hexagon->GetX() && this->GetCurrentPlayer()->GetY() == hexagon->GetY())) {
			return false;
		}
		else {
			return true;
		}
	}

	double player_x = this->GetCurrentPlayer()->GetX();
	double player_y = this->GetCurrentPlayer()->GetY();

	double target_x = hexagon->GetX();
	double target_y = hexagon->GetY();

	// Constraint : The target hexagon must be different from the current player's hexagon
	if (player_x == target_x && player_y == target_y) {
		return false;
	}

	for (auto& h : hexagons) {
		h.visited = false; // Reset visited status for all hexagons
	}

	int step = 1;
	switch (player->GetState()) {
	case PLAYER_HUMAN:
		step = 2;
		break;
	case PLAYER_ZOMBIE:
		step = 3;
		break;
	case PLAYER_SUPER_ZOMBIE:
		step = 4;
		break;
	}

	return Check(player_x, player_y, target_x, target_y, step, true, hexagon, player, path);
}

void Game::Move(double x, double y) {
	this->have_to_update = true;
	this->pause_timer_i_dont_want_to_use_it_but_HWI_said_it_is_necessary_bull_shit = -1;

	this->Save();

	Player* player = GetCurrentPlayer();
	Hexagon* hexagon = GetHexagon(x, y);

	this->pause_timer = clock();

	if(teleporting_player != nullptr) {
		std::vector<Hexagon*> path;
		path.push_back(hexagon);
		path.push_back(GetHexagon(teleporting_player->GetX(), teleporting_player->GetY()));
		teleporting_player->SetPosition(x, y, &path);
		Log::System("Player teleported to hexagon (" + std::to_string(x) + ", " + std::to_string(y) + ").");
		teleporting_player = nullptr;
		return;
	}

	auto prev_players = GetPlayers(x, y);
	std::vector<Hexagon*> path;
	if (IsMovable(hexagon, player, &path)) {
		player->SetPosition(x, y, &path);
		Log::System("Player moved to hexagon (" + std::to_string(x) + ", " + std::to_string(y) + ")");
	
		if (player->GetState() == PLAYER_HUMAN) {
			int human_num = 0;
			for (auto& p : players) {
				if (p.GetState() == PLAYER_HUMAN) {
					human_num++;
				}
			}
			if (human_num == 1) {
				this->final_player = player;
				Log::System("Player " + std::to_string(this->current_turn), "is the last human.");
			}
			else {
				score[player->GetIndex()] += Global::GAME::HUMAN_MOVING_SCORE;
				Log::System("Player " + std::to_string(this->current_turn), "moved. Score updated to", score[player->GetIndex()], "(+" + std::to_string(Global::GAME::HUMAN_MOVING_SCORE) + ")");
			}
		}

		if (player->GetState() == PLAYER_SUPER_ZOMBIE || player->GetState() == PLAYER_ZOMBIE) {
			for (auto prev_player : prev_players) {
				prev_player->SetState(PLAYER_ZOMBIE);
				Log::System("Player " + std::to_string(prev_player->GetIndex()), "turned into a zombie.");

				if (player->GetState() == PLAYER_SUPER_ZOMBIE) {
					score[prev_player->GetIndex()] += Global::GAME::INFECTED_SUPER_ZOMBIE_PENALTY_SCORE;
					Log::System("Player " + std::to_string(prev_player->GetIndex()), "infected by super zombie. Score updated to", score[prev_player->GetIndex()], "(" + std::to_string(Global::GAME::INFECTED_SUPER_ZOMBIE_PENALTY_SCORE) + ")");
				}
				else {
					this->minigame_triggered_player = GetCurrentPlayer();
				}
			}
		}
		if (player->GetState() == PLAYER_HUMAN && hexagon->GetProperty() == HEXAGON_TELEPORT) {
			int num = 0;
			for (int i = 0; i < this->hexagons.size(); i++) {
				if (this->hexagons[i].GetProperty() == HEXAGON_TELEPORT) {
					if (GetPlayers(this->hexagons[i].GetX(), this->hexagons[i].GetY()).empty()) {
						num++;
					}
				}
			}
			switch (num) {
			case 0:
				Log::System("No teleport hexagon available. Cannot teleport.");
				break;
			case 1:
				Log::System("No teleport hexagon available. Cannot teleport.");
				break;
			case 2:
				for (int i = 0; i < this->hexagons.size(); i++) {
					if (this->hexagons[i].GetProperty() == HEXAGON_TELEPORT
						&& (this->hexagons[i].GetX() != x || this->hexagons[i].GetY() != y)
						&& GetPlayers(this->hexagons[i].GetX(), this->hexagons[i].GetY()).empty()) {
						path.insert(path.begin(), &this->hexagons[i]);
						player->SetPosition(this->hexagons[i].GetX(), this->hexagons[i].GetY(), &path);
						Log::System("Player teleported to hexagon (" + std::to_string(this->hexagons[i].GetX()) + ", " + std::to_string(this->hexagons[i].GetY()) + ").");
					}
				}
				break;
			default:
				teleporting_player = player;
				Log::System("Select a teleport hexagon to teleport.");
				break;
			}
		}
		if (player->GetState() == PLAYER_ZOMBIE && hexagon->GetProperty() == HEXAGON_PAPAL) {
			player->SetState(PLAYER_HUMAN);
			Log::System("Player " + std::to_string(player->GetIndex()), "turned back to human.");
		}
		if (player->GetState() != PLAYER_SUPER_ZOMBIE && hexagon->GetProperty() == HEXAGON_EVENT) {
			// Event hexagon logic (to be implemented)
			Log::System("Player " + std::to_string(player->GetIndex()), "triggered an event.");

			event_triggered_player = player;
		}
	}
}

bool Game::HaveToUpdate() {
	if (IsMoving()) {
		return false;
	}
	if (!this->have_to_update) {
		return false;
	}
	return true;
}

bool Game::IsMoving() {
	for (auto& player : players) {
		if (!player.IsArrived()) {
			return true;
		}
	}
	return false;
}

void Game::UpdateTurn() {
	for (auto& player : this->players) {
		player.UpdateState();
	}
	if (!this->teleporting_player && !this->event_triggered_player&& !this->minigame_triggered_player && !this->final_player) {
		current_turn = (current_turn + 1) % players.size();

		for (int i = 0; i < this->hexagons.size(); i++) {
			this->hexagons[i].UpdateTurnCount();
		}

		timer = clock();
		pause_timer = 0;

		this->have_to_update = false;

		Log::System("Updated turn to player " + std::to_string(current_turn) + " at hexagon (" + std::to_string(players[current_turn].GetX()) + ", " + std::to_string(players[current_turn].GetY()) + ").");
	
		this->SaveData();
	}
}

bool Game::IsEventTriggered() {
	return event_triggered_player != nullptr;
}

void Game::ExecuteEvent() {
	if (event_triggered_player == nullptr) {
		return;
	}
	if (this->event_scene == nullptr) {
		Log::System("Executing event for player " + std::to_string(event_triggered_player->GetIndex()) + ".");
		this->event_scene = new EventScene(this);
		win->AddScene(this->event_scene, 0);
	}
	else if (static_cast<EventScene*>(this->event_scene)->IsEnd()){
		int hexagon_num = 0;
		for (int i = 0; i < this->hexagons.size(); i++) {
			if (this->hexagons[i].GetProperty() == HEXAGON_EVENT) {
				this->hexagons[i].SetProperty(HEXAGON_NORMAL);
			}
			else if (this->hexagons[i].GetProperty() == HEXAGON_NORMAL
				&& this->GetPlayers(this->hexagons[i].GetX(), this->hexagons[i].GetY()).empty()) {
				hexagon_num++;
			}
		}

		std::uniform_int_distribution<int> dist(0, hexagon_num - 1);
		int target_hexagon = dist(rng);

		int cnt = 0;
		for (int i = 0; i < this->hexagons.size(); i++) {
			if (this->hexagons[i].GetProperty() == HEXAGON_NORMAL
				&& this->GetPlayers(this->hexagons[i].GetX(), this->hexagons[i].GetY()).empty()) {
				if (cnt == target_hexagon) {
					this->hexagons[i].SetProperty(HEXAGON_EVENT);
					break;
				}
				cnt++;
			}
		}

		event_triggered_player = nullptr;
		win->DeleteScene(this->event_scene);
		this->event_scene = nullptr;
	}
}

bool Game::IsMinigameTriggerd() {
	return minigame_triggered_player != nullptr;
}

void Game::ExecuteMinigame() {
	if (minigame_triggered_player == nullptr) {
		return;
	}
	if (this->minigame_scene == nullptr) {
		Log::System("Executing minigame for team", minigame_triggered_player->GetIndex());
		this->minigame_scene = new MinigameScene(this);
		win->AddScene(this->minigame_scene, 0);
	}
	else if (static_cast<MinigameScene*>(this->minigame_scene)->IsEnd()) {
		minigame_triggered_player = nullptr;
		win->DeleteScene(this->minigame_scene);
		this->minigame_scene = nullptr;
	}
}

bool Game::IsFinalhumanTriggerd() {
	return final_player != nullptr;
}

void Game::ExecuteFinalHuman() {
	if (final_player == nullptr) {
		return;
	}
	if (this->final_human_scene == nullptr) {
		Log::System("Executing final human scene for player", final_player->GetIndex() + ".");
		this->final_human_scene = new FinalHumanScene(this);
		win->AddScene(this->final_human_scene, 0);
	}
	else if (static_cast<FinalHumanScene*>(this->final_human_scene)->IsEnd()) {
		final_player = nullptr;
		win->DeleteScene(this->final_human_scene);
		this->final_human_scene = nullptr;
	}
}

int Game::LeftTimerTick() {
	if (this->is_started == false) {
		return 0;
	}
	if (this->timer == 0) {
		return Global::GAME::TIME_LIMIT;
	}
	int current_time = this->pause_timer == 0 ? clock() : this->pause_timer;
	if (this->pause_timer_i_dont_want_to_use_it_but_HWI_said_it_is_necessary_bull_shit != -1) {
		current_time = this->pause_timer_i_dont_want_to_use_it_but_HWI_said_it_is_necessary_bull_shit;
	}
	int elapsed_time = (current_time - this->timer) / CLOCKS_PER_SEC;
	int remaining_time = Global::GAME::TIME_LIMIT - elapsed_time;
	if (remaining_time < 0) {
		this->timer = clock() - remaining_time;
		if (this->current_turn >= 0 && this->current_turn < this->SUPER_ZOMBIE_INDEX) {
			this->score[this->current_turn]--;
		}
		remaining_time = 0;
	}
	if (remaining_time > Global::GAME::TIME_LIMIT) {
		remaining_time = Global::GAME::TIME_LIMIT;
	}
	return remaining_time;
}

bool Game::IsStarted() const {
	return this->is_started;
}

void Game::Open() {
	std::ifstream load_file;
	namespace fs = std::filesystem;
	load_file.open(this->save_file_path, std::ios::binary | std::ios::in);
	if (!load_file.is_open()) {
		Log::System("Save file is empty.");
		return;
	}
	if (fs::file_size(this->save_file_path) == 0) {
		Log::System("Save file is empty.");
		return;
	}
	load_file.read(reinterpret_cast<char*>(&this->current_turn), sizeof(this->current_turn));
	for (auto& hexagon : this->hexagons) {
		auto v = hexagon.GetProperty();
		load_file.read(reinterpret_cast<char*>(&v), sizeof(v));
		hexagon.SetProperty(v);
	}
	for (auto& player : this->players) {
		auto v1 = player.GetX();
		auto v2 = player.GetY();
		auto v3 = player.GetState();
		load_file.read(reinterpret_cast<char*>(&v1), sizeof(v1));
		load_file.read(reinterpret_cast<char*>(&v2), sizeof(v2));
		load_file.read(reinterpret_cast<char*>(&v3), sizeof(v3));
		player.SetPosition(v1, v2, nullptr);
		player.SetState(v3, true);
	}
	for (auto& score : this->score) {
		load_file.read(reinterpret_cast<char*>(&score), sizeof(int));
	}
	load_file.close();

	// print loaded game state
	Log::System("Game state loaded from file.");
	Log::System("GAME STATE");
	Log::System("Current turn: " + std::to_string(current_turn));
	for (int i = 0; i < this->hexagons.size(); i++) {
		Log::System("Hexagon " + std::to_string(i) + ": (" + std::to_string(this->hexagons[i].GetX()) + ", " + std::to_string(this->hexagons[i].GetY()) + "), Property: " + std::to_string(this->hexagons[i].GetProperty()));
	}
	for (int i = 0; i < this->players.size(); i++) {
		Log::System("Player " + std::to_string(i) + ": (" + std::to_string(this->players[i].GetX()) + ", " + std::to_string(this->players[i].GetY()) + "), State: " + std::to_string(this->players[i].GetState()));
	}

	this->hexagon_history = std::stack<std::vector<Hexagon>>();
	this->player_history = std::stack<std::vector<Player>>();
	this->score_history = std::stack<std::vector<int>>();
	this->turn_history = std::stack<int>();
	this->hexagon_after_history = std::stack<std::vector<Hexagon>>();
	this->player_after_history = std::stack<std::vector<Player>>();
	this->score_after_history = std::stack<std::vector<int>>();
	this->turn_after_history = std::stack<int>();
}

void Game::Undo() {
	if (this->hexagon_history.empty()) {
		Log::System("No history to restore.");
		return;
	}
	this->hexagon_after_history.push(this->hexagons);
	this->hexagons = this->hexagon_history.top();
	this->hexagon_history.pop();
	this->player_after_history.push(this->players);
	this->players = this->player_history.top();
	this->player_history.pop();
	this->score_after_history.push(this->score);
	this->score = this->score_history.top();
	this->score_history.pop();
	this->turn_after_history.push(this->current_turn);
	this->current_turn = this->turn_history.top();
	this->turn_history.pop();

	this->teleporting_player = nullptr;
	this->timer = clock();

	Log::System("Game state restored from history. Current turn: " + std::to_string(current_turn + 1));
}

void Game::Redo() {
	if (this->hexagon_after_history.empty()) {
		Log::System("No history to redo.");
		return;
	}
	this->hexagon_history.push(this->hexagons);
	this->hexagons = this->hexagon_after_history.top();
	this->hexagon_after_history.pop();
	this->player_history.push(this->players);
	this->players = this->player_after_history.top();
	this->player_after_history.pop();
	this->score_history.push(this->score);
	this->score = this->score_after_history.top();
	this->score_after_history.pop();
	this->turn_history.push(this->current_turn);
	this->current_turn = this->turn_after_history.top();
	this->turn_after_history.pop();

	this->teleporting_player = nullptr;
	this->timer = clock();
	Log::System("Game state redone from history. Current turn: " + std::to_string(current_turn + 1));
}

void Game::Save() {
	this->hexagon_history.push(this->hexagons);
	this->player_history.push(this->players);
	this->score_history.push(this->score);
	this->turn_history.push(this->current_turn);
	this->hexagon_after_history = std::stack<std::vector<Hexagon>>();
	this->player_after_history = std::stack<std::vector<Player>>();
	this->score_after_history = std::stack<std::vector<int>>();
	this->turn_after_history = std::stack<int>();
}

void Game::SaveData() {
	std::ofstream save_file;
	namespace fs = std::filesystem;
	save_file.open(this->save_file_path, std::ios::binary | std::ios::out);
	if (!save_file.is_open()) {
		Log::Error("Failed to open save file for writing.");
		return;
	}
	save_file.write(reinterpret_cast<const char*>(&this->current_turn), sizeof(this->current_turn));
	for (const auto& hexagon : this->hexagons) {
		auto v = hexagon.GetProperty();
		save_file.write(reinterpret_cast<const char*>(&v), sizeof(v));
	}
	for (const auto& player : this->players) {
		auto v1 = player.GetX();
		auto v2 = player.GetY();
		auto v3 = player.GetState();
		save_file.write(reinterpret_cast<const char*>(&v1), sizeof(v1));
		save_file.write(reinterpret_cast<const char*>(&v2), sizeof(v2));
		save_file.write(reinterpret_cast<const char*>(&v3), sizeof(v3));
	}
	for (const auto& score : this->score) {
		save_file.write(reinterpret_cast<const char*>(&score), sizeof(int));
	}
	save_file.close();
	Log::System("Game state saved to file.");
}

void Game::PauseAndResume() {
	if (this->pause_timer_i_dont_want_to_use_it_but_HWI_said_it_is_necessary_bull_shit == -1) {
		this->pause_timer_i_dont_want_to_use_it_but_HWI_said_it_is_necessary_bull_shit = clock();
	}
	else {
		this->timer += clock() - this->pause_timer_i_dont_want_to_use_it_but_HWI_said_it_is_necessary_bull_shit;
		this->pause_timer_i_dont_want_to_use_it_but_HWI_said_it_is_necessary_bull_shit = -1;
	}
}

Window* Game::GetWindow() const {
	return this->win;
}
