#include "EventScene.h"
#include "Log.h"
#include "Texture.h"
#include "Resources.h"
#include "Global.h"
#include "Input.h"
#include "Hexagon.h"

EventScene::EventScene(Game* game) {
	this->game = game;
	Log::FormattedDebug("EventScene", "Constructor", "Calling constructor of EventScene");

	this->game_event = static_cast<GameEvent>(rand() % 9);

	this->background_alpha = 0;
	this->current_text = nullptr;
	this->text_alpha = 0;
	this->level = 0;
	
	this->is_finished = false;

	this->execute_event_level = 0;
	this->slides.clear();

	this->enable_event_process = false;
	this->target_hexagon = nullptr;
	this->pause = false;
}

EventScene::~EventScene() {
	Log::FormattedDebug("EventScene", "Destructor", "Calling destructor of EventScene");

	for (auto& slide : slides) {
		if (slide.texture != nullptr && slide.appearance == Slide::APPEAR) {
			SDL_DestroyTexture(slide.texture);
			slide.texture = nullptr;
		}
	}
}

int EventScene::Rendering() {
	for (auto& slide : slides) {
		if (this->level >= slide.level && this->level <= slide.level + transition_level_delta) {
			if (slide.texture == nullptr) {
				if (slide.appearance == Slide::APPEAR) {
					this->background_alpha = 255 * (this->level - slide.level) / transition_level_delta;
				}
				else if (slide.appearance == Slide::SEMI_APPEAR) {
					this->background_alpha = 255 - (105 * (this->level - slide.level) / transition_level_delta);
				}
				else {
					this->background_alpha = 150 - (150 * (this->level - slide.level) / transition_level_delta);
				}
			}
			else {
				this->current_text = slide.texture;
				if (slide.appearance == Slide::APPEAR) {
					this->text_alpha = 255 * (this->level - slide.level) / transition_level_delta;
				}
				else {
					this->text_alpha = 255 - (255 * (this->level - slide.level) / transition_level_delta);
				}
			}
		}
	}

	SDL_Rect background;
	background.x = 0;
	background.y = 0;
	background.w = Global::WIN::SCREEN_WIDTH;
	background.h = Global::WIN::SCREEN_HEIGHT;

	if (this->background_alpha > 0) {
		SDL_SetRenderDrawBlendMode(this->ren, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(this->ren, 0, 0, 0, this->background_alpha);
		SDL_RenderFillRect(this->ren, &background);
	}

	if (this->current_text != nullptr && this->text_alpha > 0) {
		SDL_Rect src, dst;
		SDL_QueryTexture(this->current_text, NULL, NULL, &src.w, &src.h);
		src.x = 0;
		src.y = 0;
		dst.x = (Global::WIN::SCREEN_WIDTH - src.w) / 2;
		dst.y = (Global::WIN::SCREEN_HEIGHT - src.h) / 2;
		dst.w = src.w;
		dst.h = src.h;
		SDL_SetTextureAlphaMod(this->current_text, this->text_alpha);
		SDL_RenderCopy(this->ren, this->current_text, &src, &dst);
		SDL_SetTextureAlphaMod(this->current_text, 255); // Reset alpha for future renders
	}

	if (this->level == this->execute_event_level) {
		switch (this->game_event) {
		case GameEvent::INSTANT_EXTRA_MOVE: {
			this->game->current_turn--;
			if (this->game->current_turn < 0) {
				this->game->current_turn = this->game->GetPlayers()->size() - 1;
			}
		} break;
		case GameEvent::SWAP_POSITION_WITH_TEAM: {
			int target = Input::GetInputInt("Choose a team to swap with (1 - " + std::to_string(this->game->GetPlayers()->size() - 1) + "): ", 1, this->game->GetPlayers()->size() - 1);
			double x1 = this->game->players[this->game->current_turn].GetX();
			double y1 = this->game->players[this->game->current_turn].GetY();
			double x2 = this->game->players[target - 1].GetX();
			double y2 = this->game->players[target - 1].GetY();
			vector<Hexagon*> path1, path2;
			path1.push_back(game->GetHexagon(x2, y2));
			path1.push_back(game->GetHexagon(x1, y1));
			path2.push_back(game->GetHexagon(x1, y1));
			path2.push_back(game->GetHexagon(x2, y2));
			this->game->players[this->game->current_turn].SetPosition(x2, y2, &path1);
			this->game->players[target - 1].SetPosition(x1, y1, &path2);
		} break;
		case GameEvent::MOVE_SUPER_ZOMBIE_TO_TILE: {
			if (target_hexagon == nullptr) {
				this->pause = true;
				this->enable_event_process = true;
			}
			else {
				if (target_hexagon->GetProperty() == HEXAGON_PAPAL) {
					Log::System("EventScene", "Rendering", "Super zombie cannot move to papal hexagon (" + std::to_string(this->target_hexagon->GetX()) + ", " + std::to_string(this->target_hexagon->GetY()) + ")");
					this->target_hexagon = nullptr;
				}
				else {
					this->pause = false;
					vector<Hexagon*> path;
					path.push_back(this->target_hexagon);
					path.push_back(this->game->GetHexagon(this->game->players[this->game->SUPER_ZOMBIE_INDEX].GetX(), this->game->players[this->game->SUPER_ZOMBIE_INDEX].GetY()));
					this->game->players[this->game->SUPER_ZOMBIE_INDEX].SetPosition(this->target_hexagon->GetX(), this->target_hexagon->GetY(), &path);
					Log::System("EventScene", "Rendering", "Super zombie moved to tile (" + std::to_string(this->target_hexagon->GetX()) + ", " + std::to_string(this->target_hexagon->GetY()) + ")");
				}
			}

		} break;
		case GameEvent::MOVE_SUPER_ZOMBIE_NEARBY: {
			if (target_hexagon == nullptr) {
				this->pause = true;
				this->enable_event_process = true;
			}
			else {
				if (target_hexagon->GetProperty() == HEXAGON_PAPAL) {
					Log::System("EventScene", "Rendering", "Super zombie cannot move to papal hexagon (" + std::to_string(this->target_hexagon->GetX()) + ", " + std::to_string(this->target_hexagon->GetY()) + ")");
					this->target_hexagon = nullptr;
				}
				else {
					this->pause = false;
					vector<Hexagon*> path;
					path.push_back(this->target_hexagon);
					path.push_back(this->game->GetHexagon(this->game->players[this->game->SUPER_ZOMBIE_INDEX].GetX(), this->game->players[this->game->SUPER_ZOMBIE_INDEX].GetY()));
					this->game->players[this->game->SUPER_ZOMBIE_INDEX].SetPosition(this->target_hexagon->GetX(), this->target_hexagon->GetY(), &path);
					Log::System("EventScene", "Rendering", "Super zombie moved to tile (" + std::to_string(this->target_hexagon->GetX()) + ", " + std::to_string(this->target_hexagon->GetY()) + ")");
				}
			}
		} break;
		case GameEvent::CHANGE_TEAM_STATE: {
			int target = Input::GetInputInt("Choose a team to change state (1 - " + std::to_string(this->game->GetPlayers()->size() - 1) + "): ", 1, this->game->GetPlayers()->size() - 1);
			PlayerState new_state = (this->game->players[target - 1].GetState() == PLAYER_HUMAN) ? PLAYER_ZOMBIE : PLAYER_HUMAN;
			this->game->players[target - 1].SetState(new_state);
		} break;
		case GameEvent::CHANGE_OWN_STATE: {
			PlayerState new_state = (this->game->GetCurrentPlayer()->GetState() == PLAYER_HUMAN) ? PLAYER_ZOMBIE : PLAYER_HUMAN;
			this->game->GetCurrentPlayer()->SetState(new_state);
		} break;
		case GameEvent::REVERSE_ALL_STATES: {
			for (int i = 0; i < this->game->GetPlayers()->size() - 1; ++i) {
				PlayerState new_state = (this->game->GetPlayers()->at(i).GetState() == PLAYER_HUMAN) ? PLAYER_ZOMBIE : PLAYER_HUMAN;
				this->game->GetPlayers()->at(i).SetState(new_state);
			}
		} break;
		case GameEvent::CREATE_OBSTACLE: {
			if (target_hexagon == nullptr) {
				this->pause = true;
				this->enable_event_process = true;
			}
			else {
				this->pause = false;
				if (this->target_hexagon->GetProperty() == HEXAGON_EVENT) {
					this->game->GetHexagon(
						(this->game->players[this->game->SUPER_ZOMBIE_INDEX]).GetX(),
						(this->game->players[this->game->SUPER_ZOMBIE_INDEX]).GetY()
					)->SetProperty(HEXAGON_EVENT);
				}
				this->target_hexagon->SetProperty(HEXAGON_OBSTACLE);
				Log::System("EventScene", "Rendering", "Obstacle created at tile (" + std::to_string(this->target_hexagon->GetX()) + ", " + std::to_string(this->target_hexagon->GetY()) + ")");
			}
		} break;
		case GameEvent::SCORE_ROULETTE_EVENT: {
			int score = Input::GetInputInt("Enter the score to add (-10 - 10): ", -10, 10);
			this->game->score[this->game->GetCurrentTurn()] += score;
			Log::System("EventScene", "Rendering", "Score roulette event executed. Added " + std::to_string(score) + " points to player " + std::to_string(this->game->GetCurrentTurn() + 1));
		} break;
		}
	}

	if (!pause) this->level++;

	if (this->slides.back().level + this->transition_level_delta < this->level) {
		this->is_finished = true;
	}
	return 0;
}

bool EventScene::IsEnd() {
	return this->is_finished;
}

int EventScene::ProcessInit() {
	string event_main_text, story_text;
	switch (this->game_event) {
	case GameEvent::INSTANT_EXTRA_MOVE:
		event_main_text = "You can move one more time.";
		this->execute_event_level = 450 + transition_level_delta;
		break;

	case GameEvent::SWAP_POSITION_WITH_TEAM:
		event_main_text = "Swap positions with a chosen team.";
		this->execute_event_level = 450 + transition_level_delta;
		break;

	case GameEvent::MOVE_SUPER_ZOMBIE_TO_TILE:
		event_main_text = "Move the super zombie to any chosen tile.";
		this->execute_event_level = 450;
		break;

	case GameEvent::MOVE_SUPER_ZOMBIE_NEARBY:
		event_main_text = "Move the super zombie to an adjacent tile.";
		this->execute_event_level = 450;
		break;

	case GameEvent::CHANGE_TEAM_STATE:
		event_main_text = "Change the state of a chosen team.";
		this->execute_event_level = 450;
		break;

	case GameEvent::CHANGE_OWN_STATE:
		event_main_text = "Change the state of your own team.";
		this->execute_event_level = 450;
		break;

	case GameEvent::REVERSE_ALL_STATES:
		event_main_text = "Reverse the state of all teams.";
		this->execute_event_level = 450;
		break;

	case GameEvent::CREATE_OBSTACLE:
		event_main_text = "Create an obstacle.";
		this->execute_event_level = 450;
		break;

	case GameEvent::SCORE_ROULETTE_EVENT:
		event_main_text = "Score event.";
		this->execute_event_level = 450;
		break;
	}

	story_text = "EVENT";

	Log::FormattedDebug("EventScene", "ProcessInit", "Initializing EventScene with game event: " + event_main_text);
	Log::System("EventScene", "ProcessInit", "Game event: " + event_main_text);

	SDL_Texture* event_main_text_tex = LoadText(event_main_text.c_str(), this->ren, Global::EVENT::MAIN_FONT_SIZE, "font", 255, 255, 255);
	SDL_Texture* event_story_text_tex = LoadText(story_text.c_str(), this->ren, Global::EVENT::MAIN_FONT_SIZE, "font", 255, 255, 255);

	this->slides = {
		Slide(0, nullptr, Slide::APPEAR),
		Slide(50, event_story_text_tex, Slide::APPEAR),
		Slide(200, event_story_text_tex, Slide::DISAPPEAR),
		Slide(250, nullptr, Slide::SEMI_APPEAR),
		Slide(250, event_main_text_tex, Slide::APPEAR),
		Slide(450, event_main_text_tex, Slide::DISAPPEAR),
		Slide(450, nullptr, Slide::DISAPPEAR),
	};
	
	for (int i = 0; i < this->slides.size(); ++i) {
		this->slides[i].level *= Global::WIN::FRAME_RATE_MULTIPLIER;
	}
	this->execute_event_level *= Global::WIN::FRAME_RATE_MULTIPLIER;
	return 0;
}

int EventScene::EventProcess(Event& evt) {
	if (this->enable_event_process) {
		double mouse_x = evt.x;
		double mouse_y = evt.y;

		Player* player = game->GetCurrentPlayer();

		for (auto& hexagon : *(game->GetHexagons())) {
			bool flag = false;
			if (hexagon.IsInside(mouse_x, mouse_y)) {
				if (game->IsMovable(&hexagon, player, nullptr)) {
					flag = true;
				}
			}
			hexagon.SetActivated(flag);
			if (flag) {
				if (evt.mouse == MOUSE_LEFT && evt.T == MOUSE_UP) {
					target_hexagon = &hexagon;
				}
			}
		}

		if (evt.mouse == MOUSE_RIGHT && evt.T == MOUSE_UP) {
			pause = false;
			level++;
			Log::System("EventScene", "EventProcess", "Event process cancelled by right-click.");
		}
	}
	return 0;
}

int EventScene::NormalProcess() {
	return 0;
}
