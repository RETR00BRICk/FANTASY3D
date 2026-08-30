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

enum class GameMode{
		MENU,
		PLAYING,
		PAUSED
};

constexpr char VERSION[] = "V0.05 REFACTORING + FPS LIMITER";
constexpr char NAME[] = "FANTASY3D";
GameMode game_mode = GameMode::MENU;
bool game_running = true;

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
	
class FPSManager{
public:
	float Tick(float target_dt){
		uint64_t freq = SDL_GetPerformanceFrequency();
		uint64_t frame_start_ticks = SDL_GetPerformanceCounter();
		float game_dt = (float)(frame_start_ticks - delay_end_ticks) / (float)freq; //TIME THAT IT TAKES FOR PHYSICS, RENDERING, READING INPUT ETC.
		int dt_diff = target_dt - game_dt*1000.0f; //target_dt IS MEASURED IN MS, game_dt IN SECONDS
		if(dt_diff > 0){ //IF GAMES RUN TOO FAST, SLOWS IT DOWN
			SDL_Delay(dt_diff);
		}
		delay_end_ticks = SDL_GetPerformanceCounter();
		float all_frame_dt = (frame_start_ticks - last_frame_ticks) / (float)freq; //TIME THAT IS BETWEEN TWO FRAMES
		last_frame_ticks = frame_start_ticks;
		if (all_frame_dt > 0.1f) all_frame_dt = 0.1f; //100 MS DT LIMIT
		return all_frame_dt;
	}
private:
	uint64_t last_frame_ticks = SDL_GetPerformanceCounter();
	uint64_t delay_end_ticks = SDL_GetPerformanceCounter();
};
	
struct Settings{
	int win_w = 800;
	int win_h = 600;
	bool cursor_locked = true;
	float sensitivity = 0.001f;
	float target_dt = 16.6667f;
	
	Settings(){
		SDL_RWops* file = SDL_RWFromFile("settings/global.txt", "rb");
		if (!file) return;
		int file_size = SDL_RWsize(file);
		if(file_size <= 0) return;
		char char_array[file_size]; //If it doesn't fit on the stack, it is the user fault, not mine!
		SDL_RWread(file, char_array, 1, file_size);
		SDL_RWclose(file);
		int settings[5];
		int cursor = 0;
		if(StringFunctions::TryLoadIntArrayFromString_CH(settings, 5, cursor, char_array, file_size)){
			win_w = settings[0];
			win_h = settings[1];
			sensitivity = (float)settings[2]/10000.0f;
			cursor_locked = (bool)settings[3];
			target_dt = 1000.0f/settings[4];
		}
	}
	void ToggleCursor(){
		cursor_locked = !cursor_locked;
		SDL_SetRelativeMouseMode((SDL_bool)cursor_locked);
	}
};

struct Entities{
	Map map{"arena2.txt"}; //for some reason compiler doesn't like Map map(...);
	Camera cam;
	Player player;
	Blinkers::Square blinker_1s{0.0f, 1.0f};
	
	Entities(Settings& settings):
	cam(settings.win_w, settings.win_h, {-5.0f, -40.0f, 20.0f}, Mathematics::pi/3.0f),
	player(4.0f, 20.0f, 10.0f, {25.0f, 25.0f, 10.0f}, Mathematics::pi/2.0f, 1.5f)
	{}
	void UpdateBlinkers(float dt){
		blinker_1s.Update(dt);
	}
};

struct SDL{
	TTF_Font* font = nullptr;
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL(Settings& settings){
		if(SDL_Init(SDL_INIT_VIDEO) < 0) Crash("SDL INIT ERROR");
		SDL_StopTextInput(); //WITHOUT THIS FPS IS UNSTABLE I DONT KNOW WHY
		TTF_Init();
		font = TTF_OpenFont("assets/fonts/Joy Circuit.otf", 32);
		if(font == nullptr) Crash("FONT DIDN'T LOAD");
		window = SDL_CreateWindow(NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, settings.win_w, settings.win_h, SDL_WINDOW_RESIZABLE);
		if(window == nullptr) Crash("WINDOW WEREN'T CREATED");
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if(renderer == nullptr) Crash("RENDER WEREN'T CREATED");
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); //For transparency
	}

	void Shutdown(){
		TTF_CloseFont(font);
		TTF_Quit();
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
};
	
struct UI{
	//MAIN MENU:
	UIScreen::TextBox game_name_text;
	UIScreen::TextBox game_version_text;
	UIScreen::TextBox press_enter_text;
	//GAME:
	int fps_text_render_counter = 0;
	UIScreen::TextBox fps_text;
	UI(SDL& sdl):
	game_name_text(sdl.renderer, NAME, sdl.font, {255, 255, 0, 255}),
	game_version_text(sdl.renderer, VERSION, sdl.font, {0, 255, 100, 255}),
	press_enter_text(sdl.renderer, "press enter to continue", sdl.font, {0, 205, 255, 255}),
	fps_text(sdl.renderer, "hello world", sdl.font, {255, 0, 0, 255})
	{}
};

void InitMath(){
	Mathematics::FillFastCosinusArray();
	Mathematics::FillFastSinusArray();
}	

void HandleEvent(Entities& entities, Settings& settings){
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			game_running = false;
		}
		if (event.type == SDL_WINDOWEVENT){
			if (event.window.event == SDL_WINDOWEVENT_RESIZED){
				int w = event.window.data1;
				int h = event.window.data2;
				settings.win_h = h;
				settings.win_w = w;
				entities.cam.SetOutputImageSize(w,h);
			}
		}
		if (event.type == SDL_KEYDOWN){
			if (event.key.keysym.scancode == SDL_SCANCODE_RETURN && game_mode == GameMode::MENU){
				game_mode = GameMode::PLAYING; 
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) settings.ToggleCursor();
			if (event.key.keysym.scancode == SDL_SCANCODE_F3) Crash();
			if (game_mode == GameMode::PLAYING){
				if (event.key.keysym.scancode == SDL_SCANCODE_M) entities.player.NextMode();
				if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) entities.player.Jump(5.0f, entities.map.floor_level); 
			}
		}
	}
}
void ReadKeyboardInput(Entities& entities, float delta_time){
	const Uint8* keystate = SDL_GetKeyboardState(NULL);
	if (game_mode == GameMode::MENU) return;
	//PLAYER MOVEMENT
	Mathematics::Vector3 movement = {0.0f, 0.0f};
	if (keystate[SDL_SCANCODE_W]) movement.y += 1.0f;
	if (keystate[SDL_SCANCODE_S]) movement.y -= 1.0f;
	if (keystate[SDL_SCANCODE_D]) movement.x += 1.0f;
	if (keystate[SDL_SCANCODE_A]) movement.x -= 1.0f;
	if (keystate[SDL_SCANCODE_E]) movement.z += 1.0f;
	if (keystate[SDL_SCANCODE_Q]) movement.z -= 1.0f;
	entities.player.Go(movement, delta_time);
}
void ReadMouseInput(Entities& entities, Settings& settings){
	if (game_mode == GameMode::MENU) return;
	int mouse_x, mouse_y;
	SDL_GetRelativeMouseState(&mouse_x, &mouse_y);
	entities.player.Turn(-mouse_x, settings.sensitivity);
}
void LoadIcon(SDL& sdl){
	SDL_Surface* icon_surface = SDL_LoadBMP("assets/textures/ICON.bmp");
	if (icon_surface) {
		Uint32 color_key = SDL_MapRGB(icon_surface->format, 255, 0, 255);
		SDL_SetColorKey(icon_surface, SDL_TRUE, color_key);
		SDL_SetWindowIcon(sdl.window, icon_surface);
		SDL_FreeSurface(icon_surface);
	}
}

int main(int argc, char* argv[])
{
	Settings settings;
	SDL sdl(settings);
	Entities entities(settings);
	UI ui(sdl);
	FPSManager fps_manager;
	InitMath();
	LoadIcon(sdl);
	//GAME LOOP
	while(game_running){
		float delta_time = fps_manager.Tick(settings.target_dt);	
		if(game_mode == GameMode::PLAYING){
			//PHYSICS 1
			entities.player.Gravitate(delta_time);
			entities.player.AddFriction(entities.map.floor_level, delta_time);
		}
		//EVENTS ADN INPUT
		HandleEvent(entities, settings);
		ReadKeyboardInput(entities, delta_time);
		ReadMouseInput(entities, settings);		
		if(game_mode == GameMode::PLAYING){
			//PHYSICS 2
			entities.player.ApplySpeed(delta_time, entities.map);
			//OBJECTS
			if(game_mode != GameMode::MENU) entities.cam.MoveTo(entities.player.GetEyePosition());		
			if(game_mode != GameMode::MENU) entities.cam.TurnToAngle(entities.player.angle_);
		}
		entities.cam.Render(sdl.renderer, entities.map);
		//UI
		entities.UpdateBlinkers(delta_time);
		if(game_mode == GameMode::MENU){
			SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 20, 128);
			SDL_Rect menu_bg = {0, 0, settings.win_w, settings.win_h};
			SDL_RenderFillRect(sdl.renderer, &menu_bg);
			if(entities.blinker_1s.state){
				ui.game_name_text.Draw(sdl.renderer, {(settings.win_w - 600)/2, 0, 600, 50});
			}
			ui.game_version_text.Draw(sdl.renderer, {settings.win_w - 600, settings.win_h - 100, 580, 80});
			ui.press_enter_text.Draw(sdl.renderer, {0, 300, 800, 50});
		}
		if(ui.fps_text_render_counter > 15){
			int fps = 1.0f/delta_time;
			ui.fps_text_render_counter = 0;
			char fps_string[64] = "fps = ";
			StringFunctions::AddIntToString_CH(fps_string, fps, 64);
			ui.fps_text.Update(sdl.renderer, fps_string, sdl.font, {255, 0, 0, 255}, false);
		}else{
			ui.fps_text_render_counter ++;
		}
		ui.fps_text.Draw(sdl.renderer, { 0, 50, 200, 50 });
		SDL_RenderPresent(sdl.renderer);
	}
	sdl.Shutdown();
	return 0;
}
