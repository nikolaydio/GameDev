
#include <vector>
#include "Graphics.h"
#include "World.h"
#include "SparseArray.h"

class Platform : public Entity {
	Vector2d pos, size;
	Texture tex;
public:
	Platform(const Vector2d _pos, const Vector2d _size, Texture& t) {
		pos = _pos; size = _size;
		tex = t;
		mask = 1;
	}
	void Render(SDL_Renderer* renderer) {
		SDL_Rect rect;
		rect.x = pos.x - size.x / 2;
		rect.y = pos.y - size.y / 2;
		rect.w = size.x;
		rect.h = size.y;
		SDL_RenderCopy(renderer, tex.ptr, 0, &rect);
	}
	Vector2d VsRect(Vector2d a_pos, Vector2d a_size) {
		Vector2d n = pos - a_pos;

		float a_extent = a_size.x / 2;
		float b_extent = size.x / 2;

		float x_overlap = a_extent + b_extent - abs( n.x );
		if(x_overlap > 0) {
			a_extent = a_size.y / 2;
			b_extent = size.y / 2;

			float y_overlap = a_extent + b_extent - abs( n.y );
			if(y_overlap > 0) {
				return Vector2d(x_overlap, y_overlap);
			}
		}
		return Vector2d(0,0);
	}
};
class Player : public Entity {
public:

	Vector2d pos;
	Vector2d velo;

	Vector2d size;
	Texture tex;
	
	//These are inputs. They are checked every frame
	bool running_left;
	bool running_right;
	bool jumping;

	//these are state flags
	bool on_ground;

	Player() {
		running_left = false;
		running_right = false;
		mask = 0;
	}

	virtual void Update(const World& world, float delta) {
		Vector2d acc;
		if(running_left) {
			acc.x -= 10;
		}
		if(running_right) {
			acc.x += 10;
		}
		if(on_ground && jumping) {
			on_ground = false;
			acc.y += 20;
		}
		acc.y += 2;

		acc = acc * 3;


		velo += acc * delta;
		if(on_ground) {
			velo.x *= 0.99;
		}

		pos += velo * delta;

		Vector2d overlap = world.VsRect(pos, size, 1);
		if(overlap.x > 0) {
			if(velo.x > 0) {
				pos.x += overlap.x;
			}else if(velo.x < 0) {
				pos.x -= overlap.x;
			}
			velo.x = 0;
		}
		if(overlap.y > 0) {
			if(velo.y > 0) {
				pos.y -= overlap.y;
			}else if(velo.y < 0) {
				pos.y += overlap.y;
			}
			velo.y = 0;
		}
	}
 
	virtual void Render(SDL_Renderer* renderer) {
		SDL_Rect rect;
		rect.x = pos.x - size.x / 2;
		rect.y = pos.y - size.y / 2;
		rect.w = size.x;
		rect.h = size.y;
		SDL_RenderCopy(renderer, tex.ptr, 0, &rect);
	}
};



void LoadFromFile(ResourceManager& res, World& world, const char* fn) {
	Texture tex = res.getTexture("test.bmp");
	world.AddEntity(new Platform(Vector2d(125,225), Vector2d(120, 65), tex));
}
class Game {
	World world;
	ResourceManager res_manager;
	SDL_Renderer* renderer;
	SDL_Window* window;
	bool running;
	Player* player;
public:
	bool Init() {
		SDL_Init(SDL_INIT_EVERYTHING);
		if(SDL_CreateWindowAndRenderer(640, 480, SDL_RENDERER_PRESENTVSYNC, &window, &renderer) != 0) {
			return false;
		}
		res_manager.Init(renderer);
		LoadFromFile(res_manager, world, "");
		player = new Player;
		player->pos = Vector2d(100, 100);
		player->size = Vector2d(40, 80);
		player->tex = res_manager.getTexture("test.bmp");
		world.AddEntity(player);
		return true;
	}
	void Run() {
		running = true;
		SDL_Event e;
		int last_frame = SDL_GetTicks();
		while(running) {

			int now = SDL_GetTicks();
			float delta = (now - last_frame) / 1000.0f;
			last_frame = now;

			while( SDL_PollEvent( &e ) != 0 )
			{ 
				if(e.type == SDL_QUIT) {
					running = false;
				}else if(e.type == SDL_KEYDOWN) {
					if(e.key.keysym.sym == SDLK_LEFT) {
						player->running_left = true;
					}else if(e.key.keysym.sym == SDLK_RIGHT) {
						player->running_right = true;
					}
				}else if(e.type == SDL_KEYUP) {
					if(e.key.keysym.sym == SDLK_LEFT) {
						player->running_left = false;
					}else if(e.key.keysym.sym == SDLK_RIGHT) {
						player->running_right = false;
					}
				}
			}
			world.Update(delta);
			SDL_RenderClear(renderer);
			world.Render(renderer);
			SDL_RenderPresent(renderer);
		}
	}
	void Done() {
		res_manager.Cleanup();
		if(renderer)
			SDL_DestroyRenderer(renderer);
		if(window)
			SDL_DestroyWindow(window);
		SDL_Quit();
	}
};
#define MAX_ENTITES 1024
int main(int argc, char* argv[]) {
	SparseArray<int> test;
	IdAllocator allocator(MAX_ENTITES);
	test.SetCapacity(MAX_ENTITES);
	

	Game game;
	if(game.Init())
		game.Run();
	game.Done();
	return 0;
}