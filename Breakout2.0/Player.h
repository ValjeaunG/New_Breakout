#pragma once

#include "SDL2/include/SDL.h"

struct Player
{
	int w, h;
	float x, y, xvel;
	char controls[17];

	void init_Player(int width, int height, float xpos, float ypos, float vel)
	{
		w = width;
		h = height;
		x = xpos;
		y = ypos;
		xvel = vel;
	}

	void halve_Width(int width)
	{
		w = width / 2;
	}

	void double_Width(int width)
	{
		w = width * 2;
	}

	void move_Controls(const Uint8 input1, const Uint8 input2, int bound1, int bound2)
	{
		//if not at edges of screen
		if (input1) xvel = -9.f;
		else if (input2) xvel = 9.f;

		if (input1 || input2) x += xvel;

		//if at edges of screen
		if (x <= bound1) x = 0;
		else if (x >= bound2 - w) x = bound2 - w;
	}

	void draw_Player(SDL_Renderer* renderer, int r, int g, int b)
	{
		SDL_SetRenderDrawColor(renderer, r, g, b, 255);

		SDL_Rect rect;
		rect.w = w;
		rect.h = h;
		rect.x = x;
		rect.y = y;

		SDL_RenderFillRect(renderer, &rect);
	}
};
