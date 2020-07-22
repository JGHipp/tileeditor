#ifndef TILEMAP_H
#define TILEMAP_H

#include "Texture.hpp"
#include "Graphics.hpp"
#include "Camera.hpp"
#include <vector>

class Tilemap
{
	private: 
		int* tileData;
		bool tileInRange(int x, int y);
	public: 
		Texture* tilemapTexture;
		Texture** tileTextures;
		Tilemap(int tileSize, const char* textureFile);
		int width, height, tileSize;
		void init();
		void loadData(const char* pathToLevelFile);
		void exportToFile(const char* pathToLevelFile, const char* levelName);
		void render(Graphics* graphics, Camera* camera);
		int getTileId(int x, int y);
		void setTile(int x, int y, int id);
		Rectangle* getTileRectangle(int x, int y);
};

#endif
