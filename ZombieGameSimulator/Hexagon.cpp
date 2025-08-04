#include "Hexagon.h"
#include <cmath>
#include <Global.h>
#include <Log.h>
#include <Texture.h>
#include <Resources.h>

Hexagon::Hexagon(double x, double y) {
	this->x = x;
	this->y = y;

	GetCenterPointFromHexagonCoordinate(x, y, center_x, center_y);
	GetPointsFromHexagonCoordinate(x, y, Global::GAME::HEXAGON_SIZE, points);
}
double Hexagon::GetX() const { return x; }
double Hexagon::GetY() const { return y; }

HexagonType Hexagon::GetProperty() const {
	return type;
}

void Hexagon::DrawHexagon(SDL_Renderer* ren) const {
	Sint16 vx[6], vy[6];
	for (size_t i = 0; i < 6; ++i) {
		vx[i] = points[i].x;
		vy[i] = points[i].y;
	}

	SDL_Rect dst;
	dst.w = circumradius * 2;
	dst.h = circumradius * 2;
	dst.x = static_cast<int>(center_x - circumradius);
	dst.y = static_cast<int>(center_y - circumradius);

	switch (type) {
	case HEXAGON_NORMAL:
		if (Global::SYSTEM::TEXTURE_RENDERING) {;
			SDL_RenderCopy(ren, Resources::tile_normal, NULL, &dst);
		}
		else {
			filledPolygonRGBA(ren, vx, vy, 6, 0, 0, 0, 200);
		}
		break;
	case HEXAGON_PAPAL:
		if (Global::SYSTEM::TEXTURE_RENDERING) {
			SDL_RenderCopy(ren, Resources::tile_papal, NULL, &dst);
		}
		else {
			filledPolygonRGBA(ren, vx, vy, 6, 129, 212, 227, 200);
		}
		break;
	case HEXAGON_TELEPORT:
		if (Global::SYSTEM::TEXTURE_RENDERING) {
			SDL_RenderCopy(ren, Resources::tile_teleport, NULL, &dst);
		}
		else {
			filledPolygonRGBA(ren, vx, vy, 6, 199, 129, 227, 200);
		}
		break;
	case HEXAGON_EVENT:
		if (Global::SYSTEM::TEXTURE_RENDERING) {
			SDL_RenderCopy(ren, Resources::tile_event, NULL, &dst);
		}
		else {
			filledPolygonRGBA(ren, vx, vy, 6, 227, 193, 129, 200);
		}		
		break;
	case HEXAGON_OBSTACLE:
		if (Global::SYSTEM::TEXTURE_RENDERING) {
			SDL_RenderCopy(ren, Resources::tile_obstacle, NULL, &dst);
		}
		else {
			filledPolygonRGBA(ren, vx, vy, 6, 150, 150, 150, 200);
		}
		break;
	}

	if (activated) {
		filledPolygonRGBA(ren, vx, vy, 6, 255, 255, 255, 100);
	}

	if (!Global::SYSTEM::TEXTURE_RENDERING) {
		polygonColor(ren, vx, vy, 6, 0xFFFFFFFF);
	}
}

void Hexagon::SetProperty(HexagonType type) {
	this->type = type;
}

void Hexagon::GetCenterPointFromHexagonCoordinate(double x, double y, double &pixel_x, double &pixel_y) {
	pixel_x = x * apothem * 2.0 + Global::WIN::SCREEN_WIDTH_HALF;
	pixel_y = y * circumradius * 3.0 + Global::WIN::SCREEN_HEIGHT_HALF;
}

void Hexagon::GetPointsFromCenterPoint(double center_x, double center_y, double radius, std::vector<SDL_Point>& points) {
	points.resize(6);
	radius = Global::WIN::SCREEN_WIDTH * radius / 1280.0;
	for (int i = 0; i < 6; ++i) {
		double angle = i * M_PI / 3.0 + M_PI / 6.0; // 60 degrees in radians
		points[i].x = static_cast<int>(center_x + radius * cos(angle));
		points[i].y = static_cast<int>(center_y + radius * sin(angle));
	}
}

void Hexagon::GetPointsFromHexagonCoordinate(double x, double y, double radius, std::vector<SDL_Point>& points) {
	double center_x, center_y;
	GetCenterPointFromHexagonCoordinate(x, y, center_x, center_y);
	GetPointsFromCenterPoint(center_x, center_y, radius, points);
}

void Hexagon::SetActivated(bool activated) {
	this->activated = activated;
}

bool Hexagon::IsInside(double x, double y) const {
	bool inside = false;
	int n = points.size();

	for (int i = 0, j = n - 1; i < n; j = i++) {
		double xi = points[i].x, yi = points[i].y;
		double xj = points[j].x, yj = points[j].y;

		bool intersect = ((yi > y) != (yj > y)) &&
			(x < (xj - xi) * (y - yi) / (yj - yi + 1e-12) + xi);
		if (intersect)
			inside = !inside;
	}
	return inside;
}