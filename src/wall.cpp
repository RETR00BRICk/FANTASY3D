#include "wall.h"
#include "mathematics.h"
#include <SDL2/SDL.h>

void Wall::CalculateMiddlePoint(){
	pos_m = Mathematics::CalculateMiddlePoint2D(pos0, pos1);
}
