
#include "gui.h"
#include <string.h>


typedef struct {
	SDL_Rect rect;
	char label[32];
	on_button_clicked_t callback;
} gui_button_t;

static gui_button_t buttons[16];
static int button_index = -1;

int gui_add_button(SDL_Rect* rect, const char* label, on_button_clicked_t callback)
{
	button_index++;
	int index = button_index;
	if (index >= 16) return -1;
	memcpy(&buttons[index].rect,rect,sizeof(SDL_Rect));
	strcpy(buttons[index].label,label);
	buttons[index].callback = callback;
	return index;
}

void gui_update(SDL_Event* evt)
{
	if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_RETURN) {
		buttons[0].callback();	
	}
}

void gui_render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer,32,62,255,255);
	SDL_RenderFillRect(renderer,&buttons[0].rect);
}
