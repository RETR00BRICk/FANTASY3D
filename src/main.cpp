#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>

#include "mathematics.h"
#include "string_functions.h"
#include "entity_type.h"
#include "wall.h"
#include "map.h"
#include "blinkers.h"
#include "ui.h"
#include "camera.h"
#include "physics_body.h"
#include "player.h"
//This code is garbage
//_CH in the function's name means it changes the values, given to the function in the ( )
//Get in the function's name means it returns the value. But some functions can return data even if "Get" is absent
//FISH EYE EFFECT IS NOT A BUG ITS A FEATURE
//Made by RETR00BRICk

namespace Game{
	enum class GameMode{
		MENU,
		PLAYING,
		PAUSED
	};
	constexpr char version[] = "V0.04 REFACTORING/ICON LOADING UPDATE";
	constexpr char name[] = "FANTASY3D";
	//GAME
	bool game_running = true;
	Player* player = nullptr;
	Camera* camera = nullptr;
	Map* map = nullptr;
	GameMode game_mode = GameMode::MENU;
	//BLINKERS
	Blinkers::Square blinker_1s(0.0f, 1.0f);
	//PHYSICS
	float gravity = 9.81f;
	//SETTINGS
	int screen_w = 800;
	int screen_h = 600;
	int target_fps = 60;
	bool cursor_locked = true;
	float sensitivity = 0.001f;
	//FUNCTIONS	
	void UpdateBlinkers(float dt){
		blinker_1s.Update(dt);
	}
	void LoadSettings(){
		SDL_RWops* file = SDL_RWFromFile("settings/global.txt", "rb");
		if (!file) return;
		int file_size = SDL_RWsize(file);
		char char_array[file_size]; //If it doesn't fit on the stack, it is the user fault, not mine!
		SDL_RWread(file, char_array, 1, file_size);
		SDL_RWclose(file);
		int settings[5];
		int cursor = 0;
		if(StringFunctions::TryLoadIntArrayFromString_CH(settings, 5, cursor, char_array, file_size)){
			screen_w = settings[0];
			screen_h = settings[1];
			sensitivity = (float)settings[2]/10000.0f;
			cursor_locked = (bool)settings[3];
			target_fps = settings[4];
		}
	}
	void LoadIcon(SDL_Window* window){
		SDL_Surface* icon_surface = SDL_LoadBMP("assets/textures/ICON.bmp");
		if (icon_surface) {
			Uint32 color_key = SDL_MapRGB(icon_surface->format, 255, 0, 255);
			SDL_SetColorKey(icon_surface, SDL_TRUE, color_key);
			SDL_SetWindowIcon(window, icon_surface);
			SDL_FreeSurface(icon_surface);
		}
	}
	void Crash(const char* message = "UKNOWN ERROR"){
		game_running = false;
		char buffer[512] = "ERROR: ";
		StringFunctions::AddTextToString_CH(buffer, message, 512);
		StringFunctions::AddTextToString_CH(buffer, "\nLAST STD ERROR LOG: ", 512);
		StringFunctions::AddTextToString_CH(buffer, SDL_GetError(), 512);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "CRASH!", buffer, NULL);
		SDL_Quit();
		exit(1);
	}
	void LockUnlockCursor(){
		cursor_locked = !cursor_locked;
		SDL_SetRelativeMouseMode((SDL_bool)cursor_locked);
	}
	void EventHandler(SDL_Event* event){
		while (SDL_PollEvent(event)) {
			if (event->type == SDL_QUIT) {
				game_running = false;
			}
			if (event->type == SDL_WINDOWEVENT){
				if (event->window.event == SDL_WINDOWEVENT_RESIZED){
					int w = event->window.data1;
					int h = event->window.data2;
					screen_h = h;
					screen_w = w;
					camera->SetOutputImageSize(w,h);
				}
			}
			if (event->type == SDL_KEYDOWN){
				if (event->key.keysym.scancode == SDL_SCANCODE_RETURN && game_mode == GameMode::MENU){
				game_mode = GameMode::PLAYING; }
				if (event->key.keysym.scancode == SDL_SCANCODE_ESCAPE) LockUnlockCursor();
				if (event->key.keysym.scancode == SDL_SCANCODE_F3) Crash();
				if (game_mode == GameMode::MENU) return;
				if (event->key.keysym.scancode == SDL_SCANCODE_M) player->NextMode();
				if (event->key.keysym.scancode == SDL_SCANCODE_SPACE) player->Jump(5.0f, map->floor_level); 
			}
		}
	}
	void ReadKeyboardInput(const uint8_t* keystate, float delta_time){
		if (game_mode == GameMode::MENU) return;
		//PLAYER MOVEMENT
		Mathematics::Vector3 movement = {0.0f, 0.0f};
		if (keystate[SDL_SCANCODE_W]) movement.y += 1.0f;
		if (keystate[SDL_SCANCODE_S]) movement.y -= 1.0f;
		if (keystate[SDL_SCANCODE_D]) movement.x += 1.0f;
		if (keystate[SDL_SCANCODE_A]) movement.x -= 1.0f;
		if (keystate[SDL_SCANCODE_E]) movement.z += 1.0f;
		if (keystate[SDL_SCANCODE_Q]) movement.z -= 1.0f;
		player->Go(movement, delta_time);
	}
	void ReadMouseInput(){
		if (game_mode == GameMode::MENU) return;
		int mouse_x, mouse_y;
		SDL_GetRelativeMouseState(&mouse_x, &mouse_y);
		player->Turn(-mouse_x, Game::sensitivity);
	}
};

int main(int argc, char* argv[])
{
	//SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) Game::Crash("SDL INIT ERROR");
    TTF_Init();
    TTF_Font* font = TTF_OpenFont("assets/fonts/Joy Circuit.otf", 32);
    if(!font) Game::Crash("FONT DIDN'T LOAD");
	SDL_Window* window = SDL_CreateWindow(Game::name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Game::screen_w, Game::screen_h, SDL_WINDOW_RESIZABLE);
	if(window == nullptr) Game::Crash("WINDOW WEREN'T CREATED");
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(renderer == nullptr) Game::Crash("RENDER WEREN'T CREATED");
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); //For transparency
	SDL_SetRelativeMouseMode((SDL_bool)Game::cursor_locked);	//Lock cursor
	SDL_Event event;
	const uint8_t* keystate;
	//GAME
	Mathematics::FillFastCosinusArray();
	Mathematics::FillFastSinusArray();
	Game::LoadSettings();
	Game::LoadIcon(window);
	//ENTITIES
	Map world_map("arena2.txt");
	Camera cam(Game::screen_w, Game::screen_h, {-5.0f, -40.0f, 20.0f}, Mathematics::pi/3.0f);
	Player player(4.0f, 20.0f, 10.0f, {25.0f, 25.0f, 10.0f}, Mathematics::pi/2.0f, 1.5f); //25 25
	Game::map = &world_map;
	Game::camera = &cam;
	Game::player = &player;
	//DELTA TIME
	uint64_t last_ticks_count = SDL_GetPerformanceCounter();
	uint64_t current_ticks_count = SDL_GetPerformanceCounter();	
	float delta_time = 0.0f;
	//MENU TEXT
	UI::TextBox game_name_text(renderer, Game::name, font, {255, 255, 0, 255});
	UI::TextBox game_version_text(renderer, Game::version, font, {0, 255, 100, 255});
	UI::TextBox press_enter_text(renderer, "press enter to continue", font, {0, 205, 255, 255});
	//FPS
    int fps_text_render_counter = 0;
	UI::TextBox fps_text(renderer, "hello world", font, {255, 0, 0, 255});
	//GAME LOOP
	while (Game::game_running) {
		//FPS CONTROL
		current_ticks_count = SDL_GetPerformanceCounter();
		delta_time = (float)(current_ticks_count - last_ticks_count) / (float)SDL_GetPerformanceFrequency();
		last_ticks_count = current_ticks_count;
		//PHYSICS 1
		player.Gravitate(Game::gravity, delta_time);
		player.AddFriction(world_map.floor_level, delta_time);
		//EVENTS
		Game::EventHandler(&event);
		Game::UpdateBlinkers(delta_time);
        //KEYBOARD
        keystate = SDL_GetKeyboardState(NULL);
        Game::ReadKeyboardInput(keystate, delta_time);
		if(Game::game_mode != Game::GameMode::MENU) cam.MoveTo(player.GetEyePosition());
		//MOUSE
		Game::ReadMouseInput();
		if(Game::game_mode != Game::GameMode::MENU) cam.TurnToAngle(player.angle_);
		//PHYSICS 2
		player.ApplySpeed(delta_time, world_map);
		//GRAPHICS
        cam.Render(renderer, world_map);
        //MENU
        if(Game::game_mode == Game::GameMode::MENU){
			SDL_SetRenderDrawColor(renderer, 0, 0, 20, 128);
			SDL_Rect menu_bg = {0, 0, Game::screen_w, Game::screen_h};
			SDL_RenderFillRect(renderer, &menu_bg);
			if(Game::blinker_1s.state){
				game_name_text.Draw(renderer, {(Game::screen_w - 600)/2, 0, 600, 50});
			}
			game_version_text.Draw(renderer, {Game::screen_w - 600, Game::screen_h - 100, 580, 80});
			press_enter_text.Draw(renderer, {0, 300, 800, 50});
		}
        //FPS TEXT
        if(fps_text_render_counter > 1000){
			int fps = 1.0f/delta_time;
			fps_text_render_counter = 0;
			char fps_string[64] = "fps = ";
			StringFunctions::AddIntToString_CH(fps_string, fps, 64);
			fps_text.Update(renderer, fps_string, font, {255, 0, 0, 255}, false);
		}else{
			fps_text_render_counter ++;
		}
		fps_text.Draw(renderer, { 0, 50, 200, 50 });
        SDL_RenderPresent(renderer);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

