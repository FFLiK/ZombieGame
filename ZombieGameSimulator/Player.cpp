#include "Player.h"
#include "Hexagon.h"
#include "Log.h"
#include "Global.h"
#include "Resources.h"
#include "Texture.h"

Player::Player(PlayerState state, double x, double y, int index) {
	this->x = x;
	this->y = y;
	this->state = state;
	this->reserved_state = state;

	this->index = index;

	Hexagon::GetCenterPointFromHexagonCoordinate(x, y, this->center_x, this->center_y);
}

Player::~Player() {
	if (this->index_tex != nullptr) {
		SDL_DestroyTexture(this->index_tex);
		this->index_tex = nullptr;
	}
}

Player::Player(const Player& other) {
	this->x = other.x;
	this->y = other.y;
	this->center_x = other.center_x;
	this->center_y = other.center_y;
	this->state = other.state;
	this->reserved_state = other.reserved_state;
	this->moving_frame = other.moving_frame;
	this->path.clear();
	this->index = other.index;
	this->index_tex = nullptr;
}

double Player::GetX() const {
	return this->x;
}

double Player::GetY() const {
	return this->y;
}

PlayerState Player::GetState() const {
	return this->state;
}

int Player::GetIndex() const {
	return this->index;
}

void Player::DrawPlayer(SDL_Renderer* ren, bool activated, int overlapped_count, int overlapped_index) {
	if (this->index_tex == nullptr) {
		this->index_tex = LoadText(std::to_string(this->index + 1).c_str(), ren, Global::GAME::PLAYER_FONT_SIZE, "font", 255, 255, 255);
	}
	
	std::vector <SDL_Point> points;
	SDL_Rect src, dst;

	if (Global::SYSTEM::TEXTURE_RENDERING) {
		switch (this->state) {
		case PLAYER_HUMAN:
			Hexagon::GetPointsFromCenterPoint(this->center_x, this->center_y, Global::GAME::PLAYER_SIZE, points);
			SDL_QueryTexture(Resources::player_human[this->index], NULL, NULL, &src.w, &src.h);
			src.x = 0;
			src.y = 0;
			dst.h = points[1].y - points[4].y;
			dst.w = dst.h * src.w / src.h;
			dst.h *= Global::GAME::TEXTURE_SIZE_MULTIPLIER;
			dst.w *= Global::GAME::TEXTURE_SIZE_MULTIPLIER;
			dst.x = static_cast<int>(this->center_x - dst.w / 2);
			dst.y = static_cast<int>(this->center_y - dst.h / 2);

			if (overlapped_index == overlapped_count)
				SDL_RenderCopy(ren, Resources::player_human[this->index], &src, &dst);
			break;

		case PLAYER_ZOMBIE:
			Hexagon::GetPointsFromCenterPoint(this->center_x, this->center_y, Global::GAME::PLAYER_SIZE, points);
			SDL_QueryTexture(Resources::player_zombie[this->index], NULL, NULL, &src.w, &src.h);
			src.x = 0;
			src.y = 0;
			dst.h = points[1].y - points[4].y;
			dst.w = dst.h * src.w / src.h;
			dst.h *= Global::GAME::TEXTURE_SIZE_MULTIPLIER;
			dst.w *= Global::GAME::TEXTURE_SIZE_MULTIPLIER;
			dst.x = static_cast<int>(this->center_x - dst.w / 2);
			dst.y = static_cast<int>(this->center_y - dst.h / 2);

			if (overlapped_index == overlapped_count)
				SDL_RenderCopy(ren, Resources::player_zombie[this->index], &src, &dst);
			break;

		case PLAYER_SUPER_ZOMBIE:
			Hexagon::GetPointsFromCenterPoint(this->center_x, this->center_y, Global::GAME::SUPER_ZOMBIE_SIZE, points);
			SDL_QueryTexture(Resources::player_super_zombie, NULL, NULL, &src.w, &src.h);
			src.x = 0;
			src.y = 0;
			dst.h = points[1].y - points[4].y;
			dst.w = dst.h * src.w / src.h;
			dst.h *= Global::GAME::TEXTURE_SIZE_MULTIPLIER;
			dst.w *= Global::GAME::TEXTURE_SIZE_MULTIPLIER;
			dst.x = static_cast<int>(this->center_x - dst.w / 2);
			dst.y = static_cast<int>(this->center_y - dst.h / 2);

			SDL_RenderCopy(ren, Resources::player_super_zombie, NULL, &dst);
			break;
		}
	}
	else {
		switch (this->state) {
		case PLAYER_HUMAN:
			Hexagon::GetPointsFromCenterPoint(this->center_x, this->center_y, Global::GAME::PLAYER_SIZE, points);
			Sint16 vx[6], vy[6];
			for (size_t i = 0; i < 6; ++i) {
				vx[i] = points[i].x;
				vy[i] = points[i].y;
			}

			if (overlapped_index == overlapped_count)
				filledPolygonRGBA(ren, vx, vy, 6, 150, 150, 150, 255);
			if (activated) {
				polygonColor(ren, vx, vy, 6, 0xFF0000FF);
			}
			else {
				polygonColor(ren, vx, vy, 6, 0xFFFFFFFF);
			}
			break;
		case PLAYER_ZOMBIE:
			Hexagon::GetPointsFromCenterPoint(this->center_x, this->center_y, Global::GAME::PLAYER_SIZE, points);
			for (size_t i = 0; i < 6; ++i) {
				vx[i] = points[i].x;
				vy[i] = points[i].y;
			}

			if (overlapped_index == overlapped_count)
				filledPolygonRGBA(ren, vx, vy, 6, 150, 190, 40, 255);
			if (activated) {
				polygonColor(ren, vx, vy, 6, 0xFF0000FF);
			}
			else {
				polygonColor(ren, vx, vy, 6, 0xFFFFFFFF);
			}
			break;
		case PLAYER_SUPER_ZOMBIE:
			Hexagon::GetPointsFromCenterPoint(this->center_x, this->center_y, Global::GAME::SUPER_ZOMBIE_SIZE, points);
			for (size_t i = 0; i < 6; ++i) {
				vx[i] = points[i].x;
				vy[i] = points[i].y;
			}
			filledPolygonRGBA(ren, vx, vy, 6, 150, 190, 40, 255);
			if (activated) {
				polygonColor(ren, vx, vy, 6, 0xFF0000FF);
			}
			else {
				polygonColor(ren, vx, vy, 6, 0xFFFFFFFF);
			}
			break;
		}
	}

	if (this->state != PLAYER_SUPER_ZOMBIE) {
		SDL_QueryTexture(this->index_tex, NULL, NULL, &src.w, &src.h);
		src.x = 0;
		src.y = 0;
		dst.w = src.w;
		dst.h = src.h;
		dst.x = static_cast<int>(this->center_x - dst.w / 2);
		dst.y = static_cast<int>(this->center_y - dst.h / 2);
		double w = points[1].y - points[4].y;
		if (overlapped_count) {
			double rad = w * 0.25;
			double div = overlapped_count + 1;
			dst.x -= cos((double)overlapped_index * M_PI * 2.0 / div) * rad;
			dst.y += sin((double)overlapped_index * M_PI * 2.0 / div) * rad;
		}
		SDL_RenderCopy(ren, this->index_tex, &src, &dst);
	}
}

void Player::SetPosition(double x, double y, std::vector<Hexagon*>* path) {
	this->x = x;
	this->y = y;
	if (path) {
		this->path = *path;
		std::reverse(this->path.begin(), this->path.end());
	}
	else {
		Hexagon::GetCenterPointFromHexagonCoordinate(x, y, this->center_x, this->center_y);
	}
}

void Player::SetState(PlayerState state, bool directly) {
	this->reserved_state = state;
	if (directly) {
		this->state = reserved_state;
	}
}

bool Player::IsArrived() {
	return path.empty();
}

void Player::Move() {
	if (IsArrived()) {
		return;
	}

	Hexagon* current_hexagon = path[0];
	Hexagon* next_hexagon = path[1];

	double current_center_x, current_center_y;
	Hexagon::GetCenterPointFromHexagonCoordinate(current_hexagon->GetX(), current_hexagon->GetY(), current_center_x, current_center_y);
	double next_center_x, next_center_y;
	Hexagon::GetCenterPointFromHexagonCoordinate(next_hexagon->GetX(), next_hexagon->GetY(), next_center_x, next_center_y);

	double factor = static_cast<double>(moving_frame) / PLAYER_MOVING_FRAME_NUM;
	factor = 1 - (1 - factor) * (1 - factor);

	this->center_x = current_center_x * (1 - factor) + next_center_x * factor;
	this->center_y = current_center_y * (1 - factor) + next_center_y * factor;

	if (moving_frame < PLAYER_MOVING_FRAME_NUM) {
		moving_frame++;
	}
	else {
		moving_frame = 0;
		path.erase(path.begin());
		if (path.size() == 1) {
			path.clear();
		}
	}
}

void Player::UpdateState() {
	this->state = this->reserved_state;
}

void Player::MoveSuperZombie() {

}
