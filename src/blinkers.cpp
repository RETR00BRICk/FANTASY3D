#include "blinkers.h"

namespace Blinkers{
	void Square::Update(float dt){
		timer += dt;
		if(timer > reset_time){
			state = !state;
			timer -= reset_time;
		}
	}
}
