#include "Player.h"
#include "Hexagon.h"
#include "Log.h"

Player::Player(PlayerState state, double x, double y) {
	this->x = x;
	this->y = y;
	this->state = state;
	this->reserved_state = state;

	Hexagon::GetCenterPointFromHexagonCoordinate(x, y, this->center_x, this->center_y);
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

void Player::DrawPlayer(SDL_Renderer* ren, bool activated, bool draw_left, bool draw_right) const {
	std::vector <SDL_Point> points;

	switch (this->state) {
	case PLAYER_HUMAN:
		Hexagon::GetPointsFromCenterPoint(this->center_x, this->center_y, Player::PLAYER_SIZE, points);
		Sint16 vx[6], vy[6];
		for (size_t i = 0; i < 6; ++i) {
			vx[i] = points[i].x;
			vy[i] = points[i].y;
		}

		if (!draw_left && draw_right) {
			vx[0] = vx[1];
			vx[5] = vx[1];
		}
		else if (draw_left && !draw_right) {
			vx[2] = vx[4];
			vx[3] = vx[4];
		}

		filledPolygonRGBA(ren, vx, vy, 6, 150, 150, 150, 255);
		if (activated) {
			polygonColor(ren, vx, vy, 6, 0xFF0000FF);
		}
		else {
			polygonColor(ren, vx, vy, 6, 0xFFFFFFFF);
		}
		break;
	case PLAYER_ZOMBIE:
		Hexagon::GetPointsFromCenterPoint(this->center_x, this->center_y, Player::PLAYER_SIZE, points);
		for (size_t i = 0; i < 6; ++i) {
			vx[i] = points[i].x;
			vy[i] = points[i].y;
		}

		if (!draw_left && draw_right) {
			vx[0] = vx[1];
			vx[5] = vx[1];
		}
		else if (draw_left && !draw_right) {
			vx[2] = vx[4];
			vx[3] = vx[4];
		}

		filledPolygonRGBA(ren, vx, vy, 6, 150, 190, 40, 255);
		if (activated) {
			polygonColor(ren, vx, vy, 6, 0xFF0000FF);
		}
		else {
			polygonColor(ren, vx, vy, 6, 0xFFFFFFFF);
		}		
		break;
	case PLAYER_SUPER_ZOMBIE:
		Hexagon::GetPointsFromCenterPoint(this->center_x, this->center_y, Player::SUPER_ZOMBIE_SIZE, points);
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
		}		break;
	}
}

void Player::SetPosition(double x, double y, std::vector<Hexagon*>* path) {
	this->x = x;
	this->y = y;
	this->path = *path;
	std::reverse(this->path.begin(), this->path.end());
}

void Player::SetState(PlayerState state) {
	this->reserved_state = state;
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
