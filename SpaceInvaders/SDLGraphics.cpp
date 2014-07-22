#include "Graphics.h"
#include <SDL.h>
#include <vector>
#include <SDL_mixer.h>

class Texture {
public:
	int w, h;
	SDL_Texture* ptr;
};
class SDL_Graphics : public IGraphics {
	std::vector<Texture> textures;
	std::vector<Mix_Music*> music;
	int last_time;
	SDL_Window* window;
	SDL_Renderer* renderer;
public:
	SDL_Graphics() {
		if(SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0) {
			//printf("Failed to init SDL");
		}
		window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		SDL_SetRenderDrawColor( renderer, 0x00, 0x0, 0x00, 0xFF );
		last_time = SDL_GetTicks();

		if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
		{
			printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
		}
	}
	~SDL_Graphics() {
		Mix_Quit();
		SDL_Quit();
	}
	void Clear() {
		SDL_RenderClear(renderer);
	}

	bool HandleEvent(GrEvent* ev) {
		SDL_Event e;
		if( SDL_PollEvent( &e ) != 0 )
		{ 
			if(e.type == SDL_QUIT) {
				ev->type = ET_EXIT;
				return true;
			}else {
				if(e.type == SDL_KEYDOWN) {
					ev->type = ET_KEY_DOWN;
				}else if(e.type == SDL_KEYUP) {
					ev->type = ET_KEY_UP;
				}
				switch(e.key.keysym.sym) {
				case SDLK_UP:
					ev->data = EK_UP; break;
				case SDLK_DOWN:
					ev->data = EK_DOWN; break;
				case SDLK_LEFT:
					ev->data = EK_LEFT; break;
				case SDLK_RIGHT:
					ev->data = EK_RIGHT; break;
				default:
					return false;
				}
				return true;
			}
		}
		return false;
	}
	int LoadSound(char* filename) {
		auto *m = Mix_LoadMUS(filename);
		if(!m) {
			return -1;
		}
		music.push_back(m);
		return music.size() - 1;
	}
	void PlaySoundFile(int sound) {
		Mix_PlayMusic(music[sound], 1);
	}


	void RenderTexture(int tex, Vector2d pos)  {
		SDL_Rect rect;
		rect.x = pos.x;
		rect.y = 480 - pos.y;
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