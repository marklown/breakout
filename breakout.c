/*

TODO:
- acceleration for paddle
- High scores
- BUG: at high speeds the ball passes through bricks
- Power-ups? Multi-ball, speed, paddle-size
- Balance, it gets too hard too fast
- Color changing ball based on what brick it hit?
- Bricks explode when hit

*/


#include "gui.h"
#include "linked_list.h"
#include "scores.h"
#include "util.h"
#include "SDL_FontCache.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 640
#define HEIGHT 480
#define BRICK_WIDTH 40
#define BRICK_HEIGHT 12
#define NUM_BRICK_ROWS 6
#define START_BALL_SPEED 300
#define START_PADDLE_WIDTH 64
#define START_PLAYER_LIVES 3
#define HUD_Y_MAX 18


typedef struct {
    float x, y;
} vec_t;

typedef struct {
    uint8_t r, g, b;
} color_t;

typedef struct {
    vec_t position;
    vec_t size;
} paddle_t;


typedef struct {
    vec_t position;
    vec_t velocity;
    vec_t size;
} ball_t;

typedef struct {
    vec_t position;
    color_t color;
    bool active;
} brick_t;

typedef enum {
    STATE_IDLE,
    STATE_PLAYING,
    STATE_LEVEL_COMPLETE,
    STATE_LIFE_LOST,
    STATE_GAME_OVER,
} gamestate_t;


static void init_window(void);
static void close_window(void);
static void game_loop(void);
static void setup_new_game(void);
static void render_game(void);
static void update_paddle(SDL_Event* evt);
static gamestate_t update_ball(void);
static void normalize(vec_t* vec);
static void reset_paddle_and_ball(void);
static void print_msg(char* msg);
static void clear_msg(void);
static void render_high_scores(void);

static gamestate_t step_idle(SDL_Event* evt);
static gamestate_t step_playing(SDL_Event* evt);
static gamestate_t step_level_complete(SDL_Event* evt);
static gamestate_t step_life_lost(SDL_Event* evt);
static gamestate_t step_game_over(SDL_Event* evt);

// Game variables
static SDL_Window* window;
static SDL_Renderer* renderer;
static FC_Font* font;
static gamestate_t game_state = STATE_IDLE;
static double delta_time = 0;
static paddle_t paddle;
static ball_t ball;
static brick_t bricks[WIDTH/BRICK_WIDTH*NUM_BRICK_ROWS];
static float paddle_speed = 300.0f;
static float ball_speed = START_BALL_SPEED;
static int paddle_width = START_PADDLE_WIDTH;
static int player_lives = START_PLAYER_LIVES;
static int level = 1;
static int num_bricks_removed = 0;
static char* message = "\0";
static score_t high_scores[NUM_HIGH_SCORES];
static bool show_high_scores = false;

void init_window(void)
{
    window = SDL_CreateWindow("breakout", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    font = FC_CreateFont();
    FC_LoadFont(font, renderer, "BigBlue_TerminalPlus.TTF", 12, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);
}

void close_window(void)
{
    FC_FreeFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void on_clicked(void)
{
	printf("clicked\n");
}

void game_loop(void)
{
    uint64_t now = SDL_GetPerformanceCounter();
    uint64_t last = 0;
    bool running = true;

    setup_new_game();

	SDL_Rect r = make_rect(100, 200, 100, 25);
	gui_add_text_box(&r, "test");

    while (running) {
        SDL_Event event;
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) return;

        last = now;
        now = SDL_GetPerformanceCounter();
        delta_time = 0.001*((now - last)*1000 / (double)SDL_GetPerformanceFrequency());

        switch (game_state) {
        case STATE_IDLE:
            game_state = step_idle(&event);
            break;
        case STATE_PLAYING:
            game_state = step_playing(&event);
            break;
        case STATE_LEVEL_COMPLETE:
            game_state = step_level_complete(&event);
            break;
        case STATE_LIFE_LOST:
            game_state = step_life_lost(&event);
            break;
        case STATE_GAME_OVER:
            game_state = step_game_over(&event);
            break;
        default:
            exit(1);
            break;
        }

        render_game();
    }
}

gamestate_t step_idle(SDL_Event* evt)
{
    print_msg("Press <space> to start");
	gui_update(evt);
    if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_SPACE) {
        clear_msg();
		show_high_scores = false;
        return STATE_PLAYING;
    }
    return STATE_IDLE;
}

gamestate_t step_playing(SDL_Event* evt)
{
    update_paddle(evt);
    return update_ball();
}

gamestate_t step_level_complete(SDL_Event* evt)
{
    paddle_width = (int)((float)paddle_width * 0.8f);
    ball_speed *= 1.2f;
    player_lives = START_PLAYER_LIVES;
    level++;
    setup_new_game();
    return STATE_IDLE;
}

gamestate_t step_life_lost(SDL_Event* evt)
{
    reset_paddle_and_ball();
    player_lives--;
    if (player_lives <= 0) {
        return STATE_GAME_OVER;
    }
    return STATE_IDLE;
}

gamestate_t step_game_over(SDL_Event* evt)
{
    player_lives = START_PLAYER_LIVES;
    paddle_width = START_PADDLE_WIDTH;
    ball_speed = START_BALL_SPEED;
    level = 0;
    setup_new_game();
	show_high_scores = true;
    return STATE_IDLE;
}

void update_paddle(SDL_Event* evt)
{
    float dx = 0;
    if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_LEFT) {
        dx -= paddle_speed*delta_time;
    } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_RIGHT) {
        dx += paddle_speed*delta_time;
    }

    if (paddle.position.x+dx < 0) {
        paddle.position.x = 0;
    } else if (paddle.position.x+paddle.size.x+dx > WIDTH) {
        paddle.position.x = WIDTH-paddle.size.x;
    } else {
        paddle.position.x += dx;
    }
}

gamestate_t update_ball(void)
{
    normalize(&ball.velocity);
    vec_t next_pos;
    next_pos.x = ball.position.x+ball.velocity.x*ball_speed*delta_time;
    next_pos.y = ball.position.y+ball.velocity.y*ball_speed*delta_time;

    SDL_Rect ball_rect;
    SDL_Rect paddle_rect;

    ball_rect.x = next_pos.x;
    ball_rect.y = next_pos.y;
    ball_rect.w = ball.size.x;
    ball_rect.h = ball.size.y;

    paddle_rect.x = paddle.position.x;
    paddle_rect.y = paddle.position.y;
    paddle_rect.w = paddle.size.x;
    paddle_rect.h = paddle.size.y;

    if (SDL_HasIntersection(&ball_rect, &paddle_rect)) {
        // Bounce off paddle
        float x = (next_pos.x+ball.size.x/2)-(paddle.position.x+paddle.size.x/2);
        ball.velocity.x = x/(paddle.size.x/2);
        ball.velocity.y = -ball.velocity.y;
        next_pos.x = ball.position.x+ball.velocity.x*ball_speed*delta_time;
        next_pos.y = ball.position.y+ball.velocity.y*ball_speed*delta_time;
    } else if (next_pos.x < 0 || (next_pos.x+ball.size.x > WIDTH)) {
        // Bounce off left/right walls
        ball.velocity.x = -ball.velocity.x;
        next_pos.x = ball.position.x+ball.velocity.x*ball_speed*delta_time;
    } else if (next_pos.y < HUD_Y_MAX) {
        // Bounce off top
        ball.velocity.y = -ball.velocity.y;
        next_pos.y = ball.position.y+ball.velocity.y*ball_speed*delta_time;
    } else if (next_pos.y+ball.size.y > HEIGHT) {
        // Missed the paddle
        return STATE_LIFE_LOST;
    } else {
        // Check for collision with bricks
        for (int i=0;i<sizeof(bricks)/sizeof(brick_t);i++) {
            SDL_Rect brick_rect;
            brick_rect.x = bricks[i].position.x;
            brick_rect.y = bricks[i].position.y;
            brick_rect.w = BRICK_WIDTH;
            brick_rect.h = BRICK_HEIGHT;
            if (SDL_HasIntersection(&ball_rect, &brick_rect) && bricks[i].active) {
                ball.velocity.y = -ball.velocity.y;
                next_pos.y = ball.position.y+ball.velocity.y*ball_speed*delta_time;
                bricks[i].active = false;
                num_bricks_removed++;
                if (num_bricks_removed >= sizeof(bricks)/sizeof(brick_t)) {
                    return STATE_LEVEL_COMPLETE;
                }
                break;
            }
        }
    }

    // Update the balls position
    ball.position = next_pos;

    return STATE_PLAYING;
}

void reset_paddle_and_ball(void)
{
    paddle.size.x = paddle_width;
    paddle.size.y = 12;
    paddle.position.x = WIDTH/2-paddle.size.x/2;
    paddle.position.y = HEIGHT-paddle.size.y;

    ball.size.x = 10;
    ball.size.y = 10;
    ball.position.x = WIDTH/2-ball.size.x/2;
    ball.position.y = HEIGHT/2-ball.size.y/2;
    ball.velocity.x = 0;
    ball.velocity.y = 1.0f;
}

void setup_new_game(void)
{
    srand(time(0));

    reset_paddle_and_ball();

    int index = 0;
    for (int i=0;i<NUM_BRICK_ROWS;i++) {
        color_t color;
        color.r = 64+rand()%192;
        color.g = 64+rand()%192;
        color.b = 64+rand()%192;
        for (int j=0;j<WIDTH/BRICK_WIDTH;j++) {
            brick_t brick;
            brick.position.x = j*BRICK_WIDTH;
            brick.position.y = i*BRICK_HEIGHT+HUD_Y_MAX;
            brick.color = color;
            brick.active = true;
            bricks[index++] = brick;
        }
    }

    num_bricks_removed = 0;

	read_high_scores(high_scores);
	show_high_scores = true;
}

void render_paddle(void)
{
    SDL_Rect rect = {(int)paddle.position.x, (int)paddle.position.y, (int)paddle.size.x, (int)paddle.size.y};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void render_ball(void)
{
    SDL_Rect rect = {(int)ball.position.x, (int)ball.position.y, (int)ball.size.x, (int)ball.size.y};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void render_bricks(void)
{
    for (int i=0;i<sizeof(bricks)/sizeof(brick_t);i++) {
        if (bricks[i].active) {
            SDL_Rect rect = {(int)bricks[i].position.x, (int)bricks[i].position.y, BRICK_WIDTH, BRICK_HEIGHT};
            SDL_SetRenderDrawColor(renderer, bricks[i].color.r, bricks[i].color.g, bricks[i].color.b, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void render_hud(void)
{
    SDL_Rect hud_background;
    hud_background.x = 0;
    hud_background.y = 0;
    hud_background.w = WIDTH;
    hud_background.h = HUD_Y_MAX;
    SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
    SDL_RenderFillRect(renderer, &hud_background);
    char buf[64];
    sprintf(buf, "Level: %d    Lives: %d", level, player_lives);
    FC_Draw(font, renderer, 0, 3, buf);
    FC_Draw(font, renderer, WIDTH/2-88, 3, message);
}

static void render_high_scores(void)
{
	FC_Draw(font, renderer, WIDTH/2-60, HEIGHT/3, "High scores:");
	for (int i=0;i<NUM_HIGH_SCORES;i++) {
		char buf[64];
		sprintf(buf,"%s  -  %d",high_scores[i].name,high_scores[i].value);
		if (high_scores[i].value!=0) {
			FC_Draw(font, renderer, WIDTH/2-60, HEIGHT/3+30+i*20, buf);
		}
	}	
}

void render_game(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    render_bricks();
    render_paddle();
	if (show_high_scores) render_high_scores();
	else render_ball();
    render_hud();
	gui_render(renderer, font);
    SDL_RenderPresent(renderer);
}

void normalize(vec_t* vec)
{
    vec->x /= sqrt(vec->x*vec->x+vec->y*vec->y);
    vec->y /= sqrt(vec->x*vec->x+vec->y*vec->y);
}

void print_msg(char* msg)
{
    message = msg;
}

void clear_msg(void)
{
    message = "";
}

int main(int argc, char* argv[])
{
    init_window();
    game_loop();
    close_window();
}
