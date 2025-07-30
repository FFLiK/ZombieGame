#pragma once
#include <SDL.h>
#include <Event.h>
#include <queue>
#include <atomic>
using namespace std;

class Scene {
public:
	Scene();
	virtual ~Scene();

	virtual int Rendering() abstract;
	int RegisterRenderer(SDL_Renderer* ren);
	int PushEvent(EventType T, SDL_Keycode key);
	int PushEvent(EventType T, int x, int y, int x_rel, int y_rel, EventMouse mt);
	
	int Destroy();
	
	bool IsRun();

	int __Process__();

protected:
	atomic<bool> run = false;
	atomic<bool> process_completed = false;

	SDL_Renderer* ren;

	queue<Event> event_queue;
	
	virtual int EventProcess(Event &evt) abstract;
	virtual int NormalProcess() abstract;
};

