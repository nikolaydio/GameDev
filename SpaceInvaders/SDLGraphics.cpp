#include "Graphics.h"
#include <SDL.h>
#include <vector>

class Texture {
public:
	int w, h;
	SDL_Texture* ptr;
};
class SDL_Graphics : public IGraphics {
	std::vector<Texture> textures;
	event_callback callback;
	int last_time;
	SDL_Window* window;
	SDL_Renderer* renderer;
public:
	SDL_Graphics() {
		if(SDL_Init( SDL_INIT_VIDEO ) < 0) {
			//printf("Failed to init SDL");
		}
		window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
		last_time = SDL_GetTicks();
	}
	void Clear() {
		SDL_RenderClear(renderer);
	}

	void HandleEvents() {
		SDL_Event e;
		while( SDL_PollEvent( &e ) != 0 )
		{ //User requests quit

		}
	}
	void SetCallback(event_callback clb) {
		callback = clb;
	}



	void RenderTexture(int tex, Vector2d pos)  {
		SDL_Rect rect;
		rect.x = pos.x;
		rect.y = pos.y;
		rect.w = textures[tex].w;
		rect.h = textures[tex].h;
		SDL_RenderCopy(renderer, textures[tex].ptr, 0, &rect);
	}
	
	int LoadTexture(char* filename) {
		SDL_Surface* surf = SDL_LoadBMP(filename);
		if(!surf)
			return -1;
		SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
		if(!tex) {
			SDL_FreeSurface(surf);
			return -1;
		}
		Texture t;
		t.ptr = tex;
		t.w = surf->w;
		t.h = surf->h;
		SDL_FreeSurface(surf);
		textures.push_back(t);
		return textures.size() - 1;
	}
	void Present() {
		SDL_RenderPresent(renderer);
	}
	virtual int NextDelta() {
		int now = SDL_GetTicks();
		int delta = now - last_time;
		last_time = now;
		return delta;
	}
};

IGraphics* CreateSDLGraphics() {
	return new SDL_Graphics;
}