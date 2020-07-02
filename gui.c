
#include "gui.h"
#include "linked_list.h"
#include <string.h>
#include <stdbool.h>


typedef struct {
	SDL_Rect rect;
	char label[32];
	on_button_clicked_t callback;
	bool is_mouse_over;	
} gui_button_t;


static list_t* button_list = NULL;

void gui_remove_all(void)
{
	list_clear(button_list);
}

void gui_add_button(SDL_Rect* rect, const char* label, on_button_clicked_t callback)
{
	if (button_list == NULL) {
		button_list = list_create();
	}	
	gui_button_t button;
	memset(&button, 0, sizeof(gui_button_t));
	memcpy(&button.rect, rect, sizeof(SDL_Rect));
	strcpy(button.label, label);
	button.callback = callback;
	list_add(&button_list, &button, sizeof(gui_button_t));
}

void gui_update(SDL_Event* evt)
{
	SDL_Point mouse_point;
	Uint32 mouse_state = SDL_GetMouseState(&mouse_point.x, &mouse_point.y);
	list_t* next = button_list;
	while (next != NULL) {
		gui_button_t* button = (gui_button_t*)next->data;
		if (button == NULL) return;
		if (SDL_PointInRect(&mouse_point, &button->rect)) {
			button->is_mouse_over = true;
			if (mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
				button->callback();
			}
		} else {
			button->is_mouse_over = false;
		}
		next = next->next;	
	}
}

void gui_render(SDL_Renderer* renderer, FC_Font* font)
{
	list_t* next = button_list;
	while (next != NULL) {
		gui_button_t* button = (gui_button_t*)next->data;
		if (button == NULL) return;
		if (button->is_mouse_over) {
			SDL_SetRenderDrawColor(renderer,200,200,200,255);
		} else {
			SDL_SetRenderDrawColor(renderer,128,128,128,255);
		}
		SDL_RenderFillRect(renderer,&button->rect);
		FC_Draw(font, renderer, button->rect.x+16,
			button->rect.y+button->rect.h/2-7, button->label);
		next = next->next;
	}
}
