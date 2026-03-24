#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include <fstream>
//This code is garbage
//_CH in the function's name means it changes the values, given to the function in the ( )
//Get in the function's name means it returns the value. But some functions can return data even if "Get" is absent
//FISH EYE IS NOT A BUG ITS A FEATURE
//Made by RETR00BRICk
enum class EntityType{
	WALL,
	SPRITE,
	WEIRD_WALL
};
enum class PlayerMode{
	DEFAULT,
	CHEATER,
	MAX
};
enum class GameMode{
	MENU,
	PLAYING,
	PAUSED
};

bool CompareStrings(const char* string0, const char* string1, int string_max_size){
	int i = 0;
	while(string0[i] != '\0' && string1[i] != '\0' && i < string_max_size){
		if(string0[i] != string1[i]) return false;
		i ++;
	}
	return string0[i] == string1[i];
}

void WriteTextToString_CH(char* string, const char* text, int string_max_size){
	int i = 0;
	while (text[i] != '\0' && i < string_max_size - 1) {
		string[i] = text[i];
		i++;
	}
	string[i] = '\0';
}

void AddTextToString_CH(char* string, const char* text, int string_max_size){
	int string_cursor = 0;
	while(string[string_cursor] != '\0' &&  string_cursor < string_max_size - 1){
		string_cursor++;
	}
	int text_cursor = 0;
	while(text[text_cursor] != '\0' && string_cursor < string_max_size - 1){
		string[string_cursor] = text[text_cursor]; 
		text_cursor ++;
		string_cursor ++;
	}
	string[string_cursor] = '\0';
}

void WriteIntToString_CH(char* string, int integer, int string_max_size){
	bool negative = integer < 0;
	int cursor = 0;
	if(cursor < string_max_size - 1){
		if(negative){
			string[cursor] = '-';
			integer = -integer;
			cursor ++;
		}
	}else return; 
	
	if(integer == 0){
		if(cursor < string_max_size - 1){ 
			string[cursor] = '0';
			string[cursor+1] = '\0';
			return;
		}
	}else{
		int remaining_number = integer;
		char reversed_numbers[16];
		int counter = 0;
		while(remaining_number > 0){
			char digit = '0' + remaining_number%10;
			reversed_numbers[counter] = digit;
			remaining_number /= 10;	
			counter ++;
		}
		int reversed_nums_cursor = 0;
		while(reversed_nums_cursor < counter && cursor < string_max_size - 1){
			string[cursor] = reversed_numbers[counter - reversed_nums_cursor - 1];
			cursor ++;
			reversed_nums_cursor ++;
		}
		string[cursor] = '\0';
	}
}

struct Vector2{
	float x = 0.0f, y = 0.0f;
	//FUNCTIONS THAT CHANGE THE VALUE
	void operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
    }
    void operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
    }
    void operator*=(float multiplier){
        x *= multiplier;
        y *= multiplier;
    }
    //FUNCTIONS THAT RETURN VALUE
    Vector2 operator+(const Vector2& other) const {
        return { x + other.x, y + other.y };
    }
    Vector2 operator-(const Vector2& other) const {
        return { x - other.x, y - other.y };
    }
    Vector2 operator*(float multiplier) const{
        return { x * multiplier, y * multiplier };
    }
};

struct Vector3{
	float x = 0.0f, y = 0.0f, z = 0.0f;
	//FUNCTIONS THAT ARE CHANGING THE VALUE
	void operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
    }
    void operator-=(const Vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
    }
    void operator*=(float multiplier){
        x *= multiplier;
        y *= multiplier;
        z *= multiplier;
    }
    //FUNCTIONS THAT RETURN VALUE
    Vector3 operator+(const Vector3& other) const {
        return { x + other.x, y + other.y, z + other.z};
    }
    Vector3 operator-(const Vector3& other) const {
        return { x - other.x, y - other.y, z - other.z};
    }
    Vector3 operator*(float multiplier) const {
        return { x * multiplier, y * multiplier, z * multiplier};
    }
};

namespace Mathematics{
	constexpr int sincos_array_size = 6284;
	float fast_sin_array[sincos_array_size] = {};
	float fast_cos_array[sincos_array_size] = {};
	constexpr float pi = 3.14159265358979f;
	constexpr float tau = 2.0f*pi;
	//ANGLES
	float GetClampedAngle(float angle){
		while(angle >= tau){ angle -= tau; }
		while(angle < 0){ angle += tau; }
		return angle;
	}
	
	void FillFastSinusArray(){
		for(int i = 0; i < sincos_array_size; i ++){
			fast_sin_array[i] = std::sin(i/1000.0f);
		}
	}
	void FillFastCosinusArray(){
		for(int i = 0; i < sincos_array_size; i ++){
			fast_cos_array[i] = std::cos(i/1000.0f);
		}
	}
	
	float GetFastSinus(float angle){
		angle = GetClampedAngle(angle);
		return fast_sin_array[(int)(angle*1000.0f)];
	}
	float GetFastCosinus(float angle){
		angle = GetClampedAngle(angle);
		return fast_cos_array[(int)(angle*1000.0f)];
	}
	//DISTANCES
	float GetHypot2D(const Vector2& v){
		return std::sqrt(v.x*v.x + v.y*v.y);
	}
	float GetHypot3D(const Vector3& v){
		return std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	}
	
	float GetDistance2D(const Vector2& v0, const Vector2& v1){
		return GetHypot2D(v1-v0);
	}
	float GetDistance3D(const Vector3& v0, const Vector3& v1){
		return GetHypot3D(v1-v0);
	}
	
	float GetFastDistance2D(const Vector2& v0, const Vector2& v1){
		return std::abs(v1.x - v0.x) + std::abs(v1.y - v0.y);
	}
	//VECTORS
	Vector2 GetVectorByAngle2(float angle){
		return {GetFastCosinus(angle), GetFastSinus(angle)};
	}
	Vector3 GetVectorByAngle3(float angle){
		return {GetFastCosinus(angle), GetFastSinus(angle), 0.0f};
	}
	
	Vector2 GetNormalizedVec2(const Vector2& vector){
		float length = GetHypot2D(vector);
		if(std::abs(length) <= 0.001f) return {0.0f,0.0f};
		float inv_length = 1.0f / length;
		return vector*inv_length;		
	}
	Vector3 GetNormalizedVec3(const Vector3& vector){
		float length = GetHypot3D(vector);
		if(std::abs(length) <= 0.001f) return {0.0f,0.0f,0.0f};
		float inv_length = 1.0f / length;
		return vector * inv_length;
	}
	float GetDotProduct2(const Vector2& v0, const Vector2& v1){
		return v0.x * v1.x + v0.y * v1.y;
	}
	float GetDotProduct3(const Vector3& v0, const Vector3& v1){
		return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
	}
	//CHECKS
	bool CheckPointOnLine(float line_coordinate0, float line_coordinate1, float checking_coordinate){
		return checking_coordinate >= std::min(line_coordinate0, line_coordinate1) && 
		checking_coordinate <= std::max(line_coordinate0, line_coordinate1);
	}
	bool CheckLineOnLine(float start0, float end0, float start1, float end1){
		float max_start = std::max(start0, start1);
		float min_end = std::min(end0, end1);
		return min_end >= max_start;
	}
	bool CheckDistance2D(const Vector2& v0, const Vector2& v1, float distance){
		float dx = v1.x - v0.x;
		float dy = v1.y - v0.y;
		return (dx*dx + dy*dy) <= distance*distance;
	}
	bool CheckDistance3D(const Vector3& v0, const Vector3& v1, float distance){
		float dx = v1.x - v0.x;
		float dy = v1.y - v0.y;
		float dz = v1.z - v0.z;
		return (dx*dx + dy*dy + dz*dz) <= distance*distance;
	}
	Vector2 CalculateMiddlePoint2D(const Vector2& p0, const Vector2& p1){
		Vector2 summ = p0 + p1;
		return summ * 0.5f;
	}
	// LINEAR INTERPOLATION
	float LinearInterpolationF(float x0, float x1, float target){
		return x0*(1.0f-target) + x1*target;
	}
	int LinearInterpolationInt(int x0, int x1, int target){
		return (x0*(100 - target) + x1*target)/100;
	}
};

struct Wall{
	//SETS WHEN LOADING FROMF FILE. DO NOT CHANGE
	bool is_vertical_ = false;
	Vector2 pos0 = {0.0f, 0.0f};
	Vector2 pos1 = {0.0f, 0.0f};
	float height = 0.0f;
	float pos_z = 0.0f;
	SDL_Color color = {0, 0, 0, 0};
	//OTHER VARIABLES. CAN BE CHANGED
	Vector2 pos_m = {0.0f, 0.0f};
	//FUNCTIONS
	void CalculateMiddlePoint(){
		pos_m = Mathematics::CalculateMiddlePoint2D(pos0, pos1);
	}
};

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
	Map(const char* file_name){
		Load(file_name);
	}
	void Load(const char* file_name){
		//MAKING FILE PATH
		char file_path[32];
		WriteTextToString_CH(file_path, "maps/", 32);
		AddTextToString_CH(file_path, file_name, 32);
		//TRYING TO READ THE FILE
		std::ifstream map_file(file_path);
		if (!map_file.is_open()) return;
		//VARIABLES
		walls_count = 0;
		float x0, y0, x1, y1, h, z;
		bool is_vertical;
		int r, g, b, a;
		map_file.ignore(256, '\n');	//skipping first line here (see the .txt map file)
		//READING FROM FILE
	 	while (map_file >> is_vertical >> x0 >> x1 >> y0 >> y1 >> h >> z >> r >> g >> b >> a){
			if(walls_count < kMaxWallCount){
				walls[walls_count] = {is_vertical, {x0,y0},{x1,y1},h,z,{(Uint8)r,(Uint8)g,(Uint8)b,(Uint8)a}};
				walls[walls_count].CalculateMiddlePoint();
				walls_count++;
			}else{
				break;
			}
		}
		map_file.close();
	}
	bool CheckCollision(const Vector3& coordinate, float side_offset = 0.2f, float height = 0.0f) const{
		//ITERATING ALL WALLS IN THE ARRAY
		for(int i = 0; i < walls_count; i ++){
			const Wall& current_wall = walls[i]; //const Wall& 
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
};

struct PhysicsBody{
	float mass = 1.0f;
	Vector3 pos = {0.0f,0.0f,0.0f};
	float height = 1.0f;
	Vector3 curr_speed = {0.0f,0.0f,0.0f};
	//TELEPORT
	void Teleport3(const Vector3& vector){
		pos = vector;
	}
	void Teleport2(const Vector2& vector){
		pos.x = vector.x;
		pos.y = vector.y;
	}
	//LINEAR MOVING
	void MoveByVector(const Vector3& vector, float multiplier){
		pos += vector * multiplier;
	}
	void TryMoveByVector(const Map& map, const Vector3 vector, float multiplier){
		if(vector.x == 0 and vector.y == 0 and vector.z == 0) return;
		float offset = 0.2f;
		//X
		Vector3 new_pos = pos + Vector3{vector.x, 0.0f, 0.0f} * multiplier;
		if(!map.CheckCollision(new_pos, offset, height)){
			pos = new_pos; //If there were no collision, continue moving by the given vector
		}
		//Y
		new_pos = pos + Vector3{0.0f, vector.y, 0.0f} * multiplier;
		if(!map.CheckCollision(new_pos, offset, height)){
			pos = new_pos; //If there were no collision, continue moving by the given vector
		}
		//Z
		pos = pos + Vector3{0.0f, 0.0f, vector.z} * multiplier; //We don't check collisions when moving up or down... actually I should add that
	}
	
	void MoveByAngle(float angle, float multiplier){
		MoveByVector(Mathematics::GetVectorByAngle3(angle), multiplier);
	}
	
	void ApplySpeed(float multiplier){
		pos += curr_speed * multiplier;
	}
	void TryApplySpeed(float multiplier, float floor_level){
		pos += curr_speed * multiplier;
		//Z Collision (only floor by now)
		if(pos.z < floor_level){
			pos.z = floor_level;
			if(curr_speed.z < 0.0f) curr_speed.z = 0.0f;
		}	
	}
	void Stop(){
		curr_speed = {0.0f, 0.0f, 0.0f};
	}
	//ACCELERATION
	void AccelerateByVector(const Vector3& vector, float multiplier){
		curr_speed += vector * multiplier;
	}
	void AccelerateByAngle(float angle, float multiplier){
		AccelerateByVector(Mathematics::GetVectorByAngle3(angle), multiplier);
	}
	void Gravitate(float multiplier, float g, float floor_level){
		curr_speed.z -= g*multiplier;
	}
	//FORCE
	void AddForceByVector(const Vector3& force_vector, float multiplier){
		float inv_mass = 1.0f/mass;
		AccelerateByVector(force_vector, multiplier * inv_mass);
	}
	void AddForceByAngle(float angle, float multiplier){
		float inv_mass = 1.0f/mass;
		AccelerateByAngle(angle, multiplier * inv_mass);
	}
	//SPEED
	float GetTotalSpeed2(){
		return Mathematics::GetHypot2D({curr_speed.x,curr_speed.y});
	}
	float GetTotalSpeed3(){
		return Mathematics::GetHypot3D(curr_speed);
	}
};

class Player{
public:
    //Transform
	float angle_;
	//Physics
	PhysicsBody body_;
	//FUNCTIONS
	Player(float mxs = 5.0f, float acc = 1.0f, Vector3 pos = {0.0f, 0.0f, 0.0f}, float ang = 0.0f, float height = 1.5f): 
	angle_(ang), max_speed_(mxs), acceleration_(acc){
		body_.pos = pos;
		body_.height = height;
	}
	//MODE
	void NextMode(){
		int next = (int)mode_ + 1;
		if(next == (int)PlayerMode::MAX){
			mode_ = PlayerMode::DEFAULT;
		}else{
			mode_ = (PlayerMode)next;
		}
		body_.Stop();
	}
	//HEALTH
	void Respawn(bool save_inventory = false){
		health_ = 100.0f;
		if(!save_inventory){
			armor_ = 0.0f;
		}
	}
	void RegainHealth(float value){
		if(health_ < 100.0f){
			health_ += value;
			if(health_ > 100.0f) health_ = 100.0f;
		}
	}
	void Die(){
		
	}
	//STAMINA
	void RegainStamina(float value){
		if(health_ > 10.0f && stamina_ < 100.0f){
			stamina_ += value;
			if(stamina_ > 100.0f) stamina_ = 100.0f;
		}
	}
	//DAMAGE
	void Damage(float value){
		float damage = armor_ - value;
		if(damage < 0.0f){
			armor_ = 0.0f;
			health_ += damage;
			if(health_ < 0.0f){
				health_ = 0.0f;
				Die();
			}
		}else{
			armor_ -= value;
		}
	}
	//MOVEMENT AND ROTATION
	float GetSpeedValue(){	
		if(std::abs(health_) <= 0.001f) return 0.0f;
		if(mode_ == PlayerMode::CHEATER) return max_speed_*2.0f;
		float multiplier = 1.0f;
		if(health_ < 10.0f) multiplier *= 0.5f;
		if(stamina_ < 10.0f) multiplier *= 0.5f;
		if(armor_ > 90.0f) multiplier *= 0.9f;
		return max_speed_*multiplier;
	}
	void Go(const Map& map, const Vector3& input, float multiplier){
		Vector3 fwd = Mathematics::GetVectorByAngle3(angle_);
		Vector3 right = {fwd.y, -fwd.x, 0.0f};
		Vector3 up = {0.0f, 0.0f, 1.0f};
		fwd *= input.y;
		right *= input.x;
		up *= input.z;
		Vector3 movement = fwd + right + up;	//1.41X SPEED WHEN MOVING DIAGONALLY IS A FEATURE
		if(mode_ == PlayerMode::CHEATER) body_.MoveByVector(movement, multiplier*GetSpeedValue());
		else body_.TryMoveByVector(map,{movement.x, movement.y, 0.0f}, multiplier*GetSpeedValue());
	}
	void Jump(float speed, float floor_level){
		if(mode_ == PlayerMode::CHEATER) return;
		if(std::abs(body_.pos.z - floor_level) < 0.001f){
			body_.curr_speed.z += speed;
		}
	}
	void Turn(float input, float multiplier){
		angle_ += input*multiplier;
		angle_ = Mathematics::GetClampedAngle(angle_);
	}
	//PHYSICS
	void Gravitate(float multiplier, float g, float floor_level){
		if(mode_ == PlayerMode::CHEATER) return;
		body_.Gravitate(multiplier, g, floor_level);
	}
	void ApplySpeed(float multiplier, float floor_level){
		if(mode_ == PlayerMode::CHEATER) body_.ApplySpeed(multiplier);
		else body_.TryApplySpeed(multiplier, floor_level);
	}
	//GETTERS
	Vector3 GetEyePosition(){
		return body_.pos + Vector3{0.0f, 0.0f, body_.height};
	}
private:
	//Movement
	float max_speed_;
	float acceleration_;
	//Player
	float health_ = 100.0f;
	float stamina_ = 100.0f;
	float armor_ = 0;
	PlayerMode mode_ = PlayerMode::DEFAULT;
};
struct HitInfo{	//INFORMATION OF ENTITIES WHICH IS NEEDED TO DRAW THEM
	EntityType type = EntityType::WALL;
	float distance = 10.0f;
	int line_index = 0;
	float height = 0.0f;
	float pos_z = 0.0f;
	SDL_Color color = {255, 0, 255, 255};
};

class Camera{
public:
	Camera(int out_w, int out_h, float fov, float render_plane_h, int line_count = 100, int render_distance = 100, Vector3 pos = {0.0f, 0.0f, 0.0f}, float ang = 0.0f):
	out_w_(out_w),out_h_(out_h),fov_(fov),render_plane_height_(render_plane_h),lines_count_(line_count),render_distance_(render_distance),pos_(pos),angle_(ang){
		aspect_ratio_ = (float)out_w/(float)out_h;
	}
	//SETTERS
	void SetOutputImageSize(int w, int h){
		out_w_ = w; out_h_ = h;
		aspect_ratio_ = (float)w/(float)h;
	}
	void SetQuality(int quality){
		if(quality < 1) return;
		lines_count_ = quality;
	}
	void SetRenderDistance(int distance){
		if(distance < 0) return;
		render_distance_ = distance;
	}
	//TRANSFORM
	void MoveTo(const Vector3& vector){
		pos_ = vector;
	}
	void TurnToAngle(float angle){
		angle_ = Mathematics::GetClampedAngle(angle);
	}
	//RENDERING AND CALCULATING
	void Render(SDL_Renderer* renderer, const Map& map){
		//BACKGROUND
		DrawBg(renderer, map.fog_color);
		//FLOOR
		DrawFloor(renderer, map.floor_color, map.fog_color);
		//LINES
		float angle_betw_rays = fov_/lines_count_;	//angle between 2 closest rays. Identical for every ray
		int max_hit_per_ray = 10;
		//GETTING RID OF FAR WALLS
		bool walls_to_render_flags[map.walls_count];
		SetNearWallIndexes_CH(map.walls, map.walls_count, walls_to_render_flags);
		//ITERATING ALL LINES
		for(int line_num = 0; line_num < lines_count_; line_num ++){
			//THIS RAY	
			float current_ray_angle = Mathematics::GetClampedAngle(angle_ + fov_/2.0f - angle_betw_rays * line_num);
			Vector2 ray_direction = Mathematics::GetVectorByAngle2(current_ray_angle);
			int hits_count_per_ray = 0;
			HitInfo hit_info_array[max_hit_per_ray];
			//ITERATING ALL WALLS FOR THIS RAY
			for(int wall_index = 0; wall_index < map.walls_count; wall_index++){
				//CHECKS
				if(hits_count_per_ray >= max_hit_per_ray) break;
				if(!walls_to_render_flags[wall_index]) continue;
				//THIS WALL
				const Wall& current_wall = map.walls[wall_index];
				if(current_wall.is_vertical_){
					if(std::abs(ray_direction.x) <= 0.001f) continue;
					//DISTANCE
					float distance = (current_wall.pos0.x - pos_.x)/ray_direction.x;
					if(distance <= 0.001f) continue;
					//VALIDATION
					float point_y = distance*ray_direction.y + pos_.y;
					if(!Mathematics::CheckPointOnLine(current_wall.pos0.y, current_wall.pos1.y, point_y)) continue;
					//WRITING TO ARRAY
					hit_info_array[hits_count_per_ray] = {EntityType::WALL, distance, line_num, current_wall.height, current_wall.pos_z, current_wall.color};	
				}else{
					if(std::abs(ray_direction.y) <= 0.001f) continue;
					//DISTANCE
					float distance = (current_wall.pos0.y - pos_.y)/ray_direction.y;
					if(distance <= 0.001f) continue;
					//VALIDATION
					float point_x = distance*ray_direction.x + pos_.x;
					if(!Mathematics::CheckPointOnLine(current_wall.pos0.x, current_wall.pos1.x, point_x)) continue;
					//WRITING TO ARRAY
					hit_info_array[hits_count_per_ray] = {EntityType::WALL, distance, line_num, current_wall.height, current_wall.pos_z, current_wall.color};
				}
				hits_count_per_ray++;
			}
			//SORTING ALL HIT POINTS
			SortHitArray_CH(hit_info_array, hits_count_per_ray);
			//DISPLAYING ENTITIES
			DrawHitEntities(renderer, hit_info_array, hits_count_per_ray, map.fog_color);	
		}
	}
private:
	//SCREEN
	int out_w_;
	int out_h_;
	float aspect_ratio_;
	//RENDERING
	float fov_;
	float render_plane_height_;
	int lines_count_;
	int render_distance_;
	//TRANSFORM
	Vector3 pos_;
	float angle_;
	//FUNCTIONS
	void SetNearWallIndexes_CH(const Wall* walls, int walls_count, bool* out_indexes_array){
		for(int i = 0; i < walls_count; i++){
			float distance = Mathematics::GetFastDistance2D({pos_.x,pos_.y}, walls[i].pos_m);
			out_indexes_array[i] = distance <= render_distance_*1.5f;
		}
	}
	void SortHitArray_CH(HitInfo* array, int array_size){
		int sorted_count = 1;
		while(sorted_count < array_size){
			//NEXT DIRTY ELEMENT
			HitInfo element = array[sorted_count];
			//IF IT'S VALUE IS BIGGER THAN LAST SORTED ENEMENT'S ONE = SKIPPING THE CHECK
			if(element.distance > array[sorted_count - 1].distance){
				//TRYING TO GET THE NEEDED POSITION
				int sorted_index = sorted_count - 1;
				while(sorted_index >= 0){
					if(element.distance > array[sorted_index].distance){
						array[sorted_index+1] = array[sorted_index];
					}else{
						break;
					}
					sorted_index--;
				}
				array[sorted_index + 1] = element; //ADDING DIRTY ELEMENT INTO SORTED ONES
			}
			sorted_count ++; //EXPANDING SORTED PART OF THE ARRAY
		}
	}
	SDL_Color GetFoggedColorFromDistance(const SDL_Color& input_color, const SDL_Color& fog_color, float distance){
		float distance_clamped = std::min((int)distance, render_distance_);
		float t = distance_clamped*100/render_distance_;
		Uint8 r = (Uint8)Mathematics::LinearInterpolationInt(input_color.r, fog_color.r, t);
		Uint8 g = (Uint8)Mathematics::LinearInterpolationInt(input_color.g, fog_color.g, t);
		Uint8 b = (Uint8)Mathematics::LinearInterpolationInt(input_color.b, fog_color.b, t);
		return {r,g,b,input_color.a};
	}
	void DrawBg(SDL_Renderer* renderer, const SDL_Color& color){
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderClear(renderer);
	}
	void DrawFloor(SDL_Renderer* renderer, const SDL_Color& color, const SDL_Color& fog_color){
		int start_line_num = lines_count_ - lines_count_/2;
		for(int line_num = start_line_num; line_num < lines_count_; line_num ++){
			//SCREEN POSITION
			int line_screen_top = out_h_ * line_num/lines_count_;
			int line_screen_height =  out_h_ * (line_num+1)/lines_count_ - line_screen_top;
			//WORLD DISTANCE
			float actual_render_plane_height = render_plane_height_/aspect_ratio_;
			float line_relative_pos_from_center = (float)line_num/(float)lines_count_ - 0.5f;
			float coordinate_on_render_plane = actual_render_plane_height * line_relative_pos_from_center;
			SDL_Color line_color;
			if(abs(coordinate_on_render_plane) > 0.001f){
				float distance = std::abs(pos_.z/coordinate_on_render_plane); //Just simple triangles and proportions
				line_color = GetFoggedColorFromDistance(color, fog_color, distance);
				//line_color = color;
			}else{
				line_color = fog_color;
			}	
			//DRAWING	
			SDL_SetRenderDrawColor(renderer, line_color.r, line_color.g, line_color.b, 255);
			SDL_Rect rect = {0, line_screen_top, out_w_, line_screen_height};
			SDL_RenderFillRect(renderer, &rect);
		}
	}
	void DrawHitEntities(SDL_Renderer* renderer, const HitInfo* array, int array_size, const SDL_Color& fog_color){
		for(int index = 0; index < array_size; index++){
			HitInfo entity = array[index];
			if(entity.type == EntityType::WALL){
				//SCREEN POSITION X & WIDTH
				int entity_screen_x = entity.line_index*out_w_/lines_count_;
				int entity_screen_width = (entity.line_index+1)*out_w_/lines_count_ - entity_screen_x;
				//ENTITY POSITION CLAMPING
				float entity_pos = std::max(0.0f, entity.pos_z);	//So the part of the wall won't be rendered if under the floor
				float entity_top = std::max(0.0f, entity.pos_z + entity.height);
				//SCREEN POSITION Y
				float inv_distance_x_plane_height = aspect_ratio_ / (entity.distance * render_plane_height_);
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
};

struct TextBox{
	static constexpr int max_text_len = 1024;
	char last_text[max_text_len] = {};
	SDL_Texture* text_texture = nullptr;
	//FUNCTIONS
	TextBox(SDL_Renderer* renderer, const char* text, TTF_Font* font, const SDL_Color& color){
		Update(renderer, text, font, color, true);
	}
	//RERENDER THE TEXTURE (SLOW)
	void Update(SDL_Renderer* renderer, const char* text, TTF_Font* font, const SDL_Color& color, bool f = false){
		if(!f && CompareStrings(text, last_text, max_text_len)) return;
		WriteTextToString_CH(last_text, text, max_text_len);
		SDL_Surface* text_surface = TTF_RenderText_Solid(font, last_text, color);
		if(text_texture != nullptr) SDL_DestroyTexture(text_texture);
		if(text_surface != nullptr) text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
		if(text_surface != nullptr) SDL_FreeSurface(text_surface);
	}
	//DRAW THE STORED TEXTURE (FAST)
	void Draw(SDL_Renderer* renderer, const SDL_Rect& rect){
		if(text_texture != nullptr) SDL_RenderCopy(renderer, text_texture, NULL, &rect);
	}
};

namespace Game{
	constexpr char version[] = "V0.02 FOG UPDATE";
	constexpr char name[] = "FANTASY3D";
	//GAME
	bool game_running = true;
	Player* player = nullptr;
	Camera* camera = nullptr;
	Map* map = nullptr;
	GameMode game_mode = GameMode::MENU;
	//BLINKERS
	float blinker1s_timer = 0.0f;
	bool blinker1s = false;
	//PHYSICS
	float gravity = 9.81f;
	//SETTINGS
	int screen_w = 800;
	int screen_h = 600;
	float fov = Mathematics::pi/3.0f;
	float render_plane_height = 1.0f;
	unsigned int render_quality = 400;
	unsigned int render_distance = 100;
	unsigned int target_fps = 60;
	float sensitivity = 0.001f;
	bool cursor_locked = true;
	//FUNCTIONS	
	void UpdateBlinkers(float dt){
		blinker1s_timer += dt;
		if(blinker1s_timer > 1.0f){
			blinker1s_timer = 0.0f;
			blinker1s = !blinker1s;
		}
	}
	void Crash(const char* message = "UKNOWN ERROR"){
		game_running = false;
		char buffer[512] = "ERROR: ";
		AddTextToString_CH(buffer, message, 512);
		AddTextToString_CH(buffer, "\nLAST STD ERROR LOG: ", 512);
		AddTextToString_CH(buffer, SDL_GetError(), 512);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "CRASH!", buffer, NULL);
		SDL_Quit();
		exit(1);
	}
	void LockUnlockCursor(){
		cursor_locked = !cursor_locked;
		SDL_SetRelativeMouseMode((SDL_bool)cursor_locked);
	}
	void EventHandler(SDL_Event* event){
		while (SDL_PollEvent(event)) {
			if (event->type == SDL_QUIT) {
				game_running = false;
			}
			if (event->type == SDL_WINDOWEVENT){
				if (event->window.event == SDL_WINDOWEVENT_RESIZED){
					int w = event->window.data1;
					int h = event->window.data2;
					screen_h = h;
					screen_w = w;
					camera->SetOutputImageSize(w,h);
				}
			}
			if (event->type == SDL_KEYDOWN){
				if (event->key.keysym.scancode == SDL_SCANCODE_RETURN && game_mode == GameMode::MENU){
				game_mode = GameMode::PLAYING; }
				if (event->key.keysym.scancode == SDL_SCANCODE_ESCAPE) LockUnlockCursor();
				if (event->key.keysym.scancode == SDL_SCANCODE_F3) Crash();
				if (game_mode == GameMode::MENU) return;
				if (event->key.keysym.scancode == SDL_SCANCODE_M) player->NextMode();
				if (event->key.keysym.scancode == SDL_SCANCODE_SPACE) player->Jump(5.0f, map->floor_level); 
			}
		}
	}
	void ReadKeyboardInput(const uint8_t* keystate, float delta_time){
		if (game_mode == GameMode::MENU) return;
		//READ
		Vector3 movement = {0.0f, 0.0f, 0.0f};
		if (keystate[SDL_SCANCODE_W]) movement.y += 1.0f;
		if (keystate[SDL_SCANCODE_S]) movement.y -= 1.0f;
		if (keystate[SDL_SCANCODE_D]) movement.x += 1.0f;
		if (keystate[SDL_SCANCODE_A]) movement.x -= 1.0f;
		if (keystate[SDL_SCANCODE_E]) movement.z += 1.0f;
		if (keystate[SDL_SCANCODE_Q]) movement.z -= 1.0f;
		//MOVE
		player->Go(*map, movement, delta_time);
	}
	void ReadMouseInput(){
		if (game_mode == GameMode::MENU) return;
		int mouse_x, mouse_y;
		SDL_GetRelativeMouseState(&mouse_x, &mouse_y);
		player->Turn(-mouse_x, Game::sensitivity);
	}
};

int main(int argc, char* argv[])
{
	Mathematics::FillFastCosinusArray();
	Mathematics::FillFastSinusArray();
	//SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) Game::Crash("SDL INIT ERROR");
    //TTF
    TTF_Init();
    TTF_Font* font = TTF_OpenFont("assets/fonts/Joy Circuit.otf", 32);
    if(!font) Game::Crash("FONT DIDN'T LOAD");
    //WINDOW
	SDL_Window* window = SDL_CreateWindow(Game::name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Game::screen_w, Game::screen_h, SDL_WINDOW_RESIZABLE);
	if(window == nullptr) Game::Crash("WINDOW WEREN'T CREATED");
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(renderer == nullptr) Game::Crash("RENDER WEREN'T CREATED");
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); //For transparency
	SDL_SetRelativeMouseMode(SDL_TRUE);	//Lock cursor
	SDL_Event event;
	const uint8_t* keystate;
	//ENTITIES
	Map world_map("arena.txt");
	Camera cam(Game::screen_w, Game::screen_h, Game::fov, Game::render_plane_height, Game::render_quality, Game::render_distance, {-5.0f, -40.0f, 20.0f}, Mathematics::pi/3.0f);
	Player player(5.0f, 1.0f, {25.0f, 25.0f, 10.0f}, Mathematics::pi/2.0f, 1.5f);
	Game::map = &world_map;
	Game::camera = &cam;
	Game::player = &player;
	//DELTA TIME
	uint64_t last_ticks_count = SDL_GetPerformanceCounter();
	uint64_t current_ticks_count = SDL_GetPerformanceCounter();	
	float delta_time = 0.0f;
	//MENU TEXT
	TextBox game_name_text(renderer, Game::name, font, {255, 255, 0, 255});
	TextBox game_version_text(renderer, Game::version, font, {0, 255, 100, 255});
	TextBox press_enter_text(renderer, "press enter to continue", font, {0, 205, 255, 255});
	//FPS
	int fps = 0;
    int fps_text_render_counter = 0;
	TextBox fps_text(renderer, "hello world", font, {255, 0, 0, 255});
	//GAME LOOP
	while (Game::game_running) {
		//FPS CONTROL
		current_ticks_count = SDL_GetPerformanceCounter();
		delta_time = (float)(current_ticks_count - last_ticks_count) / (float)SDL_GetPerformanceFrequency();
		fps = 1.0f/delta_time;
		last_ticks_count = current_ticks_count;
		//EVENTS
		Game::EventHandler(&event);
		Game::UpdateBlinkers(delta_time);
        //KEYBOARD
        keystate = SDL_GetKeyboardState(NULL);
        Game::ReadKeyboardInput(keystate, delta_time);
		if(Game::game_mode != GameMode::MENU) cam.MoveTo(player.GetEyePosition());
		//MOUSE
		Game::ReadMouseInput();
		if(Game::game_mode != GameMode::MENU) cam.TurnToAngle(player.angle_);
		//PHYSICS
		player.Gravitate(delta_time, Game::gravity, world_map.floor_level);
		player.ApplySpeed(delta_time, world_map.floor_level);
		//GRAPHICS
        cam.Render(renderer, world_map);
        //MENU
        if(Game::game_mode == GameMode::MENU){
			SDL_SetRenderDrawColor(renderer, 0, 0, 20, 128);
			SDL_Rect menu_bg = {0, 0, Game::screen_w, Game::screen_h};
			SDL_RenderFillRect(renderer, &menu_bg);
			if(Game::blinker1s){
				game_name_text.Draw(renderer, {(Game::screen_w - 600)/2, 0, 600, 50});
			}
			game_version_text.Draw(renderer, {Game::screen_w - 300, Game::screen_h - 100, 280, 80});
			press_enter_text.Draw(renderer, {0, 300, 800, 50});
		}
        //FPS TEXT
        if(fps_text_render_counter > 1000){
			fps_text_render_counter = 0;
			char fps_string[64];
			char buffer[32];
			WriteTextToString_CH(fps_string, "fps = ", 64);
			WriteIntToString_CH(buffer, fps, 32);
			AddTextToString_CH(fps_string, buffer, 64);
			fps_text.Update(renderer, fps_string, font, {255, 0, 0, 255}, false);
		}else{
			fps_text_render_counter ++;
		}
		fps_text.Draw(renderer, { 0, 50, 200, 50 });
        SDL_RenderPresent(renderer);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
