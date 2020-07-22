#include "Tilemap.hpp"
#include "Texture.hpp"
#include "JMath.hpp"
#include "Log.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

Tilemap::Tilemap(int tileSize, const char* textureFile): width(0), height(0), tileSize(tileSize)
{
	tilemapTexture = new Texture(textureFile);
	init();
}

void Tilemap::init()
{
	// Load texture for each tile
	int nTiles = (tilemapTexture->width / tileSize) * (tilemapTexture->height / tileSize);
	for(int id = 0; id < nTiles; id++)
	{
		int tilePosX = id % tileSize;
		int tilePosY = id / tileSize; 
		Texture* croppedTile = tilemapTexture->crop(tilePosX * tileSize, tilePosY * tileSize, tileSize, tileSize);
		// Initialize the tileTextures array assuming all tiles are equal size
		if(id == 0) tileTextures = (Texture**) malloc(sizeof(*croppedTile) * nTiles);
		tileTextures[id] = croppedTile;
	}
	// Load tilemap data
	char choice;
	std::cout << "Create (N)ew tilemap or (L)oad from file?: ";
	std::cin >> choice;
	if(toupper(choice) == 'L')
	{
		std::string path;
		std::cout << "Enter path to tilemap file: ";
		std::cin.ignore();
		std::getline(std::cin, path);
		loadData(path.c_str());
	}
	else
	{
		std::cout << "Enter width: ";
		std::cin >> width;
		std::cout << "Enter height: ";
		std::cin >> height;
		tileData = new int[width * height];
		for(int i = 0; i < width * height; i++) tileData[i] = 0;
		log("Created tilemap with dimensions %ix%i", width, height);
	}
}

/*
	Loads tile data from a BMP image
    First 4 rows of image are used to 
    declare colors for each tile ID
*/
void Tilemap::loadData(const char* pathToLevelFile)
{
	std::ifstream readFile(pathToLevelFile);
	if(!readFile.is_open()) error("Error reading tile data from '%s'", pathToLevelFile);
	std::vector<std::string> lines;
	std::string line;
	while(std::getline(readFile, line)) lines.push_back(line);
	log("Loading tilemap '%s'", lines.at(0).c_str());
	width = atoi(lines.at(1).c_str());
	height = atoi(lines.at(2).c_str());
	tileData = new int[width * height];
	for(int i = 3; i < (int) lines.size(); i++) tileData[i - 3] = atoi(lines.at(i).c_str());
}

void Tilemap::exportToFile(const char* path, const char* levelName)
{
	std::ofstream outputFile(path, std::ios_base::trunc);
	if(outputFile)
	{
		#define writeData(x) outputFile << x << std::endl;
		writeData(levelName);
		writeData(width);
		writeData(height);
		for(int i = 0; i < width * height; i++) writeData(tileData[i]);
		outputFile.close();
	} else error("Unable to write to file '%s'", path);
}

void Tilemap::render(Graphics* graphics, Camera* camera)
{
	// Only render the tiles that are in the view of the camera
	const int padding = 4;
	int nRowTiles = graphics->renderBuffer->width / tileSize;
	int nColTiles = graphics->renderBuffer->height / tileSize;
	int x1 = clamp((int) camera->getFocusX() / tileSize - (nRowTiles / 2) - padding, 0, width);
	int x2 = clamp((int) camera->getFocusX() / tileSize + (nRowTiles / 2) + padding, 0, width);
	int y1 = clamp((int) camera->getFocusY() / tileSize - (nColTiles / 2) - padding, 0, height);
	int y2 = clamp((int) camera->getFocusY() / tileSize + (nColTiles / 2) + padding, 0, height);
	for(int x = x1; x <= x2; x++)
	{
		for(int y = y1; y <= y2; y++)
			graphics->drawTexture(tileTextures[getTileId(x, y)], x * tileSize, y * tileSize, 0xFF00FF, camera);
	}
}


void Tilemap::setTile(int x, int y, int id)
{
	if(tileInRange(x, y) && id >= 0) tileData[x + y * width] = id;
}

int Tilemap::getTileId(int x, int y)
{
	if(tileInRange(x, y)) return tileData[x + y * width];
	return 0;
}

Rectangle* Tilemap::getTileRectangle(int x, int y) 
{ return new Rectangle(x * tileSize, y * tileSize, tileSize, tileSize); }
bool Tilemap::tileInRange(int x, int y) { return (x >= 0 && y >= 0 && x < width && y < height); }
