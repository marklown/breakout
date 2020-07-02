#ifndef UTIL_H
#define UTIL_H

#include <SDL2/SDL.h>

extern inline SDL_Rect make_rect(int x, int y, int w, int h)
{
	SDL_Rect r;
	r.x = x;
	r.y = y;
	r.w = y;
	r.h = h;
	return r;
}

#endif // UTIL_H
