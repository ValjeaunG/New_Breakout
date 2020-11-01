#pragma once

#include "SDL2/include/SDL.h"

struct Portal
{
	float x, y, hitbox_x, hitbox_y;
	int hitbox_w, hitbox_h, img_source_x, img_source_y, img_source_w, img_source_h, sprite_sheet; 
	unsigned int frame_update;

	void init_Portal(float xpos, float ypos, int size)
	{
		x = xpos;
		y = ypos;
		hitbox_x = x + 5.f;
		hitbox_y = y + 5.f;
		hitbox_w = size;
		hitbox_h = size;
	}

	void init_Source_Img(int x_src, int y_src, int w_src, int h_src, int n_frames, int spd, unsigned int ticks)
	{
		img_source_x = x_src;
		img_source_y = y_src;
		img_source_w = w_src;
		img_source_h = h_src;
		sprite_sheet = img_source_h * n_frames;
		spd = 100;
		frame_update = ticks;
	}

	void draw_Portal_Img(SDL_Renderer *renderer, SDL_Texture *t, int x_src, int y_src, int w_src, int h_src)
	{
		//source
		SDL_Rect src;
		src.x = x_src;
		src.y = y_src;
		src.w = w_src;
		src.h = h_src;
		//destination
		SDL_Rect dest;
		dest.x = x;
		dest.y = y;
		dest.w = src.w;
		dest.h = src.h;
		//draw image
		//copy from source texture to destination screen.
		SDL_RenderCopyEx(renderer, t, &src, &dest, 0, NULL, SDL_FLIP_NONE);
	}

	void draw_Portal_Hitbox(SDL_Renderer *renderer)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_Rect rect;
		rect.w = hitbox_w;
		rect.h = hitbox_h;
		rect.x = hitbox_x;
		rect.y = hitbox_y;
		SDL_RenderFillRect(renderer, &rect);
	}
};