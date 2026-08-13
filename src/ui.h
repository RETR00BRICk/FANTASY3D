#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

namespace UI{
	struct TextBox{
		static constexpr int max_text_len = 1024;
		char last_text[max_text_len] = {};
		SDL_Texture* text_texture = nullptr;
		//FUNCTIONS
		TextBox(SDL_Renderer* renderer, const char* text, TTF_Font* font, const SDL_Color& color);
		//RERENDER THE TEXTURE (SLOW)
		void Update(SDL_Renderer* renderer, const char* text, TTF_Font* font, const SDL_Color& color, bool f = false);
		//DRAW THE STORED TEXTURE (FAST)
		void Draw(SDL_Renderer* renderer, const SDL_Rect& rect);
	};
}
