#pragma once
#include <SDL.h>
#include <string>
#include <Scene.h>
#include <vector>
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
	
	int Rendering();
	int AddScene(Scene *scene, int pos); //0 = Front, -1 = Back, n = nth index
	int DeleteScene(Scene *scene);
	void __RenderingProcess__();

	int Destroy();
	
	EventType PollEvent();

	int RunTime();
};

