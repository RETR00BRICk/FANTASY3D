# *FANTASY3D*
Hello! This is my first C++ project with graphics.
It's a pseudo 3D engine. Uses raycasting technology to render.
![Demo](assets/gifs/demo3.gif)
## SETTINGS/PHYSICS/GRAPHICS UPDATE 0.03
* New functions for operations with strings (char arrays)
* Added white coordinate grid on the floor
* Added realistic player movement with acceleration and friction
* Remade loading from files, removed `fstream` library
* Added settings loading (settings/camera.txt and settings/global.txt)
* Render() function in `class Camera` is heavily refactored
* Some other code refactoring, optimizations

## What can it do now:
* Loading settings from text files (mouse sensitivity, fov, screen resolution etc.) 
* Render the map, provided in "maps/arena.txt" (with fog!)
* Render the grid on the floor with size of 1 meter
* Collisions with the map, supports moving by Z-axis (vertical one)
* 2 player modes: cheater (when you can fly and no-clip) and default (when you only can walk and jump)
* Quake-inspired player movement with acceleration, friction and strafe-jumping
* Simple gravity: player falls down in default mode
* Resizing the window keeps view propotions the same

## Controls:
- W,A,S,D - movement
- Q,E - move up/down (only in cheater mode)
- ESC - lock/unlock cursor
- M - change player mode: cheater (you can fly) and default (only walk and jump)
- ENTER (in menu) - start game
- F3 - crash the game

## What libraries does it use?
1. SDL2
2. SDL2_ttf
3. ~~fstream~~ no longer needed!
4. cmath

**How to install needed libraries? (Ubuntu/Debian/Mint)**:  
`sudo apt update`  
`sudo apt install libsdl2-dev libsdl2-ttf-dev`

**How to build the executable (linux)**:  
`g++ -Wall -o "main" "main.cpp" -lSDL2 -lSDL2_ttf`

**How to compile (linux)**:  
`g++ -Wall -c "main.cpp"`