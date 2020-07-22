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
	std::cout << "Bleak Editor v1.0" << std::endl;
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

	if(mouse->isPressed())
	{
		const int selectionLine = graphics->renderBuffer->width - tilemap->tilemapTexture->width;
		int mouseX = mouse->getX() / graphics->windowScale;
		int mouseY = mouse->getY() / graphics->windowScale;
		if(mouseX < selectionLine || !selectMode)
		{
			int tx = (mouseX + camera->getX()) / tilemap->tileSize;
			int ty = (mouseY + camera->getY()) / tilemap->tileSize;
			tilemap->setTile(tx, ty, selectedTile);
		}
		else
		{
			int tx = (mouseX - selectionLine) / tilemap->tileSize;
			int ty = mouseY / tilemap->tileSize;
			int id = tx + ty * (tilemap->tilemapTexture->width / tilemap->tileSize);
			if(id < (tilemap->tilemapTexture->width / tilemap->tileSize) * (tilemap->tilemapTexture->height / tilemap->tileSize))
				selectedTile = id;
		}
	}
}

void Engine::render()
{
	const u32 lineColor = 0x00FF08;
	const u32 backgroundColor = 0x0D0D0D;
	graphics->clearBuffer();
	graphics->drawRectangle(0, 0, graphics->renderBuffer->width, graphics->renderBuffer->height, backgroundColor);
	tilemap->render(graphics, camera);
	graphics->drawRectangleOutline(0, 0, tilemap->width * tilemap->tileSize, tilemap->height * tilemap->tileSize, lineColor, camera);
	if(selectMode)
	{
		graphics->drawLine(graphics->renderBuffer->width - tilemap->tilemapTexture->width - 1, 0, 
			graphics->renderBuffer->width - tilemap->tilemapTexture->width, graphics->renderBuffer->height, lineColor);
		graphics->drawTexture(tilemap->tilemapTexture, graphics->renderBuffer->width - tilemap->tilemapTexture->width, 0);
		graphics->drawRectangle(graphics->renderBuffer->width - tilemap->tilemapTexture->width, tilemap->tilemapTexture->height, 
			tilemap->tilemapTexture->width, tilemap->tilemapTexture->height, 0xFF00FF);
		graphics->drawRectangleOutline(selectedTile % (tilemap->tilemapTexture->width / tilemap->tileSize) * tilemap->tileSize +
			graphics->renderBuffer->width - tilemap->tilemapTexture->width, selectedTile / (tilemap->tilemapTexture->width / 
			tilemap->tileSize) * tilemap->tileSize, tilemap->tileSize + 1, tilemap->tileSize + 1, lineColor);		
	}
	graphics->drawRectangleOutline(0, 0, tilemap->tileSize + 2, tilemap->tileSize + 2, lineColor);
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

