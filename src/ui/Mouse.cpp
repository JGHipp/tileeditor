#include "Mouse.hpp"

Mouse::Mouse(): x(0), y(0), pressed(false)
{}

void Mouse::update(SDL_Event& event)
{
	if(event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
	{
		SDL_GetMouseState(&x, &y);
		if(event.type == SDL_MOUSEBUTTONDOWN) pressed = true;
		else if(event.type == SDL_MOUSEBUTTONUP) pressed = false;
	}
}

int Mouse::getX() { return x; }
int Mouse::getY() { return y; }
bool Mouse::isPressed() { return pressed; }
