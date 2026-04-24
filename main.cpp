#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
//This code is garbage
//_CH in the function's name means it changes the values, given to the function in the ( )
//Get in the function's name means it returns the value. But some functions can return data even if "Get" is absent
//FISH EYE EFFECT IS NOT A BUG ITS A FEATURE
//Made by RETR00BRICk
enum class EntityType{
	WALL,
	SPRITE
};

namespace StringFunctions{
	bool CompareStrings(const char* string0, const char* string1, int string_max_size){
		int i = 0;
		while(string0[i] != '\0' && string1[i] != '\0' && i < string_max_size - 1){
			if(string0[i] != string1[i]) return false;
			i ++;
		}
		return string0[i] == string1[i];
	}
	//CHAR
	bool IsCharNumeric(char checking_char){
		return checking_char == '-' || ('0' <= checking_char && checking_char <= '9');
	}
	bool IsCharDigit(char checking_char){
		return '0' <= checking_char && checking_char <= '9';
	}
	//TEXT
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
	//INT
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
			int reversed_nums_cursor = counter - 1;
			while(reversed_nums_cursor >= 0 && cursor < string_max_size - 1){
				string[cursor] = reversed_numbers[reversed_nums_cursor];
				cursor ++;
				reversed_nums_cursor --;
			}
			string[cursor] = '\0';
		}
	}
	void AddIntToString_CH(char* string, int integer, int string_max_size){
		int string_cursor = 0;
		while(string[string_cursor] != '\0' && string_cursor < string_max_size - 1){
			string_cursor++;
		}

		if(integer == 0){
			if(string_cursor < string_max_size - 1){ 
				string[string_cursor] = '0';
				string[string_cursor+1] = '\0';
				return;
			}
		}else{
			if(string_cursor < string_max_size - 1){
				if(integer < 0){
					string[string_cursor] = '-';
					integer = -integer;
					string_cursor++;
				}
			}else return; 
			
			int digits_count = 0;
			int remaining_number = integer;
			int reversed_numbers[16];
			
			while(remaining_number > 0){
				reversed_numbers[digits_count] = remaining_number%10;
				remaining_number /= 10;
				digits_count ++;
			}
			int reversed_nums_cursor = digits_count - 1;
			while(reversed_nums_cursor >= 0 && string_cursor < string_max_size - 1){
				char digit = '0' + reversed_numbers[reversed_nums_cursor];
				string[string_cursor] = digit;
				reversed_nums_cursor --;
				string_cursor ++;
			}
			string[string_cursor] = '\0';
		}
	}
	//CONVERSION FROM STRING
	int GetIntFromString(const char* string, int string_max_size){
		int string_cursor = 0;
		bool negative = false;
		int result = 0;
		if(string_cursor < string_max_size){
			negative = string[string_cursor] == '-';
			if(negative) string_cursor++;
		}else return 0;
		while(string_cursor < string_max_size && string[string_cursor] != '\0'){
			int symbol_code = string[string_cursor];
			if(IsCharDigit(symbol_code)){
				result = result*10 + (symbol_code - '0');
				string_cursor ++;
			}else return 0;
		}
		if(negative) result*=-1;
		return result;
	}
	//FOR FILE PARCER
	bool TryReadNumberFromString_CH(int& number, int& cursor, const char* string, int string_max_size){
		if(StringFunctions::IsCharNumeric(string[cursor])){
			int number_length = 0;
			int number_start = cursor;
			cursor += string[cursor] == '-';
			number_length += string[number_start] == '-';
			while(cursor < string_max_size && StringFunctions::IsCharDigit(string[cursor])){
				number_length ++;
				cursor ++;
			}
			number = StringFunctions::GetIntFromString(&string[number_start], number_length);
			return true;
		}else{
			return false;
		}
	}
	void TrySkipComment(int& cursor, const char* string, int string_max_size){
		if(string[cursor] == '#'){
			while(cursor < string_max_size && string[cursor] != '\n') cursor ++;	
		}
	}
	bool TryLoadIntArrayFromString_CH(int* num_array, int num_array_size, int& cursor, const char* string, int string_max_size){
         int read_numbers_count = 0;
         while(cursor < string_max_size){
            TrySkipComment(cursor, string, string_max_size);
            if(cursor == string_max_size) return false;
            int value;
            if(TryReadNumberFromString_CH(value, cursor, string, string_max_size)){
                num_array[read_numbers_count] = value;
                read_numbers_count++;
                if(read_numbers_count == num_array_size){
                    return true;
                }
            }else{
                cursor++;
            }
         }
         return false;
    } 
};

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
	float GetLenAndNormalizeVec2_CH(Vector2& vector){
		float length = GetHypot2D(vector);
		if(std::abs(length) <= 0.001f) return 0.0f;
		float inv_length = 1.0f / length;
		vector *= inv_length;
		return length;
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
		char file_path[32] = "maps/";
		StringFunctions::AddTextToString_CH(file_path, file_name, 32);
		//TRYING TO READ THE FILE
		SDL_RWops* file = SDL_RWFromFile(file_path, "rb");
		if (!file) return;
		int file_size = SDL_RWsize(file);	
		if (file_size < 1) return;
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
	bool CheckCollision(const Vector3& coordinate, float side_offset = 0.2f, float height = 0.0f) const{
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
	void MoveByVector(const Vector3& vector, float dt){
		pos += vector * dt;
	}
	void MoveByAngle(float angle, float value, float dt){
		MoveByVector(Mathematics::GetVectorByAngle3(angle) * value, dt);
	}
	
	void ApplySpeed(float dt){
		pos += curr_speed * dt;
	}
	void TryApplySpeed(const Map& map, float dt){
		float offset = 0.2f;
		if(curr_speed.x != 0.0f or curr_speed.y != 0.0f){
			//X
			Vector3 new_pos = pos + Vector3{curr_speed.x, 0.0f, 0.0f} * dt;
			if(!map.CheckCollision(new_pos, offset, height)){
				pos = new_pos; //If there were no collision, continue moving by the given vector
			}else{
				curr_speed.x *= -0.5f; //bounce of a wall
			}
			//Y
			new_pos = pos + Vector3{0.0f, curr_speed.y, 0.0f} * dt;
			if(!map.CheckCollision(new_pos, offset, height)){
				pos = new_pos; //If there were no collision, continue moving by the given vector
			}else{
				curr_speed.y *= -0.5f; //bounce of a wall
			}
		}
		if(curr_speed.z != 0.0f){
			Vector3 new_pos = pos + Vector3{0.0f, 0.0f, curr_speed.z} * dt;
			if(new_pos.z > map.floor_level && !map.CheckCollision(new_pos, offset, height)){
				pos = new_pos;
			}else{
				if(new_pos.z < map.floor_level) pos.z = map.floor_level;
				curr_speed.z = 0.0f;
			}
		}
	}
	void Stop(const Vector3& stop_amount_vec){
		curr_speed.x *= (1.0f - stop_amount_vec.x);
		curr_speed.y *= (1.0f - stop_amount_vec.y);
		curr_speed.z *= (1.0f - stop_amount_vec.z);
	}
	//ACCELERATION
	void AccelerateByVector(const Vector3& accel, float dt){
		curr_speed += accel * dt;
	}
	void AccelerateByAngle(float angle, float accel, float dt){
		AccelerateByVector(Mathematics::GetVectorByAngle3(angle) * accel, dt);
	}
	void AccelerateToSpeedVector(const Vector2& target_spd_vec, float accel, float dt){ //Written by the idea of Quake 1 code
		Vector2 target_spd_dir = target_spd_vec;
		float target_spd = Mathematics::GetLenAndNormalizeVec2_CH(target_spd_dir);
		
		Vector2 curr_speed_2d = {curr_speed.x, curr_speed.y};
		float curr_spd_in_target_dir = Mathematics::GetDotProduct2(curr_speed_2d, target_spd_dir);
		
		if(curr_spd_in_target_dir > target_spd) return;
		curr_speed.x += target_spd_dir.x * accel * dt;
		curr_speed.y += target_spd_dir.y * accel * dt;
	}
	//FORCE
	void AddForceByVector(const Vector3& force, float dt){
		float inv_mass = 1.0f/mass;
		AccelerateByVector(force * inv_mass, dt);
	}
	void AddForceByAngle(float angle, float force, float dt){
		float inv_mass = 1.0f/mass;
		AccelerateByAngle(angle, force * inv_mass, dt);
	}
	//FRICTION
	void AddFriction(float friction, float dt){
		float per_frame_friction = friction*dt;
		float curr_spd_squared = curr_speed.x*curr_speed.x + curr_speed.y*curr_speed.y;
		if(curr_spd_squared > per_frame_friction*per_frame_friction){
			float curr_spd_magnitude = std::sqrt(curr_spd_squared);
			float scalar = (curr_spd_magnitude - per_frame_friction) / curr_spd_magnitude;
			curr_speed.x *= scalar;
			curr_speed.y *= scalar;
		}else{
			curr_speed.x = 0.0f;
			curr_speed.y = 0.0f;
		}
	}
};

class Player{
	enum class PlayerMode{
		DEFAULT,
		CHEATER,
		MAX
	};
public:
    //Transform
	float angle_;
	//Physics
	PhysicsBody body_;
	//FUNCTIONS
	Player(float mxs = 5.0f, float acc = 1.0f, float friction = 0.5f, Vector3 pos = {0.0f, 0.0f, 0.0f}, float ang = 0.0f, float height = 1.5f): 
	angle_(ang), friction_(friction), max_speed_(mxs), acceleration_(acc){
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
		body_.Stop({0.0f,0.0f,1.0f});
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
	//STAMINA
	void RegainStamina(float value){
		if(health_ > 10.0f && stamina_ < 100.0f){
			stamina_ += value;
			if(stamina_ > 100.0f) stamina_ = 100.0f;
		}
	}
	//MOVEMENT AND ROTATION
	void Go(const Map& map, const Vector3& input, float dt){
		if(input.x == 0.0f && input.y == 0.0f && input.z == 0.0f) return;
		Vector2 fwd = Mathematics::GetVectorByAngle2(angle_);
		Vector2 right = {fwd.y, -fwd.x};
		fwd *= input.y;
		right *= input.x;
		Vector2 movement = fwd + right;	//1.41X SPEED WHEN MOVING DIAGONALLY IS A FEATURE
		movement *= GetSpeedValue();
		body_.AccelerateToSpeedVector(movement, GetAcceleration(), dt);
		if(input.z == 0.0f or mode_ != PlayerMode::CHEATER) return;
		body_.MoveByVector({0.0f, 0.0f, input.z * GetSpeedValue()}, dt);
	}
	void Jump(float speed, float floor_level){
		if(mode_ == PlayerMode::CHEATER) return;
		if(std::abs(body_.pos.z - floor_level) < 0.001f){
			body_.curr_speed.z = speed;
		}
	}
	void MoveZ(float input, float dt){
		if(input == 0.0f) return;
		body_.MoveByVector({0.0f,0.0f,GetSpeedValue()*input}, dt);
	}
	void Turn(float input, float multiplier){
		angle_ += input*multiplier;
		angle_ = Mathematics::GetClampedAngle(angle_);
	}
	void Gravitate(float acceleration, float dt){
		if(mode_ == PlayerMode::CHEATER) return;
		body_.AccelerateByVector({0.0f,0.0f,-acceleration}, dt);
	}
	void AddFriction(float floor_level, float dt){
		float friction = GetFrictionKoef(floor_level);
		body_.AddFriction(friction, dt);
	}
	void ApplySpeed(float dt, const Map& map){
		if(mode_ == PlayerMode::CHEATER) body_.ApplySpeed(dt);
		else body_.TryApplySpeed(map, dt);
	}
	//GETTERS
	Vector3 GetEyePosition(){
		return body_.pos + Vector3{0.0f, 0.0f, body_.height};
	}
	
private:
	//Movement
	float friction_;
	float max_speed_;
	float acceleration_;
	//Player
	PlayerMode mode_ = PlayerMode::DEFAULT;
	float health_ = 100.0f;
	float stamina_ = 100.0f;
	float armor_ = 0;
	//FUNCTIONS
	float GetSpeedValue(){	
		if(health_ <= 0.001f) return 0.0f;
		if(mode_ == PlayerMode::CHEATER) return max_speed_*5.0f;
		float multiplier = 1.0f;
		if(health_ < 10.0f) multiplier *= 0.5f;
		if(stamina_ < 10.0f) multiplier *= 0.5f;
		if(armor_ > 90.0f) multiplier *= 0.9f;
		return max_speed_*multiplier;
	}
	float GetAcceleration(){
		if(mode_ == PlayerMode::CHEATER) return acceleration_ * 10.0f;
		return acceleration_;
	}
	float GetFrictionKoef(float floor_level){
		if(mode_ == PlayerMode::CHEATER) return friction_ * 10.0f;
		if(body_.pos.z - floor_level > 0.001f) return 0.0f;
		return friction_;
	}
};


class Camera{
public:
	Camera(int out_w, int out_h, Vector3 pos = {0.0f, 0.0f, 0.0f}, float ang = 0.0f):
	out_w_(out_w),out_h_(out_h),pos_(pos),angle_(ang){
		aspect_ratio_ = (float)out_w/(float)out_h;
		actual_render_plane_height_ = render_plane_height_/aspect_ratio_;
		LoadSettings();
	}
	//SETTERS
	void SetOutputImageSize(int w, int h){
		out_w_ = w; out_h_ = h;
		aspect_ratio_ = (float)w/(float)h;
		actual_render_plane_height_ = render_plane_height_/aspect_ratio_;
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
		DrawBg(renderer, map.fog_color);
		DrawFloor(renderer, map.floor_color, map.fog_color, map.floor_level);
		//RAYS DIRECTIONS
		float angle_betw_rays = fov_/lines_count_;	//angle between 2 closest rays. Identical for every ray
		int max_hit_per_ray = 10;
		Vector2 ray_directions[lines_count_];
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
	Vector3 pos_;
	float angle_;
	//FUNCTIONS
	void LoadSettings(){
		SDL_RWops* file = SDL_RWFromFile("settings/camera.txt", "rb");
		if (!file) return;
		int file_size = SDL_RWsize(file);
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
	void SetNearWallIndexes_CH(const Wall* walls, int walls_count, bool* out_indexes_array){
		for(int i = 0; i < walls_count; i++){
			float distance = Mathematics::GetFastDistance2D({pos_.x,pos_.y}, walls[i].pos_m);
			out_indexes_array[i] = distance <= render_distance_*1.5f;
		}
	}
	bool TryGetDistanceOfRayIntersectionWithWall_CH(const Wall& wall, const Vector2& ray_dir, float& dist){ 
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
	void DrawFloor(SDL_Renderer* renderer, const SDL_Color& color, const SDL_Color& fog_color, float floor_level){
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
	void DrawGrid(SDL_Renderer* renderer, const Vector2* ray_directions, float floor_level){
		SDL_Rect all_dots[10*lines_count_]; //it won't overflow (im too lazy to explain why, go think yourself)
		int all_dots_count = 0;
		float delta_z = floor_level - pos_.z;
		for(int line_num = 0; line_num < lines_count_; line_num += 2){
			const Vector2& ray_dir = ray_directions[line_num]; //ray
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
	void DrawHitEntities(SDL_Renderer* renderer, const HitInfo* array, int array_size, const SDL_Color& fog_color, float floor_level){
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
		if(!f && StringFunctions::CompareStrings(text, last_text, max_text_len)) return;
		StringFunctions::WriteTextToString_CH(last_text, text, max_text_len);
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

struct Blinker{
	float timer;
	float reset_time;
	bool state = false;
	
	Blinker(float start_time, float reset_time): timer(start_time), reset_time(reset_time){}
	
	void Update(float dt){
		timer += dt;
		if(timer > reset_time){
			state = !state;
			timer -= reset_time;
		}
	}
};

namespace Game{
	enum class GameMode{
		MENU,
		PLAYING,
		PAUSED
	};
	constexpr char version[] = "V0.02 FOG UPDATE";
	constexpr char name[] = "FANTASY3D";
	//GAME
	bool game_running = true;
	Player* player = nullptr;
	Camera* camera = nullptr;
	Map* map = nullptr;
	GameMode game_mode = GameMode::MENU;
	//BLINKERS
	Blinker blinker_1s(0.0f, 1.0f);
	//PHYSICS
	float gravity = 9.81f;
	//SETTINGS
	int screen_w = 800;
	int screen_h = 600;
	int target_fps = 60;
	bool cursor_locked = true;
	float sensitivity = 0.001f;
	//FUNCTIONS	
	void UpdateBlinkers(float dt){
		blinker_1s.Update(dt);
	}
	void LoadSettings(){
		SDL_RWops* file = SDL_RWFromFile("settings/global.txt", "rb");
		if (!file) return;
		int file_size = SDL_RWsize(file);
		char char_array[file_size]; //If it doesn't fit on the stack, it is the user fault, not mine!
		SDL_RWread(file, char_array, 1, file_size);
		SDL_RWclose(file);
		int settings[5];
		int cursor = 0;
		if(StringFunctions::TryLoadIntArrayFromString_CH(settings, 5, cursor, char_array, file_size)){
			screen_w = settings[0];
			screen_h = settings[1];
			sensitivity = (float)settings[2]/10000.0f;
			cursor_locked = (bool)settings[3];
			target_fps = settings[4];
		}
	}
	void Crash(const char* message = "UKNOWN ERROR"){
		game_running = false;
		char buffer[512] = "ERROR: ";
		StringFunctions::AddTextToString_CH(buffer, message, 512);
		StringFunctions::AddTextToString_CH(buffer, "\nLAST STD ERROR LOG: ", 512);
		StringFunctions::AddTextToString_CH(buffer, SDL_GetError(), 512);
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
		//PLAYER MOVEMENT
		Vector3 movement = {0.0f, 0.0f};
		if (keystate[SDL_SCANCODE_W]) movement.y += 1.0f;
		if (keystate[SDL_SCANCODE_S]) movement.y -= 1.0f;
		if (keystate[SDL_SCANCODE_D]) movement.x += 1.0f;
		if (keystate[SDL_SCANCODE_A]) movement.x -= 1.0f;
		if (keystate[SDL_SCANCODE_E]) movement.z += 1.0f;
		if (keystate[SDL_SCANCODE_Q]) movement.z -= 1.0f;
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
	Game::LoadSettings();
	//SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) Game::Crash("SDL INIT ERROR");
    TTF_Init();
    TTF_Font* font = TTF_OpenFont("assets/fonts/Joy Circuit.otf", 32);
    if(!font) Game::Crash("FONT DIDN'T LOAD");
	SDL_Window* window = SDL_CreateWindow(Game::name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Game::screen_w, Game::screen_h, SDL_WINDOW_RESIZABLE);
	if(window == nullptr) Game::Crash("WINDOW WEREN'T CREATED");
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(renderer == nullptr) Game::Crash("RENDER WEREN'T CREATED");
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); //For transparency
	SDL_SetRelativeMouseMode((SDL_bool)Game::cursor_locked);	//Lock cursor
	SDL_Event event;
	const uint8_t* keystate;
	//ENTITIES
	Map world_map("arena.txt");
	Camera cam(Game::screen_w, Game::screen_h, {-5.0f, -40.0f, 20.0f}, Mathematics::pi/3.0f);
	Player player(4.0f, 20.0f, 10.0f, {25.0f, 25.0f, 10.0f}, Mathematics::pi/2.0f, 1.5f); //25 25
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
		last_ticks_count = current_ticks_count;
		//PHYSICS 1
		player.Gravitate(Game::gravity, delta_time);
		player.AddFriction(world_map.floor_level, delta_time);
		//EVENTS
		Game::EventHandler(&event);
		Game::UpdateBlinkers(delta_time);
        //KEYBOARD
        keystate = SDL_GetKeyboardState(NULL);
        Game::ReadKeyboardInput(keystate, delta_time);
		if(Game::game_mode != Game::GameMode::MENU) cam.MoveTo(player.GetEyePosition());
		//MOUSE
		Game::ReadMouseInput();
		if(Game::game_mode != Game::GameMode::MENU) cam.TurnToAngle(player.angle_);
		//PHYSICS 2
		player.ApplySpeed(delta_time, world_map);
		//GRAPHICS
        cam.Render(renderer, world_map);
        //MENU
        if(Game::game_mode == Game::GameMode::MENU){
			SDL_SetRenderDrawColor(renderer, 0, 0, 20, 128);
			SDL_Rect menu_bg = {0, 0, Game::screen_w, Game::screen_h};
			SDL_RenderFillRect(renderer, &menu_bg);
			if(Game::blinker_1s.state){
				game_name_text.Draw(renderer, {(Game::screen_w - 600)/2, 0, 600, 50});
			}
			game_version_text.Draw(renderer, {Game::screen_w - 300, Game::screen_h - 100, 280, 80});
			press_enter_text.Draw(renderer, {0, 300, 800, 50});
		}
        //FPS TEXT
        if(fps_text_render_counter > 1000){
        	fps = 1.0f/delta_time;
			fps_text_render_counter = 0;
			char fps_string[64] = "fps = ";
			StringFunctions::AddIntToString_CH(fps_string, fps, 64);
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
