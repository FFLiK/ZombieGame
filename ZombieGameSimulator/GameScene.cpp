#include "GameScene.h"
#include <Log.h>

GameScene::GameScene(Game* game) {
	Log::FormattedDebug("GameScene", "Constructor", "������ ȣ��");

	this->game = game;
}

GameScene::~GameScene() {
	Log::FormattedDebug("GameScene", "Destructor", "�Ҹ��� ȣ��");
}

int GameScene::Rendering() {
	for (int i = game->GetPlayers()->size() - 1; i >= 0; --i) {
		bool draw_half_left = true;
		bool draw_half_right = true;
		
		for (int j = 0; j < game->GetPlayers()->size() - 1; ++j) {
			if (game->GetPlayers()->at(i).GetX() == game->GetPlayers()->at(j).GetX()
				&& game->GetPlayers()->at(i).GetY() == game->GetPlayers()->at(j).GetY()) {
				if (i < j)
					draw_half_left = false;
				else if (i > j)
					draw_half_right = false;
			}
		}
		
		game->GetPlayers()->at(i).DrawPlayer(this->ren, (i == game->GetCurrentTurn()), draw_half_left, draw_half_right);
	}
	return 0;
}


int GameScene::EventProcess(Event& evt) {
	return 0;
}

int GameScene::NormalProcess() {
	return 0;
}