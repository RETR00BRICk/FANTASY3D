#pragma once
#include <SDL2/SDL.h>
#include <cmath>
#include "mathematics.h"
#include "wall.h"
#include "map.h"
#include "entity_type.h"

class Camera{
public:
	Camera(int out_w, int out_h, Mathematics::Vector3 pos = {0.0f, 0.0f, 0.0f}, float ang = 0.0f);
	//SETTERS
	void SetOutputImageSize(int w, int h);
	void SetQuality(int quality);
	void SetRenderDistance(int distance);
	//TRANSFORM
	void MoveTo(const Mathematics::Vector3& vector);
	void TurnToAngle(float angle);
	//RENDERING AND CALCULATING
	void Render(SDL_Renderer* renderer, const Map& map);
private:
	struct HitInfo{	//INFORMATION OF ENTITIES WHICH IS NEEDED TO DRAW THEM
		EntityType type = EntityType::WALL;
		float distance = 10.0f;
		int line_index = 0;
		float height = 0.0f;
		float pos_z = 0.0f;
		SDL_Color color = {255, 0, 255, 255};
	};
	//SCREEN
	int out_w_;
	int out_h_;
	float aspect_ratio_;
	//RENDERING
	float fov_ = Mathematics::pi/3.0f;
	float render_plane_height_ = 1.0f;
	float actual_render_plane_height_ = 1.0f;;
	int lines_count_ = 100.0f;
	int render_distance_ = 25.0f;
	//TRANSFORM
	Mathematics::Vector3 pos_;
	float angle_;
	//FUNCTIONS
	SDL_Color GetFoggedColorFromDistance(const SDL_Color& input_color, const SDL_Color& fog_color, float distance);
	void DrawHitEntities(SDL_Renderer* renderer, const HitInfo* array, int array_size, const SDL_Color& fog_color, float floor_level);
	bool TryGetDistanceOfRayIntersectionWithWall_CH(const Wall& wall, const Mathematics::Vector2& ray_dir, float& dist);
	void SetNearWallIndexes_CH(const Wall* walls, int walls_count, bool* out_indexes_array);
	void LoadSettings();
	void DrawBg(SDL_Renderer* renderer, const SDL_Color& color);
	void DrawFloor(SDL_Renderer* renderer, const SDL_Color& color, const SDL_Color& fog_color, float floor_level);
	void DrawGrid(SDL_Renderer* renderer, const Mathematics::Vector2* ray_directions, float floor_level);
	void SortHitArray_CH(HitInfo* array, int array_size);
};
