
#include <SDL2/SDL.h>
#include "map.h"
#include "wall.h"
#include "mathematics.h"
#include "string_functions.h"
#include <cmath>

Map::Map(const char* file_name){
	Load(file_name);
}

void Map::Load(const char* file_name){
	//MAKING FILE PATH
	char file_path[32] = "maps/";
	StringFunctions::AddTextToString_CH(file_path, file_name, 32);
	//TRYING TO READ THE FILE
	SDL_RWops* file = SDL_RWFromFile(file_path, "rb");
	if (!file) return;
	int file_size = SDL_RWsize(file);	
	if (file_size <= 0) return;
	char char_array[file_size]; //If it doesn't fit on the stack, it is the user fault
	int cursor = 0;
	SDL_RWread(file, char_array, 1, file_size);
	SDL_RWclose(file);
	//READING PARAMETERS (LIGHT, FOG, ETC.)
	int map_settings[7];
	if(!StringFunctions::TryLoadIntArrayFromString_CH(map_settings, 7, cursor, char_array, file_size)) return;
	fog_color.r = (Uint8)map_settings[0];
	fog_color.g = (Uint8)map_settings[1];
	fog_color.b = (Uint8)map_settings[2];
	floor_color.r = (Uint8)map_settings[3];
	floor_color.g = (Uint8)map_settings[4];
	floor_color.b = (Uint8)map_settings[5];
	floor_level = (float)map_settings[6];
	//READING WALLS
	walls_count = 0;
	while(walls_count < kMaxWallCount){
		float x0, y0, x1, y1, h, z;
		bool is_vertical;
		Uint8 r, g, b, a;
		int wall_settings[11];
		if(!StringFunctions::TryLoadIntArrayFromString_CH(wall_settings, 11, cursor, char_array, file_size)) return;
		is_vertical = (bool)wall_settings[0];
		x0 = (float)wall_settings[1];
		x1 = (float)wall_settings[2];
		y0 = (float)wall_settings[3];
		y1 = (float)wall_settings[4];
		h = (float)wall_settings[5];
		z = (float)wall_settings[6];
		r = (Uint8)wall_settings[7];
		g = (Uint8)wall_settings[8];
		b = (Uint8)wall_settings[9];
		a = (Uint8)wall_settings[10];
		walls[walls_count] = {
			is_vertical, 
			{x0,y0},{x1,y1},
			h,z,
			{r,g,b,a}
		};
		walls[walls_count].CalculateMiddlePoint();
		walls_count ++;
	}
}
bool Map::CheckCollision(const Mathematics::Vector3& coordinate, float side_offset, float height) const{
	//ITERATING ALL WALLS IN THE ARRAY
	for(int i = 0; i < walls_count; i ++){
		const Wall& current_wall = walls[i]; 
		//UNIVERSAL COORDINATES FOR BOTH X AND Y
		float wall_coordinate0, wall_coordinate1, wall_perpendicular_coordinate, checking_parallel_coordinate, checking_perpendicular_coordinate;
		if(current_wall.is_vertical_){	
			//ALONGSIDE THE WALL DIRECTION
			wall_coordinate0 = current_wall.pos0.y;
			wall_coordinate1 = current_wall.pos1.y;
			checking_parallel_coordinate = coordinate.y;
			//SIDEWATS
			wall_perpendicular_coordinate = current_wall.pos0.x;
			checking_perpendicular_coordinate = coordinate.x;
		}else{
			//ALONGISDE THE WALL DIRECTION
			wall_coordinate0 = current_wall.pos0.x;
			wall_coordinate1 = current_wall.pos1.x;
			checking_parallel_coordinate = coordinate.x;
			//SIDEWAYS
			wall_perpendicular_coordinate = current_wall.pos0.y;		
			checking_perpendicular_coordinate = coordinate.y;
		}
		if(!Mathematics::CheckPointOnLine(wall_coordinate0,wall_coordinate1,checking_parallel_coordinate)) continue; //checking along wall direction
		float wall_z_end = current_wall.pos_z + current_wall.height;
		float body_z_end = coordinate.z + height;
		if(!Mathematics::CheckLineOnLine(current_wall.pos_z, wall_z_end, coordinate.z, body_z_end)) continue; //Checking height
		if(std::abs(wall_perpendicular_coordinate - checking_perpendicular_coordinate) < side_offset) { //Checking sideways to the wall direction
			return true;
		}else continue;
	}
	return false; //If didn't find any intersection
}
