#include "Player.h"
#include "Hexagon.h"
#include "Log.h"

Player::Player(PlayerState state, double x, double y) {
	this->x = x;
	this->y = y;
	this->state = state;
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
	double center_x = 0;
	double center_y = 0;
	Hexagon::GetPixelFromHexagon(this->x, this->y, center_x, center_y);

	std::vector <SDL_Point> points;

	switch (this->state) {
	case PLAYER_HUMAN:
		Hexagon::GetPointsFromHexagon(this->x, this->y, Player::PLAYER_SIZE, points);
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
		Hexagon::GetPointsFromHexagon(this->x, this->y, Player::PLAYER_SIZE, points);
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
		}		break;
	case PLAYER_SUPER_ZOMBIE:
		Hexagon::GetPointsFromHexagon(this->x, this->y, Player::SUPER_ZOMBIE_SIZE, points);
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

void Player::SetPosition(double x, double y) {
	this->x = x;
	this->y = y;
}

void Player::SetState(PlayerState state) {
	this->state = state;
}
