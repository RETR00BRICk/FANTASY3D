#pragma once

namespace Blinkers{
	struct Square{
		float timer;
		float reset_time;
		bool state = false;
		Square(float start_time, float reset_time): timer(start_time), reset_time(reset_time){}
		void Update(float dt);
	};
}

