#pragma warning(disable:4996)
#include <iostream>
#include <time.h>

//SDL headers
#include "SDL2/include/SDL.h"
#include "SDL2/include/SDL_image.h"
#include "SDL2/include/SDL_mixer.h"

//load libraries
#pragma comment(lib,"SDL2\\lib\\x86\\SDL2.lib")
#pragma comment(lib,"SDL2\\lib\\x86\\SDL2main.lib")
#pragma comment(lib,"SDL2\\lib\\x86\\SDL2_image.lib")
#pragma comment(lib,"SDL2\\lib\\x86\\SDL2_mixer.lib")
#pragma comment(linker,"/subsystem:console")

//other headers
#include "Player.h"
#include "Ball.h"
#include "Brick.h"
#include "Portal.h"

//functions
void spawn_Ball(Ball* b)
{
	b->x = 400.f;
	b->y = 300.f;
	b->w = 10;
	b->h = 10;
	b->xvel = 2.f - 1.f*rand() / RAND_MAX;
	b->yvel = 2.f - 1.f*rand() / RAND_MAX;
	b->is_outball = false;
	b->destroyed = false;
	b->timer = 0;
}

void despawn_Ball(Ball* b)
{
	b->x = 0.f;
	b->y = 0.f;
	b->w = 0;
	b->h = 0;
	b->xvel = 0;
	b->yvel = 0;
}

int ball_Direction(Ball* b, int num)
{
	spawn_Ball(b);

	switch (num)
	{
	case 1: //down right
		b->xvel;
		b->yvel;
		break;
	case 2: //up right
		b->xvel;
		b->yvel *= -1;
		break;
	case 3: //down left
		b->xvel *= -1;
		b->yvel;
		break;
	case 4: // up left
		b->xvel *= -1;
		b->yvel *= -1;
		break;
	}

	return 0;
}

void init_Bricks(Brick* b, int num, int lvl)
{
	for (int i = 0; i < num; i++)
	{
		b[i].w = 30;
		b[i].h = 20;

		if (i <= 24)
		{
			b[i].x = (790 - b[i].w * 25) / 2 + i * b[i].w;
			b[i].y = 80;
		}
		else if (i > 24 && i <= 49)
		{
			b[i].x = (790 - b[i].w * 25) / 2 + (i - 25) * b[i].w;
			b[i].y = 90 + b[i].h;
		}
		else if (i > 49 && i <= 74)
		{
			b[i].x = (790 - b[i].w * 25) / 2 + (i - 50) * b[i].w;
			b[i].y = 100 + b[i].h * 2;
		}
		else if (i > 74)
		{
			b[i].x = (790 - b[i].w * 25) / 2 + (i - 75) * b[i].w;
			b[i].y = 110 + b[i].h * 3;
		}

		if (lvl == 8 || lvl == 9)
		{
			b[i].x = rand() % 770 + 10;
			b[i].y = rand() % 110 + 80;
		}
	}
}

void uninit_Bricks(Brick* b)
{
	b->w = 0;
	b->h = 0;
	b->x = 0;
	b->y = 0;
}

void animate_Func(Portal* p, unsigned int time)
{
	if (time - p->frame_update > 100)
	{
		p->frame_update = time;

		//update
		p->img_source_y += p->img_source_h;
	}
	//reset
	if (p->img_source_y >= p->sprite_sheet) p->img_source_y = 0;
}

void teleport_Func(Portal* p, Ball* b, float xpos, float ypos, int size)
{
	b->xvel *= -1.f;
	b->yvel *= -1.f;
	p->x = rand() % 750 + 50;
	p->y = rand() % 550 + 120;
	p->hitbox_x = p->x;
	p->hitbox_y = p->y;
	b->x = p->hitbox_x + 10.f;
	b->y = p->hitbox_y - 10.f;

	if (p->x <= 0 || p->x >= 800)
	{
		p->init_Portal(xpos, ypos, size);
		spawn_Ball(b);
	}
	if (p->y <= 0 || p->y >= 600)
	{
		p->init_Portal(xpos, ypos, size);
		spawn_Ball(b);
	}
}

void set_Portals(Portal* p1, Portal* p2, Portal* p3, Portal* p4, bool _2p, int lvl)
{
	if (!_2p)
	{
		if (lvl > 2)
		{
			p1->init_Portal(400, 400, 20);
			p2->init_Portal(400, 500, 20);
		}
		if (lvl > 4)
		{
			p3->init_Portal(100, 450, 20);
			p4->init_Portal(700, 450, 20);
		}
	}
}

void draw_Ball(SDL_Renderer* renderer, Ball* b)
{
	SDL_SetRenderDrawColor(renderer, 255, 150, 0, 255);

	SDL_Rect rect;
	rect.w = b->w;
	rect.h = b->h;
	rect.x = b->x;
	rect.y = b->y;
	SDL_RenderFillRect(renderer, &rect);
}

void draw_Bricks(SDL_Renderer* renderer, Brick* b)
{
	SDL_SetRenderDrawColor(renderer, b->r, b->g, b->b, 255);

	SDL_Rect rect;
	rect.w = b->w;
	rect.h = b->h;
	rect.x = b->x;
	rect.y = b->y;
	SDL_RenderFillRect(renderer, &rect);
}

void draw_Text(SDL_Renderer *renderer, SDL_Texture *t, char *tex, int font_size, int dest_x, int dest_y)
{
	for (int i = 0; i < 16; i++)
	{
		//source
		SDL_Rect src;
		src.x = 64 * (tex[i] % 16);//column
		src.y = 64 * (tex[i] / 16);//row
		src.w = 64;
		src.h = 64;
		//destination
		SDL_Rect dest;
		dest.x = dest_x;
		dest.y = dest_y;
		dest.w = font_size;
		dest.h = font_size;

		//draw image
		//copy from source texture to destination screen.
		//SDL_FLIP_XXX enumeration allows you to mirror the image
		SDL_RenderCopyEx(renderer, t, &src, &dest, 0, NULL, SDL_FLIP_NONE);

		//increment dest_x!
		dest_x += dest.w;
	}
}

void draw_Text_Box(SDL_Renderer *renderer, int x, int y, int w, int h)
{
	//set area color to dark blue
	SDL_SetRenderDrawColor(renderer, 0, 0, 100, 150);
	SDL_Rect rect;
	rect.w = w;
	rect.h = h;
	rect.x = x;
	rect.y = y;

	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void draw_Slow_Guage(SDL_Renderer *renderer, int x, int y, int w, int h)
{
	//set area color to purple
	SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
	SDL_Rect rect;
	rect.w = w;
	rect.h = h;
	rect.x = x;
	rect.y = y;

	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

template<typename T, typename U>
int collision_Detection(T* t, U* u)
{
	//1 = below, 2 = left, 3 = above, 4 = right

	float width = 0.5f * (t->w + u->w);
	float height = 0.5f * (t->h + u->h);
	float dx = t->x - u->x + 0.5f * (t->w - u->w);
	float dy = t->y - u->y + 0.5f * (t->h - u->h);

	if (dx*dx <= width * width && dy*dy <= height * height)
	{
		float width_y = width * dy;
		float height_x = height * dx;

		if (width_y > height_x) return (width_y + height_x > 0) ? 3 : 4;
		else return (width_y + height_x > 0) ? 2 : 1;
	}

	return 0;
}

int portal_Hbox_Collision(Portal* p, Ball* b)
{
	//1 = below, 2 = left, 3 = above, 4 = right

	float width = 0.5f * (p->hitbox_w + b->w);
	float height = 0.5f * (p->hitbox_h + b->h);
	float dx = p->hitbox_x - b->x + 0.5f * (p->hitbox_w - b->w);
	float dy = p->hitbox_y - b->y + 0.5f * (p->hitbox_h - b->h);

	if (dx*dx <= width * width && dy*dy <= height * height)
	{
		float width_y = width * dy;
		float height_x = height * dx;

		if (width_y > height_x) return (width_y + height_x > 0) ? 3 : 4;
		else return (width_y + height_x > 0) ? 2 : 1;
	}

	return 0;
}

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	//window properties
	int screen_w = 800;
	int screen_h = 600;
	SDL_Window* window = SDL_CreateWindow("Breakout2.0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_w, screen_h, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	//sprite surfaces
	SDL_Surface* font_surface = IMG_Load("font_sheet.png");
	SDL_Surface* portal_surface = IMG_Load("portal.png");
	//sprite textures
	SDL_Texture* font_texture = SDL_CreateTextureFromSurface(renderer, font_surface);
	SDL_Texture* portal_texture = SDL_CreateTextureFromSurface(renderer, portal_surface);
	//free surfaces
	SDL_FreeSurface(font_surface);
	SDL_FreeSurface(portal_surface);

	//sound setup
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	//load sound effects
	Mix_Chunk* brick_hit = Mix_LoadWAV("impact-ringy_sfx.wav");
	Mix_Chunk* brick_break = Mix_LoadWAV("impact-hard_sfx.wav");
	Mix_Chunk* teleport_sfx = Mix_LoadWAV("air-twister_sfx.wav");

	//keyboard stuff
	unsigned char prev_key_state[256];
	unsigned char* keys = (unsigned char*)SDL_GetKeyboardState(NULL);
	const Uint8* state = SDL_GetKeyboardState(NULL);

	//frame rate stuff
	const int fps = 60;
	const int frame_delay = 1000 / fps;
	Uint32 frame_start;
	int frame_time;

	//flags
	bool running = true;
	bool paused = false;
	bool is_there_2p = false;
	bool lvl_complete = false;
	bool lvl_failed = false;

	//player stuff
	Player p1;
	p1.init_Player(150, 10, 0.f, 590.f, 0.f);
	Player p2;

	//ball stuff
	int max_balls = 10;
	int num_balls = 1;
	int active_balls = 0;
	int ball_count = 1;
	int lost_ball_limit = 2;
	int lost_balls = 0;
	int dir_change = 1;
	Ball* ball = (Ball*)malloc(sizeof(Ball)*max_balls);
	spawn_Ball(&ball[active_balls]);

	//brick stuff
	int n_bricks = 100;
	int row_bricks = 37;
	int destroyed_bricks = 0;
	Brick* brick = (Brick*)malloc(sizeof(Brick)*n_bricks);
	for (int i = 0; i < row_bricks; i++)
	{
		brick[i].destroyed = false;
		brick[i].tier = 1;
		brick[i].color_timer = 25;
		brick[i].hit = false;
	}
	
	//portal stuff
	int anim_spd = 0;
	Portal portal1;
	portal1.init_Source_Img(0, 0, 32, 32, 5, anim_spd, SDL_GetTicks());
	Portal portal2;
	portal2.init_Source_Img(0, 0, 32, 32, 5, anim_spd, SDL_GetTicks());
	Portal portal3;
	portal3.init_Source_Img(0, 0, 32, 32, 5, anim_spd, SDL_GetTicks());
	Portal portal4;
	portal4.init_Source_Img(0, 0, 32, 32, 5, anim_spd, SDL_GetTicks());

	//text stuff
	char pause_text[17];
	char paused_menu[17];
	char resume_text[17];
	char quit_text[17];
	char level_text[17];
	char score_text[17];
	char multiplier_text[17];
	char nballs_text[17];
	char doubles_text[17];
	char timer_text[17];
	char finish_text[17];
	char retry_text[17];
	char continue_text[17];
	char title_text[17];
	char start_text[17];
	char back_text[17];
	char slow_text[17];
	char lost_balls_text[17];
	char skip_text[17];
	char end_game_text[17];
	char final_score_text[17];
	char lost_balls_num[17];
	char bonus_text[17];
	sprintf(pause_text, "P-Pause");
	sprintf(paused_menu, "Paused");
	sprintf(resume_text, "R-Resume");
	sprintf(retry_text, "R-Retry");
	sprintf(continue_text, "C-Continue");
	sprintf(quit_text, "Q-Quit");
	sprintf(p1.controls, "A,D-Move");
	sprintf(p2.controls, "Arrows-Move");
	sprintf(title_text, "Breakout 2.0");
	sprintf(start_text, "S-Start");
	sprintf(back_text, "B-Back to title");
	sprintf(slow_text, "S-Slow");
	sprintf(skip_text, "S-Skip Level");
	sprintf(end_game_text, "Game Completed");

	//misc
	int level = 0;
	int lvl_timer = 200000;
	int lvl1_timer = 1000;
	int p1_display_timer = 300;
	int p2_display_timer = 300;
	int score = 0;
	int multiplier = 1;
	int bonus_points = 10000;
	int final_score = 0;
	int completed_levels = 0;
	float slow_guage = 100.f;
	srand(time(0));

	while (running)
	{
		//copies contents of keys to prev_key_state
		memcpy(prev_key_state, keys, 256);

		//gets milliseconds at the start of a frame
		frame_start = SDL_GetTicks();

		//consume all window events first
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) exit(0);
		}

		//set window screen color to black
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		//clear screen
		SDL_RenderClear(renderer);

		//game stats etc.
		{
			sprintf(level_text, "Level%d", level);
			sprintf(nballs_text, "Balls:%d", ball_count);
			sprintf(timer_text, "Time:%d", lvl_timer);
			sprintf(score_text, "Score:%d", score);
			sprintf(multiplier_text, "Multiplier:%d", multiplier);
			sprintf(lost_balls_text, "Dont Lose%d", lost_ball_limit);

			if (!is_there_2p) sprintf(doubles_text, "Shift-2p mode");
			else if (is_there_2p) sprintf(doubles_text, "Space-1p mode");

			if (lvl_failed) sprintf(finish_text, "Level Failed");
			if (lvl_complete) sprintf(finish_text, "Level Completed");
			if (level == 10)
			{
				sprintf(score_text, "Your Score:%d", score);
				sprintf(lost_balls_num, "Balls Lost:%d", lost_balls);
				sprintf(bonus_text, "Bonus:%d", bonus_points);
				sprintf(final_score_text, "Total:%d", final_score);
			}
		}
		
		//pause and resume
		if (state[SDL_SCANCODE_P] && !lvl_complete && !lvl_failed && level != 0 && level != 10) paused = true;
		else if (state[SDL_SCANCODE_R]) paused = false;
		
		//quit
		if (state[SDL_SCANCODE_Q] && level == 0) running = false;

		//back to title screen
		if (state[SDL_SCANCODE_B])
		{
			if (paused || lvl_complete || lvl_failed || level >= 10)
			{
				level = 0;
				for (int i = 0; i < n_bricks; i++)
				{
					brick[i].destroyed = false;
					brick[i].tier = 1;
					uninit_Bricks(&brick[i]);
				}
				row_bricks = 37;
				destroyed_bricks = 0;
				for (int i = 0; i < max_balls; i++) despawn_Ball(&ball[i]);
				active_balls = 0;
				ball_count = 1;
				lost_balls = 0;
				score = 0;
				final_score = 0;
				multiplier = 1;
				bonus_points = 10000;
				slow_guage = 100.f;
				dir_change = rand() % 4 + 1;
				completed_levels = 0;
				ball_Direction(&ball[active_balls], dir_change);
				paused = false;
				lvl_failed = false;
				lvl_complete = false;
			}
		}

		if (!paused && !lvl_complete && !lvl_failed && level != 0)
		{
			//level modifiers
			{
				switch (level) {
				case 1:
					num_balls = 1;
					if (ball_count == 1)
					{
						if (lvl1_timer > 0) lvl1_timer -= 1;
						if (lvl1_timer == 0)
						{
							for (int i = 0; i < row_bricks; i++)
							{
								//moving bricks
								brick[i].x += 1.f;
								if (brick[i].x >= screen_w - brick[i].w) brick[i].x = 0;
							}
						}
					}
					break;
				case 2:
					num_balls = 3;
					for (int i = 0; i < row_bricks; i++) brick[i].y += 0.025f; //falling bricks
					break;
				case 3:
					num_balls = 3;
					for (int i = 0; i < row_bricks; i++)
					{
						brick[i].x += 1.f;
						if (brick[i].x >= screen_w - brick[i].w) brick[i].x = 0;
					}
					break;
				case 4:
					num_balls = 3;
					for (int i = 0; i < row_bricks; i++)
					{
						if (is_there_2p) brick[i].y += 0.037f;
						else if (!is_there_2p) brick[i].y += 0.025f;
					}
					break;
				case 5:
					num_balls = 5;
					if (destroyed_bricks > 37)
					{
						for (int i = 0; i < row_bricks; i++)
						{
							brick[i].x += 1.f;
							if (brick[i].x >= screen_w - brick[i].w) brick[i].x = 0;
						}
					}
					break;
				case 6:
					num_balls = 5;
					for (int i = 0; i < row_bricks; i++)
					{
						if (is_there_2p) brick[i].y += 0.037f;
						else if (!is_there_2p) brick[i].y += 0.025f;
					}
					break;
				case 7:
					num_balls = 5;
					for (int i = 0; i < row_bricks; i++)
					{
						if (brick[i].tier == 3) brick[i].x += 1.f;
						else if (brick[i].tier == 2) brick[i].x += 1.5f;
						else if (brick[i].tier == 1) brick[i].x += 2.f;

						if (brick[i].x >= screen_w - brick[i].w) brick[i].x = 0;
					}
					break;
				case 8:
					num_balls = 5;
					for (int i = 0; i < row_bricks; i++)
					{
						if (brick[i].tier == 3) brick[i].y += 0.025f;
						else if (brick[i].tier == 2) brick[i].y += 0.075f;
						else if (brick[i].tier == 1) brick[i].y += 0.112f;
					}
					break;
				case 9:
					num_balls = 7;
					for (int i = 0; i < row_bricks; i++)
					{
						if (brick[i].tier == 3) brick[i].x += 1.f;
						else if (brick[i].tier == 2) brick[i].x += 1.5f;
						else if (brick[i].tier == 1) brick[i].x += 2.f;

						if (brick[i].x >= screen_w - brick[i].w) brick[i].x = 0;
					}
					break;
				}
			}
			
			//player movement and bounds
			{
				if (p1_display_timer > 0) p1_display_timer--;
				p1.move_Controls(state[SDL_SCANCODE_A], state[SDL_SCANCODE_D], 0, screen_w);
				if (is_there_2p) 
				{ 
					if (p2_display_timer > 0) p2_display_timer--;
					p2.move_Controls(state[SDL_SCANCODE_LEFT], state[SDL_SCANCODE_RIGHT], 0, screen_w); 
				}

				//enable 2p
				if (state[SDL_SCANCODE_RSHIFT] || state[SDL_SCANCODE_LSHIFT])
				{
					is_there_2p = true;
					p2.init_Player(75, 10, 725.f, 590.f, 0.f);
					p1.halve_Width(150);
				}
				//disable 2p
				if (state[SDL_SCANCODE_SPACE])
				{
					is_there_2p = false;
					p2.init_Player(0, 0, 725.f, 590.f, 0.f);
					p2_display_timer = 300;
					p1.double_Width(75);
				}
			}

			for (int i = 0; i < max_balls; i++)
			{
				//ball movement and bounds
				{
					if (!ball[i].destroyed) 
					{
						ball[i].x += ball[i].xvel;
						ball[i].y += ball[i].yvel;
					}

					if (ball[i].x <= 0 || ball[i].x >= screen_w - ball[i].w) ball[i].xvel *= -1.f;
					if (ball[i].y <= 0) ball[i].yvel *= -1.f;
					else if (ball[i].y > screen_h + 50 && !ball[i].destroyed) ball[i].is_outball = true;

					//subtract from the "# of balls" stat
					{
						if (ball[i].is_outball)
						{
							ball[i].timer++;

							if (ball[i].timer == 50)
							{
								ball[i].timer = 0;
								ball_count -= 1;
								multiplier -= 1;
								lost_balls += 1;
								ball[i].destroyed = true;
								if (lost_ball_limit > 0) lost_ball_limit -= 1;
								if (bonus_points > 0) bonus_points -= 500;
							}
						}

						if (ball[i].destroyed) ball[i].is_outball = false;
					}

					//slow down mechanic
					{
						if (!is_there_2p)
						{
							if (state[SDL_SCANCODE_S] && slow_guage > 0)
							{
								if (ball[i].xvel > 0) ball[i].xvel = 0.25f;
								else if (ball[i].xvel < 0) ball[i].xvel = -0.25f;
								if (ball[i].yvel > 0) ball[i].yvel = 0.25f;
								else if (ball[i].yvel < 0) ball[i].yvel = -0.25f;
								slow_guage -= 0.1f;
							}
							else if (!state[SDL_SCANCODE_S] && slow_guage < 100)
							{
								if (ball[i].xvel > 0) ball[i].xvel = 2.f - 1.f*rand() / RAND_MAX;
								else if (ball[i].xvel < 0) ball[i].xvel = -1*(2.f - 1.f*rand() / RAND_MAX);
								if (ball[i].yvel > 0) ball[i].yvel = 2.f - 1.f*rand() / RAND_MAX;
								else if (ball[i].yvel < 0) ball[i].yvel = -1*(2.f - 1.f*rand() / RAND_MAX);
								slow_guage += 0.01f;
							}
							else if (state[SDL_SCANCODE_S] && slow_guage <= 0) 
							{
								if (ball[i].xvel > 0) ball[i].xvel = 2.f - 1.f*rand() / RAND_MAX;
								else if (ball[i].xvel < 0) ball[i].xvel = -1 * (2.f - 1.f*rand() / RAND_MAX);
								if (ball[i].yvel > 0) ball[i].yvel = 2.f - 1.f*rand() / RAND_MAX;
								else if (ball[i].yvel < 0) ball[i].yvel = -1 * (2.f - 1.f*rand() / RAND_MAX);
							}
						}
					}

					//lost ball limit
					{
						if (level == 3 || level == 5 || level == 7 || level == 9)
						{
							if (lost_ball_limit == 0) lvl_failed = true;
						}
					}
				}

				//collision
				{
					//with players 
					{
						int p1_collision = collision_Detection(&p1, &ball[i]);
						int p2_collision = collision_Detection(&p2, &ball[i]);
						if (p1_collision == 3 || p2_collision == 3) ball[i].yvel *= -1.f;
						else if (p1_collision == 2 || p1_collision == 4 || p2_collision == 2 || p2_collision == 4)
						{
							ball[i].xvel *= -1.f;
							ball[i].yvel *= -1.f;
						}
					}
					
					//with bricks
					{
						for (int j = 0; j < row_bricks; j++)
						{
							int brick_collision = collision_Detection(&brick[j], &ball[i]);

							if (!brick[j].destroyed)
							{
								if (brick_collision != 0)
								{
									ball[i].yvel *= -1;
									dir_change = rand() % 4 + 1;
									brick[j].hit = true;

									if (brick[j].tier > 1)
									{
										brick[j].tier -= 1; 
										Mix_PlayChannel(-1, brick_hit, 0);
									}
									else if (brick[j].tier == 1)
									{
										Mix_PlayChannel(-1, brick_break, 0);
										brick[j].destroyed = true;
										destroyed_bricks += 1;
										score += 10 * multiplier;
									}

									if (active_balls < num_balls)
									{
										active_balls += 1;
										ball_count += 1;
										multiplier += 1;
										ball_Direction(&ball[active_balls], dir_change);
									}
									break;
								}
							}

							//end level
							{
								if (destroyed_bricks == row_bricks - 1) lvl_timer--;
								else if (destroyed_bricks == row_bricks)
								{
									brick[j].destroyed = true;
									lvl_timer = 200000;
									lvl_complete = true;
								}
								if (lvl_timer == 0) destroyed_bricks += 1;
								if (ball_count == 0 || brick[j].y > screen_h && !brick[j].destroyed) lvl_failed = true;
							}
						}
					}

					//with other balls
					{
						for (int k = 0; k < max_balls; k++)
						{
							int ball_collision = collision_Detection(&ball[k], &ball[i]);
							if (ball_collision == 1 || ball_collision == 3)
							{
								ball[i].yvel *= -1.f;
								ball[k].yvel *= -1.f;
							}
							else if (ball_collision == 2 || ball_collision == 4)
							{
								ball[i].xvel *= -1.f;
								ball[k].xvel *= -1.f;
							}
						}
					}

					//with portals
					{
						int portal1_collision = portal_Hbox_Collision(&portal1, &ball[i]);
						int portal2_collision = portal_Hbox_Collision(&portal2, &ball[i]);
						int portal3_collision = portal_Hbox_Collision(&portal3, &ball[i]);
						int portal4_collision = portal_Hbox_Collision(&portal4, &ball[i]);

						if (!is_there_2p)
						{
							if (level > 2)
							{
								if (portal1_collision != 0)
								{
									teleport_Func(&portal1, &ball[i], 400, 400, 20);
									Mix_PlayChannel(-1, teleport_sfx, 0);
								}
								if (portal2_collision != 0)
								{ 
									teleport_Func(&portal2, &ball[i], 400, 500, 20); 
									Mix_PlayChannel(-1, teleport_sfx, 0);
								}
							}
							if (level > 4)
							{
								if (portal3_collision != 0) 
								{ 
									teleport_Func(&portal3, &ball[i], 100, 450, 20);
									Mix_PlayChannel(-1, teleport_sfx, 0);
								}
								if (portal4_collision != 0) 
								{ 
									teleport_Func(&portal4, &ball[i], 700, 450, 20); 
									Mix_PlayChannel(-1, teleport_sfx, 0);
								}
							}
						}
					}
				}
			}
			
			{} ///dont mess with this

			//brick colors
			{
				for (int i = 0; i < row_bricks; i++)
				{
					switch (brick[i].tier) {
					case 1:
						brick[i].r = 255;
						brick[i].g = 0;
						brick[i].b = 0;
						break;
					case 2:
						brick[i].r = 255;
						brick[i].g = 255;
						brick[i].b = 0;
						break;
					case 3:
						brick[i].r = 0;
						brick[i].g = 255;
						brick[i].b = 0;
						break;
					}

					if (level == 6 || level == 9)
					{
						if (!brick[i].hit)
						{
							brick[i].color_timer--;
							if (brick[i].color_timer == 0)
							{
								brick[i].tier = rand() % 3 + 1;

								///for some reason every brick after the 37th brick doesnt change color which is why this for loop exists
								for (int j = 37; j < row_bricks; j++)
								{
									if (!brick[j].hit) brick[j].tier = rand() % 3 + 1; 
								}

								brick[i].color_timer = 25;
							}
						}
					}
				}
			}

			unsigned int current_time = SDL_GetTicks();
			//animate portals
			{
				animate_Func(&portal1, current_time);
				animate_Func(&portal2, current_time);
				animate_Func(&portal3, current_time);
				animate_Func(&portal4, current_time);
			}
		}

		{} 

		//draw
		{
			if (level == 0)
			{
				draw_Text(renderer, font_texture, title_text, 50, 75, 10);
				draw_Text(renderer, font_texture, start_text, 25, 315, 275);
				draw_Text(renderer, font_texture, quit_text, 25, 315, 500);
				//start game
				if (state[SDL_SCANCODE_S])
				{
					level = 1;
					lvl1_timer = 1000;
					init_Bricks(brick, row_bricks, level);
					for (int i = 0; i < max_balls; i++) ball[i].destroyed = false;
				}
			}
			else if (level != 0 && level < 10)
			{
				//players
				p1.draw_Player(renderer, 255, 255, 255);
				p2.draw_Player(renderer, 100, 100, 100);

				if (!is_there_2p) draw_Slow_Guage(renderer, 650, 50, slow_guage, 10);

				//objects
				for (int i = 0; i < max_balls; i++)
				{
					if (!ball[i].destroyed) draw_Ball(renderer, &ball[i]);
				}
				for (int i = 0; i < n_bricks; i++)
				{
					if (!brick[i].destroyed) draw_Bricks(renderer, &brick[i]);
				}

				//portals
				{
					if (!is_there_2p)
					{
						if (level > 2)
						{
							//portal1.draw_Portal_Hitbox(renderer);
							portal1.draw_Portal_Img(renderer, portal_texture, portal1.img_source_x, portal1.img_source_y, portal1.img_source_w, portal1.img_source_h);
							//portal2.draw_Portal_Hitbox(renderer);
							portal2.draw_Portal_Img(renderer, portal_texture, portal2.img_source_x, portal2.img_source_y, portal2.img_source_w, portal2.img_source_h);
						}
						if (level > 4)
						{
							portal3.draw_Portal_Img(renderer, portal_texture, portal3.img_source_x, portal3.img_source_y, portal3.img_source_w, portal3.img_source_h);
							portal4.draw_Portal_Img(renderer, portal_texture, portal4.img_source_x, portal4.img_source_y, portal4.img_source_w, portal4.img_source_h);
						}
					}
				}

				//text
				{
					if (!paused) draw_Text(renderer, font_texture, pause_text, 15, 650, 10);
					else if (paused && !lvl_complete)
					{
						draw_Text_Box(renderer, 225, 125, 400, 200);
						draw_Text(renderer, font_texture, paused_menu, 50, 275, 175);
						draw_Text(renderer, font_texture, resume_text, 25, 325, 250);
						draw_Text(renderer, font_texture, back_text, 25, 225, 300);
						if (level != 9) draw_Text(renderer, font_texture, skip_text, 25, 275, 275);
						//skip level
						if (state[SDL_SCANCODE_S] && level != 9)
						{
							level += 1;
							for (int i = 0; i < n_bricks; i++)
							{
								brick[i].destroyed = false;
								uninit_Bricks(&brick[i]);
								//set brick tiers
								switch (level) {
								case 2:
									row_bricks = 25;
									brick[i].tier = 2;
									break;
								case 3:
									row_bricks = 50;
									brick[i].tier = 2;
									lost_ball_limit = 3;
									break;
								case 4:
									row_bricks = 37;
									brick[i].tier = rand() % 3 + 1;
									break;
								case 5:
									row_bricks = 75;
									brick[i].tier = 3;
									lost_ball_limit = 4;
									break;
								case 6:
									row_bricks = 75;
									brick[i].tier = rand() % 3 + 1;
									brick[i].hit = false;
									break;
								case 7:
									row_bricks = 75;
									brick[i].tier = 3;
									lost_ball_limit = 3;
									break;
								case 8:
									row_bricks = 37;
									brick[i].tier = 3;
									break;
								case 9:
									row_bricks = 100;
									brick[i].tier = rand() % 3 + 1;
									brick[i].hit = false;
									lost_ball_limit = 4;
									break;
								}
							}
							init_Bricks(brick, row_bricks, level);
							destroyed_bricks = 0;
							for (int i = 0; i < max_balls; i++) despawn_Ball(&ball[i]);
							active_balls = 0;
							ball_count = 1;
							score = 0;
							multiplier = 1;
							slow_guage = 100.f;
							dir_change = rand() % 4 + 1;
							ball_Direction(&ball[active_balls], dir_change);
							set_Portals(&portal1, &portal2, &portal3, &portal4, is_there_2p, level);
							paused = false;
						}
					}

					//screen text
					{
						draw_Text(renderer, font_texture, level_text, 15, 0, 10);
						draw_Text(renderer, font_texture, multiplier_text, 15, 150, 10);
						draw_Text(renderer, font_texture, score_text, 15, 400, 10);
						draw_Text(renderer, font_texture, nballs_text, 15, 0, 25);
						draw_Text(renderer, font_texture, doubles_text, 15, 600, 25);
						if (destroyed_bricks == row_bricks - 1) draw_Text(renderer, font_texture, timer_text, 15, 150, 25);
						if (!is_there_2p) draw_Text(renderer, font_texture, slow_text, 15, 650, 50);

						//display players' controls text
						if (p1_display_timer > 0) draw_Text(renderer, font_texture, p1.controls, 25, 0, 300);
						if (p2_display_timer > 0 && is_there_2p) draw_Text(renderer, font_texture, p2.controls, 25, 500, 300);

						if (level == 3 || level == 5 || level == 7 || level == 9) draw_Text(renderer, font_texture, lost_balls_text, 15, 0, 40);
					}
				}
			}
				
			if (lvl_failed)
			{
				draw_Text_Box(renderer, 75, 150, 650, 200);
				draw_Text(renderer, font_texture, finish_text, 50, 100, 175);
				draw_Text(renderer, font_texture, retry_text, 25, 300, 250);
				draw_Text(renderer, font_texture, back_text, 25, 225, 275);
				//retry
				if (state[SDL_SCANCODE_R])
				{ 
					for (int i = 0; i < row_bricks; i++)
					{
						brick[i].destroyed = false;

						//reset brick tiers
						switch (level) {
						case 1:
							brick[i].tier = 1;
							lvl1_timer = 1000;
							break;
						case 2:
							brick[i].tier = 2;
							break;
						case 3:
							brick[i].tier = 2;
							lost_ball_limit = 3;
							break;
						case 4:
							brick[i].tier = rand() % 3 + 1;
							break;
						case 5:
							brick[i].tier = 3;
							lost_ball_limit = 4;
							break;
						case 6:
							brick[i].tier = rand() % 3 + 1;
							brick[i].hit = false;
							break;
						case 7:
							brick[i].tier = 3;
							lost_ball_limit = 3;
							break;
						case 8:
							brick[i].tier = 3;
							break;
						case 9:
							brick[i].tier = rand() % 3 + 1;
							brick[i].hit = false;
							lost_ball_limit = 4;
							break;
						}
					}
					init_Bricks(brick, row_bricks, level); 
					destroyed_bricks = 0;
					for (int i = 0; i < max_balls; i++) despawn_Ball(&ball[i]);
					active_balls = 0;
					ball_count = 1;
					score /= 2;
					multiplier = 1;
					bonus_points -= 1000;
					dir_change = rand() % 4 + 1;
					ball_Direction(&ball[active_balls], dir_change);
					slow_guage = 100.f;
					set_Portals(&portal1, &portal2, &portal3, &portal4, is_there_2p, level);
					lvl_failed = false;
				}
			}

			if (lvl_complete)
			{
				if (level < 10)
				{
					draw_Text_Box(renderer, 0, 150, 800, 200);
					draw_Text(renderer, font_texture, finish_text, 50, 25, 175);
					draw_Text(renderer, font_texture, continue_text, 25, 275, 250);
					draw_Text(renderer, font_texture, back_text, 25, 225, 275);
				}
				//continue
				if (state[SDL_SCANCODE_C])
				{
					level += 1;
					for (int i = 0; i < max_balls; i++) despawn_Ball(&ball[i]);
					active_balls = 0;
					ball_count = 1;
					slow_guage = 100.f;
					destroyed_bricks = 0;
					if (level != 1 && level < 10) bonus_points += 5000;
					completed_levels += 1;
					switch (level) {
					case 2:
						row_bricks = 25;
						for (int i = 0; i < row_bricks; i++)
						{
							brick[i].destroyed = false;
							brick[i].tier = 2;
						}
						break;
					case 3:
						row_bricks = 50;
						for (int i = 0; i < row_bricks; i++)
						{
							brick[i].destroyed = false;
							brick[i].tier = 2;
							lost_ball_limit = 3;
						}
						break;
					case 4:
						row_bricks = 37;
						for (int i = 0; i < row_bricks; i++)
						{
							brick[i].destroyed = false;
							brick[i].tier = rand() % 3 + 1;
						}
						break;
					case 5:
						row_bricks = 75;
						for (int i = 0; i < row_bricks; i++)
						{
							brick[i].destroyed = false;
							brick[i].tier = 3;
							lost_ball_limit = 4;
						}
						break;
					case 6:
						row_bricks = 75;
						for (int i = 0; i < row_bricks; i++)
						{
							brick[i].hit = false;
							brick[i].destroyed = false;
							brick[i].tier = rand() % 3 + 1;
						}
						break;
					case 7:
						row_bricks = 75;
						for (int i = 0; i < row_bricks; i++)
						{
							brick[i].destroyed = false;
							brick[i].tier = 3;
							lost_ball_limit = 3;
						}
						break;
					case 8:
						row_bricks = 37;
						for (int i = 0; i < row_bricks; i++)
						{
							brick[i].destroyed = false;
							brick[i].tier = 3;
						}
						break;
					case 9:
						row_bricks = 100;
						for (int i = 0; i < row_bricks; i++)
						{
							brick[i].hit = false;
							brick[i].destroyed = false;
							brick[i].tier = rand() % 3 + 1;
							lost_ball_limit = 4;
						}
						break;
					case 10:
						if (completed_levels < 9) bonus_points = 0;
						else if (completed_levels == 9)
						{
							if (!is_there_2p)
							{
								if (lost_balls < 30) bonus_points *= 3;
								else if (lost_balls > 30 && lost_balls < 50) bonus_points *= 2;
								else if (lost_balls > 50) bonus_points *= 1;
							}
							else if (is_there_2p)
							{
								if (lost_balls < 10) bonus_points *= 3;
								else if (lost_balls > 10 && lost_balls < 20) bonus_points *= 2;
								else if (lost_balls > 20) bonus_points *= 1;
							}
						}

						final_score = score + bonus_points;
						break;
					}

					if (level < 10)
					{
						dir_change = rand() % 4 + 1;
						ball_Direction(&ball[active_balls], dir_change);
						init_Bricks(brick, row_bricks, level);
						set_Portals(&portal1, &portal2, &portal3, &portal4, is_there_2p, level);
					}
					
					lvl_complete = false;
				}
			}

			//end game stats
			if (level >= 10)
			{
				draw_Text(renderer, font_texture, end_game_text, 50, 50, 50);
				draw_Text(renderer, font_texture, score_text, 25, 225, 200);
				draw_Text(renderer, font_texture, lost_balls_num, 25, 250, 250);
				draw_Text(renderer, font_texture, bonus_text, 25, 250, 300);
				draw_Text(renderer, font_texture, final_score_text, 25, 250, 350);
				draw_Text(renderer, font_texture, back_text, 25, 225, 500);
			}
		}
		
		//updates screen
		SDL_RenderPresent(renderer);

		//caps frame rate
		frame_time = SDL_GetTicks() - frame_start;
		if (frame_delay > frame_time) SDL_Delay(frame_delay - frame_time);
	}

	return 0;
}