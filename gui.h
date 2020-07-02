#ifndef GUI_H
#define GUI_H

#include "SDL_FontCache.h"
#include <SDL2/SDL.h>

typedef void (*on_button_clicked_t)(void);

void gui_add_button(SDL_Rect* rect, const char* label, on_button_clicked_t callback);
void gui_add_text_box(SDL_Rect* rect, const char* optional_text);
void gui_update(SDL_Event* evt);
void gui_render(SDL_Renderer* renderer, FC_Font* font);
void gui_remove_all(void);


#endif // GUI_H
