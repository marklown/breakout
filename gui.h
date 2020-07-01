#ifndef GUI_H
#define GUI_H

#include <SDL2/SDL.h>

typedef void (*on_button_clicked_t)(void);

int gui_add_button(SDL_Rect* rect, const char* label, on_button_clicked_t callback);
void gui_update(SDL_Event* evt);
void gui_render(SDL_Renderer* renderer);


#endif // GUI_H
