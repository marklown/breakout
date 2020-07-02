
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

typedef struct {
	SDL_Rect rect;
	char text[1024];	
} gui_text_box_t;


static list_t* button_list = NULL;
static list_t* text_box_list = NULL;

void gui_remove_all(void)
{
	list_clear(button_list);
	list_clear(text_box_list);
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

void gui_add_text_box(SDL_Rect* rect, const char* optional_text)
{
	if (text_box_list == NULL) {
		text_box_list = list_create();
	}
	gui_text_box_t text_box;
	memset(&text_box, 0, sizeof(gui_text_box_t));
	memcpy(&text_box.rect, rect, sizeof(SDL_Rect));
	strcpy(text_box.text, optional_text);
	list_add(&text_box_list, &text_box, sizeof(gui_text_box_t));
}

static void update_buttons(SDL_Event* evt, SDL_Point* mouse_point, Uint32 mouse_state)
{
	list_t* next = button_list;
	while (next != NULL) {
		gui_button_t* button = (gui_button_t*)next->data;
		if (button == NULL) return;
		if (SDL_PointInRect(mouse_point, &button->rect)) {
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

static void update_text_boxes(SDL_Event* evt, SDL_Point* mouse_point, Uint32 mouse_state)
{
	list_t* next = text_box_list;
	while (next != NULL) {
		gui_text_box_t* text_box = (gui_text_box_t*)next->data;
		if (text_box == NULL) return;
		next = next->next;	
	}
}

void gui_update(SDL_Event* evt)
{
	SDL_Point mouse_point;
	Uint32 mouse_state = SDL_GetMouseState(&mouse_point.x, &mouse_point.y);
	update_buttons(evt, &mouse_point, mouse_state);
	update_text_boxes(evt, &mouse_point, mouse_state);
}

static void render_buttons(SDL_Renderer* renderer, FC_Font* font)
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

static void render_text_boxes(SDL_Renderer* renderer, FC_Font* font)
{
	SDL_SetRenderDrawColor(renderer,128,128,128,255);
	list_t* next = text_box_list;
	while (next != NULL) {
		gui_text_box_t* text_box = (gui_text_box_t*)next->data;
		if (text_box == NULL) return;
		SDL_RenderFillRect(renderer,&text_box->rect);
		FC_Draw(font, renderer, text_box->rect.x+2,
			text_box->rect.y+text_box->rect.h/2-7, text_box->text);
		next = next->next;
	}
}

void gui_render(SDL_Renderer* renderer, FC_Font* font)
{
	render_buttons(renderer, font);
	render_text_boxes(renderer, font);
}
