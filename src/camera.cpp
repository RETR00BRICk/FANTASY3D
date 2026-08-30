
#include <SDL2/SDL.h>
#include <cmath>
#include "mathematics.h"
#include "wall.h"
#include "map.h"
#include "entity_type.h"
#include "string_functions.h"
#include "camera.h"

Camera::Camera(int out_w, int out_h, Mathematics::Vector3 pos, float ang):
out_w_(out_w),out_h_(out_h),pos_(pos),angle_(ang){
	aspect_ratio_ = (float)out_w/(float)out_h;
	actual_render_plane_height_ = render_plane_height_/aspect_ratio_;
	LoadSettings();
}

void Camera::SetOutputImageSize(int w, int h){
	out_w_ = w; out_h_ = h;
	aspect_ratio_ = (float)w/(float)h;
	actual_render_plane_height_ = render_plane_height_/aspect_ratio_;
}

void Camera::SetQuality(int quality){
	if(quality < 1) return;
	lines_count_ = quality;
}

void Camera::SetRenderDistance(int distance){
	if(distance < 0) return;
	render_distance_ = distance;
}

void Camera::MoveTo(const Mathematics::Vector3& vector){
	pos_ = vector;
}

void Camera::TurnToAngle(float angle){
	angle_ = Mathematics::GetClampedAngle(angle);
}
	
void Camera::Render(SDL_Renderer* renderer, const Map& map){
	DrawBg(renderer, map.fog_color);
	DrawFloor(renderer, map.floor_color, map.fog_color, map.floor_level);
	//RAYS DIRECTIONS
	float angle_betw_rays = fov_/lines_count_;	//angle between 2 closest rays. Identical for every ray
	int max_hit_per_ray = 10;
	Mathematics::Vector2 ray_directions[lines_count_];
	for(int line_num = 0; line_num < lines_count_; line_num ++){
		float current_ray_angle = Mathematics::GetClampedAngle(angle_ + fov_/2.0f - angle_betw_rays * line_num);
		ray_directions[line_num] = Mathematics::GetVectorByAngle2(current_ray_angle);
	}
	//GRID
	DrawGrid(renderer, ray_directions, map.floor_level);
	//WALLS
	bool walls_to_render_flags[map.walls_count];
	SetNearWallIndexes_CH(map.walls, map.walls_count, walls_to_render_flags);
	for(int line_num = 0; line_num < lines_count_; line_num ++){
		//THIS RAY
		int hits_count_per_ray = 0;
		HitInfo hit_info_array[max_hit_per_ray];
		//ITERATING ALL WALLS FOR THIS RAY
		for(int wall_index = 0; wall_index < map.walls_count; wall_index++){
			if(hits_count_per_ray >= max_hit_per_ray) break;
			if(!walls_to_render_flags[wall_index]) continue;
			const Wall& current_wall = map.walls[wall_index];
			//GETTING DISTANCE	
			float distance;
			if(!TryGetDistanceOfRayIntersectionWithWall_CH(current_wall, ray_directions[line_num], distance)) continue;
			hit_info_array[hits_count_per_ray] = {EntityType::WALL, distance, line_num, current_wall.height, current_wall.pos_z, current_wall.color};
			hits_count_per_ray++;
		}
		SortHitArray_CH(hit_info_array, hits_count_per_ray);
		DrawHitEntities(renderer, hit_info_array, hits_count_per_ray, map.fog_color, map.floor_level);	
	}
}

void Camera::LoadSettings(){
	SDL_RWops* file = SDL_RWFromFile("settings/camera.txt", "rb");
	if (!file) return;
	int file_size = SDL_RWsize(file);
	if(file_size <= 0) return;
	char char_array[file_size]; //If it doesn't fit on the stack, it is the user fault, not mine!
	SDL_RWread(file, char_array, 1, file_size);
	SDL_RWclose(file);
	int settings[4];
	int cursor = 0;
	if(StringFunctions::TryLoadIntArrayFromString_CH(settings, 4, cursor, char_array, file_size)){
		fov_ = (float)settings[0]*Mathematics::pi/180.f;
		render_plane_height_ = (float)settings[1]/100.0f;
		lines_count_ = settings[2];
		render_distance_ = settings[3];
		actual_render_plane_height_ = render_plane_height_/aspect_ratio_;
	}
}

void Camera::SetNearWallIndexes_CH(const Wall* walls, int walls_count, bool* out_indexes_array){
	for(int i = 0; i < walls_count; i++){
		float distance = Mathematics::GetFastDistance2D({pos_.x,pos_.y}, walls[i].pos_m);
		out_indexes_array[i] = distance <= render_distance_*1.5f;
	}
}

void Camera::DrawBg(SDL_Renderer* renderer, const SDL_Color& color){
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(renderer);
}

void Camera::DrawFloor(SDL_Renderer* renderer, const SDL_Color& color, const SDL_Color& fog_color, float floor_level){
	if(pos_.z <= floor_level) return;
	int start_line_num = lines_count_ - lines_count_/2;
	for(int line_num = start_line_num; line_num < lines_count_; line_num ++){
		//SCREEN POSITION
		int line_screen_top = out_h_ * line_num/lines_count_;
		int line_screen_height =  out_h_ * (line_num+1)/lines_count_ - line_screen_top;
		//WORLD DISTANCE
		float line_relative_pos_from_center = (float)line_num/(float)lines_count_ - 0.5f;
		float coordinate_on_render_plane = actual_render_plane_height_ * line_relative_pos_from_center;
		SDL_Color line_color;
		if(abs(coordinate_on_render_plane) > 0.001f){
			float distance = std::abs((pos_.z - floor_level)/coordinate_on_render_plane); //Just simple triangles and proportions
			line_color = GetFoggedColorFromDistance(color, fog_color, distance);
		}else{
			line_color = fog_color;
		}	
		//DRAWING	
		SDL_SetRenderDrawColor(renderer, line_color.r, line_color.g, line_color.b, 255);
		SDL_Rect rect = {0, line_screen_top, out_w_, line_screen_height};
		SDL_RenderFillRect(renderer, &rect);
	}
}

void Camera::DrawGrid(SDL_Renderer* renderer, const Mathematics::Vector2* ray_directions, float floor_level){
	SDL_Rect all_dots[10*lines_count_]; //it won't overflow (im too lazy to explain why, go think yourself)
	int all_dots_count = 0;
	float delta_z = floor_level - pos_.z;
	for(int line_num = 0; line_num < lines_count_; line_num += 2){
		const Mathematics::Vector2& ray_dir = ray_directions[line_num]; //ray
		float next_line_gridX;		//grid X represents vertial lines (only X coordinate changes)
		float next_line_gridY;		//grid Y represents horisontal lines (only Y coordinate changes)
		//first grid line x
		if(ray_dir.x > 0) next_line_gridX = std::ceil(pos_.x);
		else next_line_gridX = std::floor(pos_.x);
		//first grid line y
		if(ray_dir.y > 0) next_line_gridY = std::ceil(pos_.y);
		else next_line_gridY = std::floor(pos_.y);
		float dist_gridX = (next_line_gridX - pos_.x)/ray_dir.x;
		float dist_gridY = (next_line_gridY - pos_.y)/ray_dir.y;
		float delta_dist_gridX = std::abs(1.0f/ray_dir.x);
		float delta_dist_gridY = std::abs(1.0f/ray_dir.y);
		//FINDING DISTANCES TO OTHER GRID LINES
		int dot_screen_x = line_num*out_w_/lines_count_; //dot is a point rendered on a screen
		//GRID X
		while(dist_gridX <= 10.0f){
			float inv_distance_x_plane_height = 1.0f / (dist_gridX * actual_render_plane_height_); //x means muiltiply
			float dot_relative_position = 0.5f - delta_z * inv_distance_x_plane_height;
			int dot_screen_y = dot_relative_position * out_h_;
			all_dots[all_dots_count] = {dot_screen_x, dot_screen_y, 4, 4}; //adding dot
			//moving to next grid line
			dist_gridX += delta_dist_gridX;
			all_dots_count ++;
		}
		//GRID Y
		while(dist_gridY <= 10.0f){
			float inv_distance_x_plane_height = 1.0f / (dist_gridY * actual_render_plane_height_); //x means muiltiply
			float dot_relative_position = 0.5f - delta_z * inv_distance_x_plane_height;
			int dot_screen_y = dot_relative_position * out_h_;
			all_dots[all_dots_count] = {dot_screen_x, dot_screen_y, 4, 4}; //adding dot
			//moving to next grid line
			dist_gridY += delta_dist_gridY; 
			all_dots_count ++;
		}		
	}
	//DRAWING
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRects(renderer, all_dots, all_dots_count);
}

bool Camera::TryGetDistanceOfRayIntersectionWithWall_CH(const Wall& wall, const Mathematics::Vector2& ray_dir, float& dist){ 
	if(wall.is_vertical_){
		dist = (wall.pos0.x - pos_.x)/ray_dir.x;
		if(dist < 0.0f) return false;
		//VALIDATION
		float point_y = dist*ray_dir.y + pos_.y;
		if(!Mathematics::CheckPointOnLine(wall.pos0.y, wall.pos1.y, point_y)) return false;		
	}else{
		dist = (wall.pos0.y - pos_.y)/ray_dir.y;
		if(dist < 0.0f) return false;
		//VALIDATION
		float point_x = dist*ray_dir.x + pos_.x;
		if(!Mathematics::CheckPointOnLine(wall.pos0.x, wall.pos1.x, point_x)) return false;	
	}
	return true;
}

void Camera::SortHitArray_CH(HitInfo* array, int array_size){
	int sorted_count = 1; //FIRST ELEMENT OF A SORTED PART IN ARRAY AS ALREADY SORTED
	while(sorted_count < array_size){
		//NEXT DIRTY ELEMENT
		HitInfo element = array[sorted_count];
		//IF DIRTY ELEMENT IS BIGGER THAN LAST SORTED ONE, STARTING TO MOVE IT INTO SORTED PART
		if(element.distance > array[sorted_count - 1].distance){
			int sorted_index = sorted_count - 1;
			while(sorted_index >= 0){
				if(element.distance > array[sorted_index].distance){ 
					array[sorted_index+1] = array[sorted_index]; //MOVING SORTED ELEMENTS TO THE RIGHT ONE BY ONE
				}else{
					break; //FOUND CORRECT PLACE
				}
				sorted_index--;
			}
			array[sorted_index + 1] = element; //INSERTING DIRTY ELEMENT INTO CORRECT PLACE
		}
		sorted_count ++; //EXPANDING SORTED PART OF THE ARRAY
	}
}

void Camera::DrawHitEntities(SDL_Renderer* renderer, const HitInfo* array, int array_size, const SDL_Color& fog_color, float floor_level){
	for(int index = 0; index < array_size; index++){
		const HitInfo& entity = array[index];
		if(entity.type == EntityType::WALL){
			//SCREEN POSITION X & WIDTH
			int entity_screen_x = entity.line_index*out_w_/lines_count_;
			int entity_screen_width = (entity.line_index+1)*out_w_/lines_count_ - entity_screen_x;
			//ENTITY POSITION CLAMPING
			float entity_pos = std::max(floor_level, entity.pos_z);	//So the part of the wall won't be rendered if under the floor
			float entity_top = std::max(floor_level, entity.pos_z + entity.height);
			//SCREEN POSITION Y
			float inv_distance_x_plane_height = 1.0f / (entity.distance * actual_render_plane_height_); //x means muiltiply
			float entity_relative_top_position = 0.5f - (entity_top - pos_.z) * inv_distance_x_plane_height;
			float entity_relative_bottom_position = 0.5f - (entity_pos - pos_.z) * inv_distance_x_plane_height;
			int entity_screen_top = entity_relative_top_position * out_h_;
			int entity_screen_bottom = entity_relative_bottom_position * out_h_;
			//HEIGHT
			int entity_screen_height = entity_screen_bottom - entity_screen_top;
			//DRAWING
			SDL_Color color = GetFoggedColorFromDistance(entity.color, fog_color, entity.distance);
			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
			SDL_Rect rect = {entity_screen_x, entity_screen_top, entity_screen_width, entity_screen_height};
			SDL_RenderFillRect(renderer, &rect);
		}
	}
}

SDL_Color Camera::GetFoggedColorFromDistance(const SDL_Color& input_color, const SDL_Color& fog_color, float distance){
	float distance_clamped = std::min((int)distance, render_distance_);
	float t = distance_clamped*100/render_distance_;
	Uint8 r = (Uint8)Mathematics::LinearInterpolationInt(input_color.r, fog_color.r, t);
	Uint8 g = (Uint8)Mathematics::LinearInterpolationInt(input_color.g, fog_color.g, t);
	Uint8 b = (Uint8)Mathematics::LinearInterpolationInt(input_color.b, fog_color.b, t);
	return {r,g,b,input_color.a};
}
