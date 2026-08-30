#pragma once

namespace Blinkers{
	struct Square{
		float timer;
		float reset_time;
		bool state = false;
		void Update(float dt);
	};
}
