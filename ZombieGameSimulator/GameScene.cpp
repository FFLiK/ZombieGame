#include "GameScene.h"
#include <Global.h>
#include <Resources.h>
#include <Texture.h>
#include <Log.h>

GameScene::GameScene(Game* game) {
	Log::FormattedDebug("GameScene", "Constructor", "Calling constructor of GameScene");

	this->game = game;
}

GameScene::~GameScene() {
	for (auto& tex : timer_text_tex) {
		SDL_DestroyTexture(tex);
	}

	for (auto& tex : score_text_tex) {
		SDL_DestroyTexture(tex);
	}

	Log::FormattedDebug("GameScene", "Destructor", "Calling destructor of GameScene");
}

int GameScene::Rendering() {
	// Render timer
	SDL_Rect src, dst;
	SDL_QueryTexture(this->timer_title_tex, NULL, NULL, &src.w, &src.h);
	src.x = 0;
	src.y = 0;
	dst.w = src.w;
	dst.h = src.h;
	dst.x = 40 * Global::WIN::SIZE_MULTIPLIER;
	dst.y = Global::WIN::SCREEN_HEIGHT - 120 * Global::WIN::SIZE_MULTIPLIER - src.h;
	SDL_RenderCopy(this->ren, this->timer_title_tex, &src, &dst);
	SDL_QueryTexture(this->timer_text_tex[game->LeftTimerTick()], NULL, NULL, &src.w, &src.h);
	dst.w = src.w;
	dst.h = src.h;
	dst.y = Global::WIN::SCREEN_HEIGHT - 30 * Global::WIN::SIZE_MULTIPLIER - src.h;
	SDL_RenderCopy(this->ren, this->timer_text_tex[game->LeftTimerTick()], NULL, &dst);
	
	// Render scores
	SDL_QueryTexture(this->score_title_tex, NULL, NULL, &src.w, &src.h);
	dst.w = src.w;
	dst.h = src.h;
	dst.x = Global::WIN::SCREEN_WIDTH - 200 * Global::WIN::SIZE_MULTIPLIER;
	dst.y = 40 * Global::WIN::SIZE_MULTIPLIER;
	SDL_RenderCopy(this->ren, this->score_title_tex, &src, &dst);
	for (int i = 0; i < this->score_text_tex.size(); ++i) {
		SDL_QueryTexture(this->score_text_tex[i], NULL, NULL, &src.w, &src.h);
		dst.w = src.w;
		dst.h = src.h;
		dst.x = Global::WIN::SCREEN_WIDTH - 200 * Global::WIN::SIZE_MULTIPLIER;
		dst.y = 100 * Global::WIN::SIZE_MULTIPLIER + i * (src.h + 8 * Global::WIN::SIZE_MULTIPLIER);
		SDL_RenderCopy(this->ren, this->score_text_tex[i], NULL, &dst);
	}

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

	// Render the players
	for (int i = game->GetPlayers()->size() - 1; i >= 0; --i) {
		bool draw_half_left = true;
		bool draw_half_right = true;
		
		if (game->GetPlayers()->at(i).IsArrived()) {
			for (int j = 0; j < game->GetPlayers()->size() - 1; ++j) {
				if (game->GetPlayers()->at(i).GetX() == game->GetPlayers()->at(j).GetX()
					&& game->GetPlayers()->at(i).GetY() == game->GetPlayers()->at(j).GetY()) {
					if (game->GetPlayers()->at(j).IsArrived()) {
						if (i > j)
							draw_half_left = false;
						else if (i < j)
							draw_half_right = false;
					}
				}
			}
		}
		
		game->GetPlayers()->at(i).Move();
		game->GetPlayers()->at(i).DrawPlayer(this->ren, (i == game->GetCurrentTurn()), draw_half_left, draw_half_right);
	}

	if (game->HaveToUpdate()) {
		if (game->IsEventTriggered()) {
			game->ExecuteEvent();
		}
		game->UpdateTurn();
	}

	for (int i = 0; i < game->GetPlayers()->size() - 1; i++) {
		if (game->GetScore(i) != this->score_copy[i]) {
			this->score_copy[i] = game->GetScore(i);
			std::string score_text = "Team " + std::to_string(i + 1) + " : " + std::to_string(game->GetScore(i));
			SDL_DestroyTexture(this->score_text_tex[i]);
			this->score_text_tex[i] = LoadText(score_text.c_str(), this->ren, Global::GAME::SCORE_FONT_SIZE, "font", 255, 255, 255);
		}
	}
	
	return 0;
}


int GameScene::ProcessInit() {
	this->path.clear();
	this->timer_text_tex.clear();
	this->score_text_tex.clear();
	this->score_copy.clear();
	for (int i = 0; i <= Global::GAME::TIME_LIMIT; i++) {
		std::string timer_text = std::to_string(i);
		this->timer_text_tex.push_back(LoadText(timer_text.c_str(), this->ren, Global::GAME::TIMER_FONT_SIZE, "font", 255, 255, 255));
	}
	for (int i = 0; i < game->GetPlayers()->size() - 1; i++) {
		std::string score_text = "Team " + std::to_string(i + 1) + " : " + std::to_string(game->GetScore(i));
		this->score_text_tex.push_back(LoadText(score_text.c_str(), this->ren, Global::GAME::SCORE_FONT_SIZE, "font", 255, 255, 255));
		this->score_copy.push_back(game->GetScore(i));
	}
	this->timer_title_tex = LoadText("Timer.", this->ren, Global::GAME::TIMER_FONT_SIZE * 0.5, "font", 255, 255, 255);
	this->score_title_tex = LoadText("Score.", this->ren, Global::GAME::SCORE_FONT_SIZE * 2.0, "font", 255, 255, 255);
	return 0;
}

int GameScene::EventProcess(Event& evt) {
	if (!game->IsStarted()) {
		if (evt.T == KEY_UP && evt.key == SDLK_SPACE) {
			game->Start();
			Log::System("Game started.");
		}
	}
	else if (!game->IsMoving()) {
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

		if (evt.mouse == MOUSE_RIGHT && evt.T == MOUSE_UP) {
			Log::System("Forced turn change by the right click.");
			game->UpdateTurn();
		}

		if (evt.T == KEY_UP) {
			if (evt.key == SDLK_z) {
				this->game->Undo();
			}
			else if (evt.key == SDLK_y) {
				this->game->Redo();
			}
			else if (evt.key == SDLK_SPACE) {
				this->game->PauseAndResume();
			}
		}
	}
	return 0;
}

int GameScene::NormalProcess() {
	return 0;
}