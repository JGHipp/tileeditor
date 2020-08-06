#include "Engine.hpp"
#include <iostream>
#include <chrono>		// std::chrono
#ifdef __MINGW32__		// Add suport for cross-compiling
#include <mingw.thread.h>
#else
#include <thread>		// std::this_thread::sleep_for()
#endif
#include <SDL2/SDL.h>	// Simple Direct Media Layer

Engine::Engine(double targetFPS): selectMode(false), canToggle(true), selectedTile(1), targetFPS(targetFPS), updates(0)
{
	std::cout << "Bleak Editor v1.1" << std::endl;
}

void Engine::run()
{
	init();
	double milisecondsPerFrame = 1000 / targetFPS;
	unsigned long long startTime, sleepFor;
	while(running)
	{
		startTime = milisecondsSinceEpoch();
		update();
		render();
		updates++;
		sleepFor = milisecondsPerFrame - (milisecondsSinceEpoch() - startTime);
		// Sleep for time neccesary to run at target FPS
		if(sleepFor > 0) std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
	}
	std::cout << "Bleak Editor terminated" << std::endl;
	exit();
}

int Engine::getMouseX() { return mouse->getX() / graphics->windowScale; }
int Engine::getMouseY() { return mouse->getY() / graphics->windowScale; }
int Engine::getMouseTileX() { return (getMouseX() + camera->getX()) / tilemap->tileSize; } 
int Engine::getMouseTileY() { return  (getMouseY() + camera->getY()) / tilemap->tileSize; }

int nFlooded = 0;
const int MAX_FLOOD = 2000;

void Engine::floodfill(int x, int y, int fill, int before) 
{ 
    if(x >= 0 && y >= 0 && x < tilemap->width && y < tilemap->width && tilemap->getTileId(x, y) == before && nFlooded < MAX_FLOOD) 
    { 
    	nFlooded++;
        tilemap->setTile(x, y, fill); 
        floodfill(x + 1, y, fill, before); 
        floodfill(x, y + 1, fill, before); 
        floodfill(x - 1, y, fill, before); 
        floodfill(x, y - 1, fill, before); 
    } 
} 

void Engine::update()
{
	SDL_Event event;
	while(SDL_PollEvent(&event) != 0) // Handle events
 	{
    	if(event.type == SDL_QUIT) running = false;
    	keyboard->update(event);
    	mouse->update(event);   	
	}
	if(keyboard->keyPressed(SDLK_t) && canToggle)
	{
		canToggle = false;
		selectMode = !selectMode;
	} 
	else if(!keyboard->keyPressed(SDLK_t)) canToggle = true;
	if(keyboard->keyPressed(SDLK_ESCAPE)) selectMode = false;
	// Move the camera
	const double CAMERA_SPEED = 3;
	if(keyboard->keyPressed(SDLK_w)) camera->setDy(-CAMERA_SPEED);
	else if(keyboard->keyPressed(SDLK_s)) camera->setDy(CAMERA_SPEED);
	else camera->setDy(0);
	if(keyboard->keyPressed(SDLK_a)) camera->setDx(-CAMERA_SPEED);
	else if(keyboard->keyPressed(SDLK_d)) camera->setDx(CAMERA_SPEED);
	else camera->setDx(0);
	camera->update();
	if(keyboard->keyPressed(SDLK_F1))
	{
		std::string path, levelName;
		std::cout << "Assign a level ID string: ";
		std::cin >> levelName;
		std::cout << "Export '" << levelName << "' to (path): ";
		std::cin >> path;
		std::cout << "Exporting '" << levelName << "' to '" << path << "'... ";
		tilemap->exportToFile(path.c_str(), levelName.c_str());
		std::cout << "Done!" << std::endl;
	}
	else if(keyboard->keyPressed(SDLK_f))
	{
		floodfill(getMouseTileX(), getMouseTileY(), selectedTile, tilemap->getTileId(getMouseTileX(), getMouseTileY()));
		nFlooded = 0;
	}
	if(mouse->isPressed())
	{
		const int selectionLine = graphics->renderBuffer->width - tilemap->tilemapTexture->width;
		int mouseX = mouse->getX() / graphics->windowScale;
		int mouseY = mouse->getY() / graphics->windowScale;
		if(mouseX < selectionLine || !selectMode) tilemap->setTile(getMouseTileX(), getMouseTileY(), selectedTile);
		else
		{
			int tx = (getMouseX() - selectionLine) / tilemap->tileSize, ty = mouseY / tilemap->tileSize;
			int width = tilemap->tilemapTexture->width / tilemap->tileSize, height = tilemap->tilemapTexture->height / tilemap->tileSize; 
			int id = tx + ty * width;
			if(id < width * height) selectedTile = tx + ty * width;
		}
	}
}

const u32 BACKGROUND_COLOR = 0x0D0D0D, LINE_COLOR = 0x00FF08, SELECTION_COLOR = 0xFF0000, MOUSE_TILE_COLOR = 0xFFFFFF;

void Engine::render()
{
	graphics->clearBuffer();
	graphics->drawRectangle(0, 0, graphics->renderBuffer->width, graphics->renderBuffer->height, BACKGROUND_COLOR);
	tilemap->render(graphics, camera);
	graphics->drawRectangleOutline(0, 0, tilemap->width * tilemap->tileSize, tilemap->height * tilemap->tileSize, LINE_COLOR, camera);
	graphics->drawRectangleOutline(getMouseTileX() * tilemap->tileSize, getMouseTileY() * tilemap->tileSize, 
		tilemap->tileSize, tilemap->tileSize, MOUSE_TILE_COLOR, camera);
	if(selectMode)
	{
		graphics->drawLine(graphics->renderBuffer->width - tilemap->tilemapTexture->width - 1, 0, 
			graphics->renderBuffer->width - tilemap->tilemapTexture->width, graphics->renderBuffer->height, LINE_COLOR);
		graphics->drawTexture(tilemap->tilemapTexture, graphics->renderBuffer->width - tilemap->tilemapTexture->width, 0);
		graphics->drawRectangle(graphics->renderBuffer->width - tilemap->tilemapTexture->width, tilemap->tilemapTexture->height, 
			tilemap->tilemapTexture->width, tilemap->tilemapTexture->height, 0xFF00FF);
		graphics->drawRectangleOutline(selectedTile % (tilemap->tilemapTexture->width / tilemap->tileSize) * tilemap->tileSize +
			graphics->renderBuffer->width - tilemap->tilemapTexture->width, selectedTile / (tilemap->tilemapTexture->width / 
			tilemap->tileSize) * tilemap->tileSize, tilemap->tileSize + 1, tilemap->tileSize + 1, SELECTION_COLOR);		
	}
	graphics->drawRectangleOutline(0, 0, tilemap->tileSize + 2, tilemap->tileSize + 2, SELECTION_COLOR);
	graphics->drawTexture(tilemap->tileTextures[selectedTile], 1, 1, 0xFF00FF);
	graphics->updateWindow();
}

void Engine::init()
{
	running = true;
	SDL_Init(SDL_INIT_EVERYTHING);
	keyboard = new Keyboard();
	mouse = new Mouse();
	tilemap = new Tilemap(16, "res/tilesheet.bmp");
	graphics = new Graphics(480, 280, 2.0, "Bleak Tile Editor");
	camera = new Camera(graphics->renderBuffer->width / 2, graphics->renderBuffer->height / 2);
	camera->setMode(1);
}

void Engine::exit()
{
	graphics->destroy();
	SDL_Quit();
}

long long Engine::updatesSinceStart() { return updates; }
unsigned long long Engine::milisecondsSinceEpoch()
{ return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1); }

