#include "physics_body.h"
#include <cmath>
#include "mathematics.h"
#include "map.h"


void PhysicsBody::Teleport3(const Mathematics::Vector3& vector){
	pos = vector;
}

void PhysicsBody::Teleport2(const Mathematics::Vector2& vector){
	pos.x = vector.x;
	pos.y = vector.y;
}

void PhysicsBody::MoveByVector(const Mathematics::Vector3& vector, float dt){
	pos += vector * dt;
}

void PhysicsBody::MoveByAngle(float angle, float value, float dt){
	MoveByVector(Mathematics::GetVectorByAngle3(angle) * value, dt);
}

void PhysicsBody::ApplySpeed(float dt){
	pos += curr_speed * dt;
}

void PhysicsBody::Stop(bool x, bool y, bool z){
	curr_speed.x *= !x;
	curr_speed.y *= !y;
	curr_speed.z *= !z;
}

void PhysicsBody::AccelerateByVector(const Mathematics::Vector3& accel, float dt){
	curr_speed += accel * dt;
}

void PhysicsBody::Gravitate(float dt){
	curr_speed -= Mathematics::Vector3{0.0f, 0.0f, gravity} * dt;
}

void PhysicsBody::AccelerateByAngle(float angle, float accel, float dt){
	AccelerateByVector(Mathematics::GetVectorByAngle3(angle) * accel, dt);
}

void PhysicsBody::AccelerateToSpeedVector(const Mathematics::Vector2& target_spd_vec, float accel, float dt){ //Written by the idea of Quake 1 code
	Mathematics::Vector2 target_spd_dir = target_spd_vec;
	float target_spd = Mathematics::GetLenAndNormalizeVec2_CH(target_spd_dir);
	
	Mathematics::Vector2 curr_speed_2d = {curr_speed.x, curr_speed.y};
	float curr_spd_in_target_dir = Mathematics::GetDotProduct2(curr_speed_2d, target_spd_dir);
	
	if(curr_spd_in_target_dir > target_spd) return;
	curr_speed.x += target_spd_dir.x * accel * dt;
	curr_speed.y += target_spd_dir.y * accel * dt;
}

void PhysicsBody::AddForceByVector(const Mathematics::Vector3& force, float dt){
	float inv_mass = 1.0f/mass;
	AccelerateByVector(force * inv_mass, dt);
}

void PhysicsBody::AddForceByAngle(float angle, float force, float dt){
	float inv_mass = 1.0f/mass;
	AccelerateByAngle(angle, force * inv_mass, dt);
}

void PhysicsBody::AddFriction(float friction, float dt){
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

void PhysicsBody::TryApplySpeed(const Map& map, float dt){
	float offset = 0.2f;
	if(curr_speed.x != 0.0f or curr_speed.y != 0.0f){
		//X
		Mathematics::Vector3 new_pos = pos + Mathematics::Vector3{curr_speed.x, 0.0f, 0.0f} * dt;
		if(!map.CheckCollision(new_pos, offset, height)){
			pos = new_pos; //If there were no collision, continue moving by the given vector
		}else{
			curr_speed.x *= -0.5f; //bounce of a wall
		}
		//Y
		new_pos = pos + Mathematics::Vector3{0.0f, curr_speed.y, 0.0f} * dt;
		if(!map.CheckCollision(new_pos, offset, height)){
			pos = new_pos; //If there were no collision, continue moving by the given vector
		}else{
			curr_speed.y *= -0.5f; //bounce of a wall
		}
	}
	if(curr_speed.z != 0.0f){
		Mathematics::Vector3 new_pos = pos + Mathematics::Vector3{0.0f, 0.0f, curr_speed.z} * dt;
		if(new_pos.z > map.floor_level && !map.CheckCollision(new_pos, offset, height)){
			pos = new_pos;
		}else{
			if(new_pos.z < map.floor_level) pos.z = map.floor_level;
			curr_speed.z = 0.0f;
		}
	}
}
