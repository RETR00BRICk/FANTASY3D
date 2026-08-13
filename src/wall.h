#pragma once
#include "mathematics.h"
#include <SDL2/SDL.h>

struct Wall{
	//SETS WHEN LOADING FROMF FILE. DO NOT CHANGE
	bool is_vertical_ = false;
	Mathematics::Vector2 pos0 = {0.0f, 0.0f};
	Mathematics::Vector2 pos1 = {0.0f, 0.0f};
	float height = 0.0f;
	float pos_z = 0.0f;
	SDL_Color color = {0, 0, 0, 0};
	//OTHER VARIABLES. CAN BE CHANGED
	Mathematics::Vector2 pos_m = {0.0f, 0.0f};
	//FUNCTIONS
	void CalculateMiddlePoint();
};
