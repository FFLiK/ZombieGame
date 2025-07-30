#include <SDL.h>

enum EventType { NONE, QUIT, KEY_DOWN, KEY_UP, MOUSE_UP, MOUSE_DOWN, MOUSE_MOVE };
enum EventMouse { MOUSE_NONE, MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE };

class Event {
public:
	EventType T = NONE;
	SDL_Keycode key;
	int x, y, x_rel, y_rel;
	EventMouse mouse = MOUSE_NONE;
};