#include <iostream>
#include <SDL.h>
#include <unordered_map>
#include <string>

void sdl_error_log(char* err) {
	std::cout << "Failed to " << err << ": " << SDL_GetError() << std::endl;
}
void sdl_error_exit(char* err) {
	sdl_error_log(err);
	SDL_Quit();
	exit(-1);
}


class ResourceManager {
	std::unordered_map<std::string, SDL_Texture*> textures;
	SDL_Texture* loadTexture(const std::string& fn) {
		SDL_Texture* tex = 0;
		SDL_Surface* img = SDL_LoadBMP(fn.c_str());
		if(img != 0) {
			tex = SDL_CreateTextureFromSurface(ren, img);
			SDL_FreeSurface(img);
			if(!tex) {
				sdl_error_log("convert surface");
			}
		}else {
			sdl_error_log("load img");
		}
		return tex;
	}
	SDL_Renderer* ren;
public:
	ResourceManager(SDL_Renderer* renderer) : ren(renderer) {}
	SDL_Texture* getTexture(const std::string& fn) {
		auto i = textures.find(fn);
		if(i != textures.end()) {
			return (*i).second;
		}
		SDL_Texture* tex = loadTexture(fn);
		if(tex != 0) {
			textures[fn] = tex;
		}
		return tex;
	}
	void Cleanup() {
		for(auto i = textures.begin(), end = textures.end(); i != end; ++i) {
			SDL_DestroyTexture(i->second);
		}
		textures.clear();
	}
};


int main(int argc, char* argv[]) {
	if( SDL_Init(SDL_INIT_EVERYTHING) != 0 ) {
		sdl_error_exit("init sdl");
	}
	SDL_Window* win = SDL_CreateWindow("Bulls and cows", 100, 100, 640, 580, SDL_WINDOW_SHOWN);
	if( !win ) {
		sdl_error_exit("open window");
	}
	SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if( !ren ) {
		SDL_DestroyWindow(win);
		sdl_error_exit("renderer");
	}

	ResourceManager res(ren);
	SDL_Texture* tex = res.getTexture("test.bmp");

	//Main loop stuff
	bool done = false;
	SDL_Event e;
	while( !done ) {
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT) 
				done = true;
			if(e.type == SDL_KEYDOWN)
				done = true;
			if(e.type == SDL_MOUSEBUTTONDOWN)
				done = true;
		}

		SDL_RenderClear(ren);
		SDL_RenderCopy(ren, tex, 0, 0);
		SDL_RenderPresent(ren);
	}

	res.Cleanup();
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}