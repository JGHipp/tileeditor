#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include "../level/Camera.hpp"
#include "../util/JMath.hpp"
#include "Texture.hpp"

typedef Uint32 u32;
class Graphics
{
	private:
		SDL_Window* window; 		// Pointer to window 
		SDL_Surface* windowSurface; // imageSurface gets drawn to this and scaled
		SDL_Surface* imageSurface;  // What we draw to
	public:
		Texture* renderBuffer;		// Texture object representing imageSurface
		double windowScale;
		Graphics(int width, int height, double scale, const char* windowTitle);
		void toggleWindow();
		void destroy();
		void updateWindow();
		void clearBuffer();
		int setPixel(int x, int y, u32 color);
		void drawRectangle(int xPos, int yPos, int width, int height, u32 color);
		void drawRectangle(double xPos, double yPos, int width, int height, u32 color, Camera* camera);
		void drawRectangle(Rectangle rectangle, u32 color);
		void drawRectangle(Rectangle rectangle, u32 color, Camera* camera);
		void drawRectangleOutline(int xPos, int yPos, int width, int height, u32 color);
		void drawRectangleOutline(double xPos, double yPos, int width, int height, u32 color, Camera* camera);
		void drawRectangleOutline(Rectangle rectangle, u32 color);
		void drawRectangleOutline(Rectangle rectangle, u32 color, Camera* camera);
		void drawLine(int x1, int y1, int x2, int y2, u32 color);
		void drawLine(double x1, double y1, double x2, double y2, u32 color, Camera* camera);
		void drawVector(int x, int y, double angle, double magnitude, u32 color);
		void drawVector(double x, double y, double angle, double magnitude, u32 color, Camera* camera);
		void drawTexture(Texture* texture, int xPos, int yPos);
		void drawTexture(Texture* texture, double xPos, double yPos, Camera* camera);
		void drawTexture(Texture* texture, int xPos, int yPos, u32 transparent);
		void drawTexture(Texture* texture, double xPos, double yPos, u32 transparent, Camera* camera);
};

#endif
