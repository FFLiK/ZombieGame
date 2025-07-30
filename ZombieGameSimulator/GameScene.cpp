#include "GameScene.h"
#include <Log.h>

GameScene::GameScene(Window *win, Game* game) {
	Log::FormattedDebug("GameScene", "Constructor", "Calling constructor of GameScene");

	this->game = game;
	this->win = win;
}

GameScene::~GameScene() {
	Log::FormattedDebug("GameScene", "Destructor", "Calling destructor of GameScene");
}

int GameScene::Rendering() {
	// Render the board
	for (const auto& hexagon : *(game->GetHexagons())) {
		hexagon.DrawHexagon(this->ren);
	}
	double prev_x = -1, prev_y = -1;
	for (const auto& hexagon : path) {
		double center_x, center_y;
		Hexagon::GetCenterPointFromHexagonCoordinate(hexagon->GetX(), hexagon->GetY(), center_x, center_y);
		filledCircleRGBA(this->ren, center_x, center_y, 5, 255, 0, 0, 200);
		if (prev_x != -1 && prev_y != -1) {
			lineRGBA(this->ren, prev_x, prev_y, center_x, center_y, 255, 0, 0, 200);
		}
		prev_x = center_x;
		prev_y = center_y;
	}

	bool prev_moving_state = this->is_moving;
	this->is_moving = false;
	// Render the players
	for (int i = game->GetPlayers()->size() - 1; i >= 0; --i) {
		bool draw_half_left = true;
		bool draw_half_right = true;
		
		if (game->GetPlayers()->at(i).IsArrived()) {
			for (int j = 0; j < game->GetPlayers()->size() - 1; ++j) {
				if (game->GetPlayers()->at(i).GetX() == game->GetPlayers()->at(j).GetX()
					&& game->GetPlayers()->at(i).GetY() == game->GetPlayers()->at(j).GetY()) {
					if (game->GetPlayers()->at(j).IsArrived()) {
						if (i < j)
							draw_half_left = false;
						else if (i > j)
							draw_half_right = false;
					}
				}
			}
		}
		
		game->GetPlayers()->at(i).Move();
		this->is_moving |= !game->GetPlayers()->at(i).IsArrived();
		game->GetPlayers()->at(i).DrawPlayer(this->ren, (i == game->GetCurrentTurn()), draw_half_left, draw_half_right);
	}
	if (!this->is_moving && prev_moving_state) {
		game->UpdateTurn();
		if (game->IsEventTriggered()) {
			game->ExecuteEvent();
		}
	}
	return 0;
}


int GameScene::EventProcess(Event& evt) {
	if (!is_moving) {
		double mouse_x = evt.x;
		double mouse_y = evt.y;

		Player* player = game->GetCurrentPlayer();

		path.clear();
		for (auto& hexagon : *(game->GetHexagons())) {
			bool flag = false;
			if (hexagon.IsInside(mouse_x, mouse_y)) {
				if (game->IsMovable(&hexagon, player, &path)) {
					flag = true;
				}
			}
			hexagon.SetActivated(flag);
			if (flag) {
				if (evt.mouse == MOUSE_LEFT && evt.T == MOUSE_UP) {
					game->Move(hexagon.GetX(), hexagon.GetY());
					path.clear();
				}
			}
		}
	}
	return 0;
}

int GameScene::NormalProcess() {
	return 0;
}