#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "string_functions.h"
#include "ui.h"

namespace UI{
	TextBox::TextBox(SDL_Renderer* renderer, const char* text, TTF_Font* font, const SDL_Color& color){
		Update(renderer, text, font, color, true);
	}

	void TextBox::Update(SDL_Renderer* renderer, const char* text, TTF_Font* font, const SDL_Color& color, bool f){
		if(!f && StringFunctions::CompareStrings(text, last_text, max_text_len)) return;
		StringFunctions::WriteTextToString_CH(last_text, text, max_text_len);
		SDL_Surface* text_surface = TTF_RenderText_Solid(font, last_text, color);
		if(text_texture != nullptr){
			SDL_DestroyTexture(text_texture);
			text_texture = nullptr;
		}
		if(text_surface != nullptr) text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
		if(text_surface != nullptr) SDL_FreeSurface(text_surface);
	}

	void TextBox::Draw(SDL_Renderer* renderer, const SDL_Rect& rect){
		if(text_texture != nullptr) SDL_RenderCopy(renderer, text_texture, NULL, &rect);
	}
}
