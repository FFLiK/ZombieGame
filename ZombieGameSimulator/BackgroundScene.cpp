#include "BackgroundScene.h"
#include <Log.h>

BackgroundScene::BackgroundScene(Game *game) {
	Log::FormattedDebug("BackgroundScene", "Constructor", "생성자 호출");

	this->game = game;
}

BackgroundScene::~BackgroundScene() {
	Log::FormattedDebug("BackgroundScene", "Destructor", "소멸자 호출");
}

int BackgroundScene::Rendering() {
	for (const auto& hexagon : *(game->GetHexagons())) {
		hexagon.DrawHexagon(this->ren);
	}
	double prev_x = -1, prev_y = -1;
	for (const auto& hexagon : path) {
		double center_x, center_y;
		Hexagon::GetPixelFromHexagon(hexagon->GetX(), hexagon->GetY(), center_x, center_y);
		filledCircleRGBA(this->ren, center_x, center_y, 5, 255, 0, 0, 200);
		if (prev_x != -1 && prev_y != -1) {
			lineRGBA(this->ren, prev_x, prev_y, center_x, center_y, 255, 0, 0, 200);
		}
		prev_x = center_x;
		prev_y = center_y;
	}
	return 0;
}


int BackgroundScene::EventProcess(Event& evt) {
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
			}
		}
	}
	return 0;
}

int BackgroundScene::NormalProcess()
{
	return 0;
}