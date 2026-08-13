#pragma once
#include <cmath>
#include "mathematics.h"
#include "physics_body.h"

class Map; 

class Player{
public:
    //Transform
	float angle_;
	//Physics
	PhysicsBody body_;
	//FUNCTIONS
	Player(float mxs = 5.0f, float acc = 1.0f, float friction = 0.5f, Mathematics::Vector3 pos = {0.0f, 0.0f, 0.0f}, float ang = 0.0f, float height = 1.5f);                
	Mathematics::Vector3 GetEyePosition();
	void NextMode();
	//HEALTH AND STAMINA
	void Damage(float value);
	void Die();
	void Respawn(bool save_inventory = false);
	void RegainHealth(float value);
	void RegainStamina(float value);
	//MOVEMENT
	void Go(const Mathematics::Vector3& input, float dt);
	void Jump(float speed, float floor_level);
	void MoveZ(float input, float dt);
	void Turn(float input, float multiplier);
	void Gravitate(float acceleration, float dt);
	void AddFriction(float floor_level, float dt);
	void ApplySpeed(float dt, const Map& map);
private:
	enum class PlayerMode{
		DEFAULT,
		CHEATER,
		MAX
	};
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
	float GetAcceleration();
	float GetFrictionKoef(float floor_level);
	float GetSpeedValue();
};
