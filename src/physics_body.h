#pragma once
#include <cmath>
#include "mathematics.h"

struct Map; 

struct PhysicsBody{
	float mass = 1.0f;
	Mathematics::Vector3 pos = {0.0f,0.0f,0.0f};
	float height = 1.0f;
	Mathematics::Vector3 curr_speed = {0.0f,0.0f,0.0f};
	static inline float gravity = 9.81f;
	//TELEPORT
	void Teleport3(const Mathematics::Vector3& vector);
	void Teleport2(const Mathematics::Vector2& vector);
	//LINEAR MOVING
	void MoveByVector(const Mathematics::Vector3& vector, float dt);
	void MoveByAngle(float angle, float value, float dt);
	void ApplySpeed(float dt);
	void TryApplySpeed(const Map& map, float dt);
	void Stop(bool x = true, bool y = true, bool z = true);
	//ACCELERATION
	void AccelerateByVector(const Mathematics::Vector3& accel, float dt);
	void Gravitate(float dt);
	void AccelerateByAngle(float angle, float accel, float dt);
	void AccelerateToSpeedVector(const Mathematics::Vector2& target_spd_vec, float accel, float dt);
	//FORCE
	void AddForceByVector(const Mathematics::Vector3& force, float dt);
	void AddForceByAngle(float angle, float force, float dt);
	//FRICTION
	void AddFriction(float friction, float dt);
};
