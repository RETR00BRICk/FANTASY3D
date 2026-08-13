#pragma once
#include <cmath>

namespace Mathematics{
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
	
	inline constexpr int sincos_array_size = 6284;
	inline float fast_sin_array[sincos_array_size] = {};
	inline float fast_cos_array[sincos_array_size] = {};
	inline constexpr float pi = 3.14159265358979f;
	inline constexpr float tau = 2.0f*pi;
	//ANGLES
	inline float GetClampedAngle(float angle){
		while(angle >= tau){ angle -= tau; }
		while(angle < 0){ angle += tau; }
		return angle;
	}
	
	inline void FillFastSinusArray(){
		for(int i = 0; i < sincos_array_size; i ++){
			fast_sin_array[i] = std::sin(i/1000.0f);
		}
	}
	inline void FillFastCosinusArray(){
		for(int i = 0; i < sincos_array_size; i ++){
			fast_cos_array[i] = std::cos(i/1000.0f);
		}
	}
	
	inline float GetFastSinus(float angle){
		angle = GetClampedAngle(angle);
		return fast_sin_array[(int)(angle*1000.0f)];
	}
	inline float GetFastCosinus(float angle){
		angle = GetClampedAngle(angle);
		return fast_cos_array[(int)(angle*1000.0f)];
	}
	//DISTANCES
	inline float GetHypot2D(const Vector2& v){
		return std::sqrt(v.x*v.x + v.y*v.y);
	}
	inline float GetHypot3D(const Vector3& v){
		return std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	}
	
	inline float GetDistance2D(const Vector2& v0, const Vector2& v1){
		return GetHypot2D(v1-v0);
	}
	inline float GetDistance3D(const Vector3& v0, const Vector3& v1){
		return GetHypot3D(v1-v0);
	}
	
	inline float GetFastDistance2D(const Vector2& v0, const Vector2& v1){
		return std::abs(v1.x - v0.x) + std::abs(v1.y - v0.y);
	}
	//VECTORS
	inline Vector2 GetVectorByAngle2(float angle){
		return {GetFastCosinus(angle), GetFastSinus(angle)};
	}
	inline Vector3 GetVectorByAngle3(float angle){
		return {GetFastCosinus(angle), GetFastSinus(angle), 0.0f};
	}
	
	inline Vector2 GetNormalizedVec2(const Vector2& vector){
		float length = GetHypot2D(vector);
		if(std::abs(length) <= 0.001f) return {0.0f,0.0f};
		float inv_length = 1.0f / length;
		return vector*inv_length;		
	}
	inline float GetLenAndNormalizeVec2_CH(Vector2& vector){
		float length = GetHypot2D(vector);
		if(std::abs(length) <= 0.001f) return 0.0f;
		float inv_length = 1.0f / length;
		vector *= inv_length;
		return length;
	}
	inline Vector3 GetNormalizedVec3(const Vector3& vector){
		float length = GetHypot3D(vector);
		if(std::abs(length) <= 0.001f) return {0.0f,0.0f,0.0f};
		float inv_length = 1.0f / length;
		return vector * inv_length;
	}
	inline float GetDotProduct2(const Vector2& v0, const Vector2& v1){
		return v0.x * v1.x + v0.y * v1.y;
	}
	inline float GetDotProduct3(const Vector3& v0, const Vector3& v1){
		return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
	}
	//CHECKS
	inline bool CheckPointOnLine(float line_coordinate0, float line_coordinate1, float checking_coordinate){
		return checking_coordinate >= std::min(line_coordinate0, line_coordinate1) && 
		checking_coordinate <= std::max(line_coordinate0, line_coordinate1);
	}
	inline bool CheckLineOnLine(float start0, float end0, float start1, float end1){
		float max_start = std::max(start0, start1);
		float min_end = std::min(end0, end1);
		return min_end >= max_start;
	}
	inline bool CheckDistance2D(const Vector2& v0, const Vector2& v1, float distance){
		float dx = v1.x - v0.x;
		float dy = v1.y - v0.y;
		return (dx*dx + dy*dy) <= distance*distance;
	}
	inline bool CheckDistance3D(const Vector3& v0, const Vector3& v1, float distance){
		float dx = v1.x - v0.x;
		float dy = v1.y - v0.y;
		float dz = v1.z - v0.z;
		return (dx*dx + dy*dy + dz*dz) <= distance*distance;
	}
	inline Vector2 CalculateMiddlePoint2D(const Vector2& p0, const Vector2& p1){
		Vector2 summ = p0 + p1;
		return summ * 0.5f;
	}
	// LINEAR INTERPOLATION
	inline float LinearInterpolationF(float x0, float x1, float target){
		return x0*(1.0f-target) + x1*target;
	}
	inline int LinearInterpolationInt(int x0, int x1, int target){
		return (x0*(100 - target) + x1*target)/100;
	}
};
