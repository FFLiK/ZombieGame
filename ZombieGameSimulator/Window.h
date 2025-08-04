#pragma once
#include <SDL.h>
#include <string>
#include <Scene.h>
#include <vector>
#include <mutex>
using namespace std;

class Window {
private:
	SDL_Window* win;
	SDL_Renderer* ren;
	SDL_Event evt;

	atomic<bool> run = false;
	atomic<bool> rendering_completed;
	int fps;

	int windows_created_time;

	vector<Scene*> scene_list;

	std::recursive_mutex  mtx;

public:
	class WindowData {
	public:
		string title;
		int width, height;
		int fps;
	};

	Window(WindowData w_dat);
	~Window();

	int SetWindow(WindowData w_dat);
	
	int Execute();
	int AddScene(Scene *scene, int pos); //0 = Front, -1 = Back, n = nth index
	int DeleteScene(Scene *scene);
	void __Process__();

	int Destroy();
	
	EventType PollEvent();

	int RunTime();
};

