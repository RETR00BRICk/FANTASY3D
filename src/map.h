#pragma once
#include <SDL2/SDL.h>
#include "wall.h"

namespace Mathematics{ struct Vector3; } 

struct Map{
	//WALLS
	static constexpr int kMaxWallCount = 500;
	Wall walls[kMaxWallCount];
	int walls_count = 0;
	//ENVIROMENT
	SDL_Color fog_color = {200, 170, 200, 255};
	SDL_Color floor_color = {50, 100, 45, 255};
	float floor_level = 0.0f;
	//FUNCTIONS
	Map(const char* file_name);
	void Load(const char* file_name);
	bool CheckCollision(const Mathematics::Vector3& coordinate, float side_offset = 0.2f, float height = 0.0f) const;
};
