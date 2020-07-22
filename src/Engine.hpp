#ifndef Engine_H
#define Engine_H

#include "Graphics.hpp"
#include "Keyboard.hpp"
#include "Tilemap.hpp"
#include "Camera.hpp"
#include "Mouse.hpp"

class Engine
{
	private:
		bool running, selectMode, canToggle;
		int selectedTile;
		double targetFPS;
		long long updates;
		Graphics* graphics;
		Keyboard* keyboard;
		Mouse* mouse;
		Tilemap* tilemap;
		Camera* camera;
		void update();
		void render();
		void init();
		long long unsigned int milisecondsSinceEpoch();
	public:
		Engine(double targetFPS);
		void run();
		void exit();
		long long updatesSinceStart();	
};

#endif
