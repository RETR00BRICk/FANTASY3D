#include <cmath>
#include "mathematics.h"
#include "physics_body.h"
#include "player.h"

Player::Player(float mxs, float acc, float friction, Mathematics::Vector3 pos, float ang, float height): 
angle_(ang), friction_(friction), max_speed_(mxs), acceleration_(acc){
	body_.pos = pos;
	body_.height = height;
}

Mathematics::Vector3 Player::GetEyePosition(){
	return body_.pos + Mathematics::Vector3{0.0f, 0.0f, body_.height};
}

void Player::Go(const Mathematics::Vector3& input, float dt){
	if(input.x == 0.0f && input.y == 0.0f && input.z == 0.0f) return;
	Mathematics::Vector2 fwd = Mathematics::GetVectorByAngle2(angle_);
	Mathematics::Vector2 right = {fwd.y, -fwd.x};
	fwd *= input.y;
	right *= input.x;
	Mathematics::Vector2 movement = fwd + right;	//1.41X SPEED WHEN MOVING DIAGONALLY IS A FEATURE
	movement *= GetSpeedValue();
	body_.AccelerateToSpeedVector(movement, GetAcceleration(), dt);
	if(input.z == 0.0f or mode_ != PlayerMode::CHEATER) return;
	body_.MoveByVector({0.0f, 0.0f, input.z * GetSpeedValue()}, dt);
}

void Player::Die(){
	
}

void Player::Damage(float value){
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

void Player::NextMode(){
	int next = (int)mode_ + 1;
	if(next == (int)PlayerMode::MAX){
		mode_ = PlayerMode::DEFAULT;
	}else{
		mode_ = (PlayerMode)next;
	}
	body_.Stop(false, false, true);
}

void Player::Respawn(bool save_inventory){
	health_ = 100.0f;
	if(!save_inventory){
		armor_ = 0.0f;
	}
}

void Player::RegainHealth(float value){
	if(health_ < 100.0f){
		health_ += value;
		if(health_ > 100.0f) health_ = 100.0f;
	}
}

void Player::RegainStamina(float value){
	if(health_ > 10.0f && stamina_ < 100.0f){
		stamina_ += value;
		if(stamina_ > 100.0f) stamina_ = 100.0f;
	}
}

void Player::Jump(float speed, float floor_level){
	if(mode_ == PlayerMode::CHEATER) return;
	if(std::abs(body_.pos.z - floor_level) < 0.001f){
		body_.curr_speed.z = speed;
	}
}

void Player::MoveZ(float input, float dt){
	if(input == 0.0f) return;
	body_.MoveByVector({0.0f,0.0f,GetSpeedValue()*input}, dt);
}

void Player::Turn(float input, float multiplier){
	angle_ += input*multiplier;
	angle_ = Mathematics::GetClampedAngle(angle_);
}

void Player::Gravitate(float dt){
	if(mode_ == PlayerMode::CHEATER) return;
	body_.Gravitate(dt);
}

void Player::AddFriction(float floor_level, float dt){
	float friction = GetFrictionKoef(floor_level);
	body_.AddFriction(friction, dt);
}

void Player::ApplySpeed(float dt, const Map& map){
	if(mode_ == PlayerMode::CHEATER) body_.ApplySpeed(dt);
	else body_.TryApplySpeed(map, dt);
}

float Player::GetAcceleration(){
	if(mode_ == PlayerMode::CHEATER) return acceleration_ * 10.0f;
	return acceleration_;
}

float Player::GetFrictionKoef(float floor_level){
	if(mode_ == PlayerMode::CHEATER) return friction_ * 10.0f;
	if(body_.pos.z - floor_level > 0.001f) return 0.0f;
	return friction_;
}

float Player::GetSpeedValue(){	
	if(health_ <= 0.001f) return 0.0f;
	if(mode_ == PlayerMode::CHEATER) return max_speed_*5.0f;
	float multiplier = 1.0f;
	if(health_ < 10.0f) multiplier *= 0.5f;
	if(stamina_ < 10.0f) multiplier *= 0.5f;
	if(armor_ > 90.0f) multiplier *= 0.9f;
	return max_speed_*multiplier;
}
