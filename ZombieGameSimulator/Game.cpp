#include "Game.h"
#include <Log.h>

Game::Game() {
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
	players.emplace_back(PLAYER_HUMAN, -2, -2);
	players.emplace_back(PLAYER_HUMAN, 2, -2);
	players.emplace_back(PLAYER_HUMAN, 4, 0);
	players.emplace_back(PLAYER_HUMAN, 2, 2);
	players.emplace_back(PLAYER_HUMAN, -2, 2);
	players.emplace_back(PLAYER_HUMAN, -4, 0);
	players.emplace_back(PLAYER_SUPER_ZOMBIE, 0, 0);

	current_turn = 0;

	this->teleporting_player = nullptr;
	this->event_triggered_player = nullptr;

	Log::System("Game setup completed.");
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

Player* Game::GetPlayer(double x, double y) {
	for (int i = 0; i < players.size(); ++i) {
		if (players[i].GetX() == x && players[i].GetY() == y) {
			if (i != this->current_turn) {
				return &players[i];
			}
		}
	}
	return nullptr;
}

Player* Game::GetCurrentPlayer() {
	return &players[current_turn];
}

int Game::GetCurrentTurn() const {
	return current_turn;
}

bool Game::Check(double cur_x, double cur_y, double tarGet_x, double tarGet_y, int step, bool first_move, Hexagon* hexagon, Player* player, std::vector<Hexagon*>* path) {
	auto p = this->GetPlayer(cur_x, cur_y);
	auto h = this->GetHexagon(cur_x, cur_y);

	if (h == nullptr) {
		return false; // Invalid hexagon
	}

	// Constraint 1 : Human players or super zombie cannot move or crossover to papal hexagons
	if (!first_move && (player->GetState() == PLAYER_HUMAN || player->GetState() == PLAYER_SUPER_ZOMBIE) && h->GetProperty() == HEXAGON_PAPAL) {
		return false;
	}
	// Constraint 2 : Zombie cannot crossover to papal hexagons, but can move to them
	if (!first_move && player->GetState() == PLAYER_ZOMBIE && h->GetProperty() == HEXAGON_PAPAL) {
		if (step > 0) {
			return false; // Cannot crossover, but can move to it
		}
	}
	// ELIMINATED : Constraint 2 : Zombies cannot move or crossover to teleport hexagons~~~
	/*
	if (!first_move && (player->GetState() == PLAYER_ZOMBIE || player->GetState() == PLAYER_SUPER_ZOMBIE) && h->GetProperty() == HEXAGON_TELEPORT) {
		return false;
	}
	*/
	// Constraint 3 : Players cannot move or crossover the other player's hexagon
	if (!first_move && p != nullptr && !(player->GetState() != PLAYER_HUMAN && p->GetState() == PLAYER_HUMAN && step == 0)) {
		return false;
	}
	// Constraint 4 : Players cannot visit the same hexagon twice in the same turn
	if (h->visited) {
		return false;
	}

	h->visited = true;
	
	if (step == 0) {
		if (cur_x == tarGet_x && cur_y == tarGet_y) {
			if (path) {
				path->push_back(this->GetHexagon(cur_x, cur_y));
			}
			h->visited = false;
			return true;
		}
	}
	else {
		if (Check(cur_x - 1.0, cur_y, tarGet_x, tarGet_y, step - 1, false, hexagon, player, path)) {
			if (path) {
				path->push_back(this->GetHexagon(cur_x, cur_y));
			}			
			h->visited = false;
			return true;
		}
		else if (Check(cur_x + 1.0, cur_y, tarGet_x, tarGet_y, step - 1, false, hexagon, player, path)) {
			if (path) {
				path->push_back(this->GetHexagon(cur_x, cur_y));
			}
			h->visited = false;
			return true;
		}
		else if (Check(cur_x + 0.5, cur_y - 0.5, tarGet_x, tarGet_y, step - 1, false, hexagon, player, path)) {
			if (path) {
				path->push_back(this->GetHexagon(cur_x, cur_y));
			}			
			h->visited = false;
			return true;
		}
		else if (Check(cur_x - 0.5, cur_y + 0.5, tarGet_x, tarGet_y, step - 1, false, hexagon, player, path)) {
			if (path) {
				path->push_back(this->GetHexagon(cur_x, cur_y));
			}			
			h->visited = false;
			return true;
		}
		else if (Check(cur_x - 0.5, cur_y - 0.5, tarGet_x, tarGet_y, step - 1, false, hexagon, player, path)) {
			if (path) {
				path->push_back(this->GetHexagon(cur_x, cur_y));
			}			
			h->visited = false;
			return true;
		}
		else if (Check(cur_x + 0.5, cur_y + 0.5, tarGet_x, tarGet_y, step - 1, false, hexagon, player, path)) {
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
			&& (hexagon->GetX() != player->GetX() || hexagon->GetY() != player->GetY())) {
			return true;
		}
		else {
			return false;
		}
	}

	double player_x = this->GetCurrentPlayer()->GetX();
	double player_y = this->GetCurrentPlayer()->GetY();

	double tarGet_x = hexagon->GetX();
	double tarGet_y = hexagon->GetY();

	// Constraint : The tarGet hexagon must be different from the current player's hexagon
	if (player_x == tarGet_x && player_y == tarGet_y) {
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

	return Check(player_x, player_y, tarGet_x, tarGet_y, step, true, hexagon, player, path);
}

void Game::Move(double x, double y) {
	Player* player = GetCurrentPlayer();
	Hexagon* hexagon = GetHexagon(x, y);

	if(teleporting_player != nullptr) {
		std::vector<Hexagon*> path;
		path.push_back(hexagon);
		path.push_back(GetHexagon(teleporting_player->GetX(), teleporting_player->GetY()));
		teleporting_player->SetPosition(x, y, &path);
		Log::System("Player teleported to hexagon (" + std::to_string(x) + ", " + std::to_string(y) + ").");
		teleporting_player = nullptr;
		return;
	}

	Player* prev_player = GetPlayer(x, y);
	std::vector<Hexagon*> path;
	if (IsMovable(hexagon, player, &path)) {
		player->SetPosition(x, y, &path);
		Log::System("Player moved to hexagon (" + std::to_string(x) + ", " + std::to_string(y) + ")");
	
		if (player->GetState() == PLAYER_SUPER_ZOMBIE || player->GetState() == PLAYER_ZOMBIE) {
			if (prev_player != nullptr && prev_player->GetState() == PLAYER_HUMAN) {
				prev_player->SetState(PLAYER_ZOMBIE);
				Log::System("Player at hexagon (" + std::to_string(x) + ", " + std::to_string(y) + ") turned into a zombie.");
			}
		}
		if (player->GetState() == PLAYER_HUMAN && hexagon->GetProperty() == HEXAGON_TELEPORT) {
			// Teleport to the other teleport hexagon (It have to be modified later)
			bool tp1 = GetPlayer(-4, 0) == nullptr && !(x == -4 && y == 0);
			bool tp2 = GetPlayer(2, 2) == nullptr && !(x == 2 && y == 2);
			bool tp3 = GetPlayer(2, -2) == nullptr && !(x == 2 && y == -2);

			int num = tp1 + tp2 + tp3;
			switch (num) {
			case 0:
				Log::System("No teleport hexagon available. Cannot teleport.");
				break;
			case 1:
				if (tp1) {
					path.insert(path.begin(), GetHexagon(-4, 0));
					player->SetPosition(-4, 0, &path);
					Log::System("Player teleported to hexagon (-4, 0).");
				}
				else if (tp2) {
					path.insert(path.begin(), GetHexagon(2, 2));
					player->SetPosition(2, 2, &path);
					Log::System("Player teleported to hexagon (2, 2).");
				}
				else if (tp3) {
					path.insert(path.begin(), GetHexagon(2, -2));
					player->SetPosition(2, -2, &path);
					Log::System("Player teleported to hexagon (2, -2).");
				}
				break;
			case 2:
				teleporting_player = player;
				Log::System("Select a teleport hexagon to teleport.");
				break;
			}
		}
		if (player->GetState() == PLAYER_ZOMBIE && hexagon->GetProperty() == HEXAGON_PAPAL) {
			player->SetState(PLAYER_HUMAN);
			Log::System("Player at hexagon (" + std::to_string(x) + ", " + std::to_string(y) + ") turned back to human.");
		}
		if (player->GetState() != PLAYER_SUPER_ZOMBIE && hexagon->GetProperty() == HEXAGON_EVENT) {
			// Event hexagon logic (to be implemented)
			Log::System("Player at hexagon (" + std::to_string(x) + ", " + std::to_string(y) + ") triggered an event.");

			event_triggered_player = player;
		}
	}
}

void Game::UpdateTurn() {
	for (auto& player : this->players) {
		player.UpdateState();
	}
	current_turn = (current_turn + 1) % players.size();
}

bool Game::IsEventTriggered() {
	return event_triggered_player != nullptr;
}

void Game::ExecuteEvent() {
	if (event_triggered_player == nullptr) {
		return;
	}
	Log::System("Executing event for player at hexagon (" + std::to_string(event_triggered_player->GetX()) + ", " + std::to_string(event_triggered_player->GetY()) + ").");

	GetHexagon(this->event_triggered_player->GetX(), this->event_triggered_player->GetY())->SetProperty(HEXAGON_NORMAL);
	GetHexagon(players[SUPER_ZOMBIE_INDEX].GetX(), players[SUPER_ZOMBIE_INDEX].GetY())->SetProperty(HEXAGON_EVENT);
	
	event_triggered_player = nullptr;
}
