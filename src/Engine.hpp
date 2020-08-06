#ifndef Engine_H
#define Engine_H

#include "gfx/Graphics.hpp"
#include "ui/Keyboard.hpp"
#include "ui/Mouse.hpp"
#include "level/Tilemap.hpp"
#include "level/Camera.hpp"

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
		void floodfill(int x, int y, int oldTile, int newTile);
		long long unsigned int milisecondsSinceEpoch();
	public:
		Engine(double targetFPS);
		int getMouseX();
		int getMouseY();
		int getMouseTileX(); 
		int getMouseTileY();
		void run();
		void exit();
		long long updatesSinceStart();	
};

#endif
