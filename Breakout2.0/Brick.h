#pragma once

struct Brick
{
	float x, y;
	int w, h, r, g, b, tier, color_timer;
	bool destroyed, hit;
};
