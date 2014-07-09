#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <time.h>

#include <unordered_map>
#include <string>


const static SDL_Color fontColor = { 0, 0, 0 };
const static int secret_number_digits = 4;

//Spacing between bulls/cows images
#define IMG_SPACING_PIXELS 10
#define IMG_SIZE 64
#define DIGIT_SPACING_PIXELS 40
//Yeah I'm aware I can do it with viewport. I prefer this way
#define GLOBAL_X_OFFSET 40


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


bool HasDifferentDigits(char* num) {
	for(int i = 0; i < secret_number_digits; ++i) {
		for(int j = i + 1; j < secret_number_digits; ++j) {
			if(num[i] == num[j])
				return false;
		}
	}
	return true;
}
void UpdateBullsAndCows(char* guess_number, char* secret_number, int* bulls, int* cows) {
	if(!HasDifferentDigits(guess_number)) {
		*bulls = secret_number_digits; *cows = secret_number_digits;
		return;
	}
	*bulls = 0; *cows = 0;
	for(int i = 0; i < secret_number_digits; ++i) {
		for(int j = 0; j < secret_number_digits; ++j) {
			if(guess_number[i] == secret_number[j]) {
				if(i == j) ++*bulls;
				else ++*cows;
			}
		}
	}
}
void GenerateSecretNumber(char* num) {
	do{
		for(int i = 0; i < secret_number_digits; ++i) {
			num[i] = rand() % 10;
		}
	}while(!HasDifferentDigits(num));
}


bool Init(SDL_Window** win, SDL_Renderer** ren) {
	*win = 0; *ren = 0;

	if( SDL_Init(SDL_INIT_EVERYTHING) != 0 ) {
		sdl_error_log("init sdl");
		return false;
	}
	*win = SDL_CreateWindow("Bulls and cows", 100, 100, 640, 580, SDL_WINDOW_SHOWN);
	if( !win ) {
		sdl_error_log("open window");
		return false;
	}
	*ren = SDL_CreateRenderer(*win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if( !ren ) {
		sdl_error_log("create renderer");
		return false;
	}
	if(TTF_Init() == -1) {
		sdl_error_log("init TTF");
	}
	return true;
}


Texture RenderText(SDL_Renderer* ren, char* text, TTF_Font* font) {
	SDL_Surface* surf = TTF_RenderText_Solid(font, text, fontColor);
	if(!surf) {
		return Texture();
	}
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
	if(!tex) {
		SDL_FreeSurface(surf);
		return Texture();
	}
	Texture d; d.ptr = tex; d.width = surf->w; d.height = surf->h;
	return d;
}

#define CLEANUP_AND_EXIT(cond) if(cond) { \
	res.Cleanup(); \
	SDL_DestroyRenderer(ren); \
	SDL_DestroyWindow(win); \
	SDL_Quit(); }

int main(int argc, char* argv[]) {
	srand (time(NULL));
	//------------Init SDL---------------------------
	//-----------------------------------------------
	SDL_Window* win;
	SDL_Renderer* ren;
	
	if(!Init(&win, &ren)) {
		if(ren) {
			SDL_DestroyRenderer(ren);
		}
		if(win) {
			SDL_DestroyWindow(win);
		}
		return -1;
	}


	//------------    Init Resources    --------------
	//-------------------------------------------------
	ResourceManager res(ren);
	Texture bull = res.getTexture("bull.bmp");
	Texture cow = res.getTexture("cow.bmp");
	Texture hint;
	TTF_Font* font72 = TTF_OpenFont("asta.ttf", 72);
	TTF_Font* font28 = TTF_OpenFont("asta.ttf", 28);

	CLEANUP_AND_EXIT(res.HasFailed() || !font72 || !font28);

	Texture digit_textures[10];
	for(int i = 0; i < 10; ++i) {
		char digit_buffer[2];
		_itoa_s(i, digit_buffer, 10);
		Texture d = RenderText(ren, digit_buffer, font72);
		CLEANUP_AND_EXIT(!d.ptr);

		digit_textures[i] = d;
		//this will release the texture at the end of program
		res.addTexture(digit_buffer, d);
	}

	//Initialize the description text
	{
		char* desc_text = "Use arrows to change the numbers and \"C\" to check your answer.";
		hint = RenderText(ren, desc_text, font28);
		CLEANUP_AND_EXIT(!hint.ptr);
		res.addTexture("InfoText", hint);
	}

	TTF_CloseFont(font72);
	TTF_CloseFont(font28);

	//------------Main Loop-------------------------
	//----------------------------------------------
	bool done = false;
	SDL_Event e;
	int base = (int)pow(10, secret_number_digits-1);

	char secret_number[secret_number_digits];
	char guess_number[secret_number_digits];

	//Fill in the numbers from above
	GenerateSecretNumber(secret_number);
	for(int i = 0; i < secret_number_digits; ++i) {
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
			if(e.type == SDL_KEYDOWN) {
				switch( e.key.keysym.sym ) {
				case SDLK_UP:
					guess_number[selected] = std::min(guess_number[selected] + 1, 9); break;
				case SDLK_DOWN:
					guess_number[selected] = std::max(guess_number[selected] - 1, 0); break;
				case SDLK_LEFT:
					selected = std::max(selected - 1, 0); break;
				case SDLK_RIGHT:
					selected = std::min(selected + 1, secret_number_digits-1); break;
				case SDLK_c:
					UpdateBullsAndCows(guess_number, secret_number, &bull_count, &cow_count);
				}
			}
		}
		

		//--------------RENDERING-------------
		SDL_Rect rect;
		SDL_SetRenderDrawColor( ren, 0xFF, 0xFF, 0xFF, 0xFF );
		SDL_RenderClear(ren);

		//Render the images(bulls + cows)
		RenderImageRow(ren, bull.ptr, bull_count, IMG_SIZE, GLOBAL_X_OFFSET, 200);
		RenderImageRow(ren, cow.ptr, cow_count, IMG_SIZE, GLOBAL_X_OFFSET, 300);


		int digit_position_offset = GLOBAL_X_OFFSET + secret_number_digits * (IMG_SIZE + IMG_SPACING_PIXELS);

		//Render the red background behind the selected digit
		rect.x = digit_position_offset + DIGIT_SPACING_PIXELS * selected;
		rect.y = 250;
		rect.w = digit_textures[guess_number[selected]].width;
		rect.h = digit_textures[guess_number[selected]].height;
		SDL_SetRenderDrawColor( ren, 0xFF, 0x00, 0x00, 0xFF );
		SDL_RenderFillRect( ren, &rect );

		//Render the digits
		for(int i = 0; i < secret_number_digits; ++i) {
			int digit = guess_number[i];
			rect.x = digit_position_offset + DIGIT_SPACING_PIXELS * i;
			rect.y = 250;
			rect.w = digit_textures[digit].width;
			rect.h = digit_textures[digit].height;
			SDL_RenderCopy(ren, digit_textures[digit].ptr, 0, &rect);
		}

		//Draw description text
		rect.x = 40;
		rect.y = 400;
		rect.w = hint.width;
		rect.h = hint.height;
		SDL_RenderCopy(ren, hint.ptr, 0, &rect);
		SDL_RenderPresent(ren);
	}


	CLEANUP_AND_EXIT(true);
	return 0;
}