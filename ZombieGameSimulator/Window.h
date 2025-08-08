#pragma once
#include <SDL.h>
#include <string>
#include <Scene.h>
#include <vector>
#include <mutex>
#include <chrono>
using namespace std;

class Window {
private:
	SDL_Window* win;
	SDL_Renderer* ren;
	SDL_Event evt;

	atomic<bool> run = false;
	atomic<bool> rendering_completed;
	int fps;

	std::chrono::time_point<std::chrono::steady_clock> windows_created_time;

	vector<Scene*> scene_list;

	std::recursive_mutex scene_mtx;
	std::recursive_mutex event_mtx;

	bool is_full_screen = false;

	int frame_cnt;
	double frame_time = 0;

	atomic<bool> is_executing = false;

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
	void WaitForExecuting();
	int AddScene(Scene *scene, int pos); //0 = Front, -1 = Back, n = nth index
	int DeleteScene(Scene *scene);
	void __Process__();

	int Destroy();
	
	EventType PollEvent();

	double RunTime();
};

