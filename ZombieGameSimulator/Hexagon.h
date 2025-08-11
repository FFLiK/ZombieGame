#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <vector>
#include "Global.h"

enum HexagonType {
	HEXAGON_NORMAL,
	HEXAGON_PAPAL,
	HEXAGON_TELEPORT,
	HEXAGON_EVENT,
	HEXAGON_OBSTACLE,
	UNDEFINED
};

class Hexagon {
private:
	inline static const double circumradius = Global::WIN::SCREEN_WIDTH * Global::GAME::HEXAGON_SIZE / 1280.0;
	inline static const double apothem = circumradius * 0.86602540378;

	double x, y;

	double center_x, center_y;
	std::vector<SDL_Point> points;

	HexagonType type = HEXAGON_NORMAL;
	HexagonType prev_type = UNDEFINED;

	int turn_cnt;

	bool activated = false;

public:
	Hexagon(double x, double y);
	~Hexagon();

	double GetX() const;
	double GetY() const;
	HexagonType GetProperty() const;

	void DrawHexagon(SDL_Renderer* ren) const;

	void SetProperty(HexagonType type);

	static void GetCenterPointFromHexagonCoordinate(double x, double y, double &center_x, double &center_y);
	static void GetPointsFromCenterPoint(double center_x, double center_y, double radius, std::vector<SDL_Point>& points);
	static void GetPointsFromHexagonCoordinate(double x, double y, double radius, std::vector<SDL_Point>& points);

	void SetActivated(bool activated);

	bool IsInside(double x, double y) const;
	
	bool visited = false;

	void UpdateTurnCount();
};

