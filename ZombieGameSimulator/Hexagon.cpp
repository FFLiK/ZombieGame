#include "Hexagon.h"
#include <Log.h>

Hexagon::Hexagon(double x, double y) {
	this->x = x;
	this->y = y;

	GetCenterPointFromHexagonCoordinate(x, y, center_x, center_y);
	GetPointsFromHexagonCoordinate(x, y, circumradius, points);
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

	switch (type) {
	case HEXAGON_NORMAL:
		filledPolygonRGBA(ren, vx, vy, 6, 0, 0, 0, 200);
		break;
	case HEXAGON_PAPAL:
		filledPolygonRGBA(ren, vx, vy, 6, 129, 212, 227, 200);
		break;
	case HEXAGON_TELEPORT:
		filledPolygonRGBA(ren, vx, vy, 6, 199, 129, 227, 200);
		break;
	case HEXAGON_EVENT:
		filledPolygonRGBA(ren, vx, vy, 6, 227, 193, 129, 200);
		break;
	}

	if (activated) {
		filledPolygonRGBA(ren, vx, vy, 6, 255, 255, 255, 100);
	}

	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	SDL_RenderDrawLines(ren, points.data(), points.size());
	SDL_RenderDrawLine(ren, points.back().x, points.back().y, points.front().x, points.front().y);
}

void Hexagon::SetProperty(HexagonType type) {
	this->type = type;
}

void Hexagon::GetCenterPointFromHexagonCoordinate(double x, double y, double &pixel_x, double &pixel_y) {
	pixel_x = x * apothem * 2.0 + 1280 / 2.0;
	pixel_y = y* circumradius * 3.0 + 720 / 2.0;
}

void Hexagon::GetPointsFromCenterPoint(double center_x, double center_y, double radius, std::vector<SDL_Point>& points) {
	points.resize(6);
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
