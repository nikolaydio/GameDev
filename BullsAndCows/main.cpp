#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>

#include <unordered_map>
#include <string>


const static SDL_Color fontColor = { 0, 0, 0 };
const static int secret_number_digits = 4;
#define IMG_SPACING_PIXELS 10
#define RAND_RANGE(min, max) ((min) + rand() % ((max) - (min)))

void sdl_error_log(char* err) {
	std::cout << "Failed to " << err << ": " << SDL_GetError() << std::endl;
}
void sdl_error_exit(char* err) {
	sdl_error_log(err);
	SDL_Quit();
	exit(-1);
}
void std_error_exit(char* err) {
	std::cout << "Failed to " << err << std::endl;
	SDL_Quit();
	exit(-1);
}

class Texture {
public:
	SDL_Texture* ptr;
	int width;
	int height;
	Texture() { ptr = 0; width = 0; height = 0; }
	Texture& operator=(const Texture& sec) {
		ptr = sec.ptr;
		width = sec.width;
		height = sec.height;
		return *this;
	}
};
class ResourceManager {
	Texture loadTexture(const std::string& fn) {
		Texture tex;
		SDL_Surface* img = SDL_LoadBMP(fn.c_str());
		if(img != 0) {

			tex.ptr = SDL_CreateTextureFromSurface(ren, img);
			tex.width = img->w;
			tex.height = img->h;
			SDL_FreeSurface(img);
			if(!tex.ptr) {
				sdl_error_log("convert surface");
			}
		}else {
			sdl_error_log("load img");
		}
		return tex;
	}
	SDL_Renderer* ren;
	int fails;
	std::unordered_map<std::string, Texture> textures;
public:
	ResourceManager(SDL_Renderer* renderer) : ren(renderer), fails(0) {}
	Texture getTexture(const std::string& fn) {
		auto i = textures.find(fn);
		if(i != textures.end()) {
			return (*i).second;
		}
		Texture tex = loadTexture(fn);
		if(tex.ptr != 0) {
			textures[fn] = tex;
		}
		if(tex.ptr == 0)
			++fails;
		return tex;
	}
	int HasFailed() {
		int fail_count = fails;
		fails = 0;
		return fail_count;
	}
	void addTexture(const std::string key, Texture tex) {
		textures[key] = tex;
	}
	void Cleanup() {
		for(auto i = textures.begin(), end = textures.end(); i != end; ++i) {
			SDL_DestroyTexture(i->second.ptr);
		}
		textures.clear();
	}
};

void RenderImageRow(SDL_Renderer* ren, SDL_Texture* tex, int count, int size, int x, int y) {
	SDL_Rect rect;
	for(int i = 0; i < count; ++i) {
		rect.x = i * (size + IMG_SPACING_PIXELS) + x;
		rect.y = y;
		rect.w = size;
		rect.h = size;
		SDL_RenderCopy(ren, tex, 0, &rect);
	}
}


int main(int argc, char* argv[]) {
	//------------Init SDL---------------------------
	//-----------------------------------------------
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

	if(TTF_Init() == -1) {
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		std_error_exit("init TTF.");
	}


	//------------    Init Resources    --------------
	//-------------------------------------------------
	ResourceManager res(ren);
	Texture bull = res.getTexture("bull.bmp");
	Texture cow = res.getTexture("cow.bmp");
	
	TTF_Font* font = TTF_OpenFont("asta.ttf", 72);
	

	if(res.HasFailed() || !font) {
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		std_error_exit("load some resources. Shutting down.");
	}

	Texture digit_textures[10];
	char digit_buffer[2];
	for(int i = 0; i < 10; ++i) {
		_itoa_s(i, digit_buffer, 10);
		SDL_Surface* digit_surface = TTF_RenderText_Solid(font, digit_buffer, fontColor);
		if(!digit_surface) {
			SDL_DestroyRenderer(ren);
			SDL_DestroyWindow(win);
			std_error_exit("draw a number.");
		}
		SDL_Texture* digit_tex = SDL_CreateTextureFromSurface(ren, digit_surface);
		if(!digit_tex) {
			SDL_DestroyRenderer(ren);
			SDL_DestroyWindow(win);
			std_error_exit("make a tex of a number.");
		}
		Texture d; d.ptr = digit_tex; d.width = digit_surface->w;
		d.height = digit_surface->h;
		SDL_FreeSurface(digit_surface);
		
		digit_textures[i] = d;
		//this will release the texture at the end of program
		res.addTexture(digit_buffer, d);
	}
	

	//------------Main Loop-------------------------
	//----------------------------------------------
	bool done = false;
	SDL_Event e;
	int base = (int)pow(10, secret_number_digits-1);

	char secret_number[secret_number_digits];
	char guess_number[secret_number_digits];

	for(int i = 0; i < secret_number_digits; ++i) {
		secret_number[i] = rand() % 10;
		guess_number[i] = i+1;
	}

	int bull_count = secret_number_digits;
	int cow_count = secret_number_digits;
	int selected = 0;
	while( !done ) {
		//Poll events
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT) 
				done = true;
			if(e.type == SDL_KEYDOWN)
				done = true;
			if(e.type == SDL_MOUSEBUTTONDOWN)
				done = true;
		}
		

		//Render the scene
		SDL_Rect rect; int size = 64;
		SDL_SetRenderDrawColor( ren, 0xFF, 0xFF, 0xFF, 0xFF );
		SDL_RenderClear(ren);
		RenderImageRow(ren, bull.ptr, bull_count, 64, 40, 200);
		RenderImageRow(ren, cow.ptr, cow_count, 64, 40, 300);

		rect.x = 40 + secret_number_digits * (size + IMG_SPACING_PIXELS) + 50 * selected;
		rect.y = 250;
		rect.w = digit_textures[0].width;
		rect.h = digit_textures[0].height;
		SDL_SetRenderDrawColor( ren, 0xFF, 0x00, 0x00, 0xFF );
		SDL_RenderFillRect( ren, &rect );
		for(int i = 0; i < secret_number_digits; ++i) {
			int digit = guess_number[i];
			rect.x = 40 + secret_number_digits * (size + IMG_SPACING_PIXELS) + 50 * i;
			rect.y = 250;
			rect.w = digit_textures[digit].width;
			rect.h = digit_textures[digit].height;
			SDL_RenderCopy(ren, digit_textures[digit].ptr, 0, &rect);
		}
		SDL_RenderPresent(ren);
	}


	//------------Cleanup---------------
	//----------------------------------
	res.Cleanup();
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}