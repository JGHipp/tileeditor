#ifndef MOUSE_H
#define MOUSE_H

#include <SDL2/SDL.h>

class Mouse
{
	private:
		int x, y;
		bool pressed;
	public:
		Mouse();
		void update(SDL_Event& event);
		bool isPressed();
		int getX();
		int getY();	
};

#endif
