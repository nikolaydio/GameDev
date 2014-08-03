
#include <vector>
#include "Graphics.h"
#include "World.h"
#include "SparseArray.h"
#include "Physics.h"


#define ENABLE_VSYNC
#ifdef ENABLE_VSYNC
#define CUSTOM_PRESENT_VSYNC SDL_RENDERER_PRESENTVSYNC
#else
#define CUSTOM_PRESENT_VSYNC 0
#endif

#define MAX_ENTITES 1024



class Tiles {

};


class Character {
	enum STATE {
		STAYING,
		RUNNING,
		JUMPING
	};
	STATE state;

	float horizontal;

	float anim_progress;
	float anim_max;
	float anim_speed;
	float anim_start;
public:
	Actor* actor;
	Sprite* sprite;
	Character() { state = STAYING; horizontal = 0;
		anim_speed = 10; anim_max = 9; anim_progress = 0;
		anim_start = 1;
	}
	void Move(float move) {
		horizontal += move;
	}
	void Jump() {
	if(state == RUNNING || state == STAYING) {
			actor->velo.y -= 600;
			state = JUMPING;
		}
	}
	void Collided(Vector2d normal) {
		if(normal.x == 0 || normal.y == 1) {
			if(state == JUMPING) {
				state = STAYING;
			}
		}
	}
	void Update(float delta) {
		actor->velo.x += horizontal * delta;
		if(state == RUNNING) {
			if(horizontal == 0) {
				if(actor->velo.x > 0) {
					actor->velo.x -= std::min(actor->velo.x, 175 * delta);
				}else{
					actor->velo.x += std::min(-actor->velo.x, 175 * delta);
				}
			}
			if(actor->velo.x > 0) {
				actor->velo.x = std::min<float>(actor->velo.x, 400);
				sprite->size.x = abs(sprite->size.x);
			}else{
				actor->velo.x = -std::min<float>(-actor->velo.x, 400);
				sprite->size.x = -abs(sprite->size.x);
			}

			anim_speed = abs(actor->velo.x) / 32;
			if(abs(actor->velo.x) < 1) {
				state = STAYING;
				sprite->source.x = 0;
				anim_progress = 0;
			}else{
				//do anim
				sprite->source.x = (int)anim_progress * 50;
				anim_progress += delta * anim_speed;
				if(anim_progress > anim_max) {
					anim_progress = anim_start + anim_progress - anim_max;
				}
			}
		}else if(state == STAYING) {
			if(abs(actor->velo.x) > 1) {
				state = RUNNING;
			}
		}

		printf("%f\n", anim_progress);

		horizontal = 0;
		
	}
};




class Game {

	ResourceManager res_manager;
	
	SDL_Renderer* renderer;
	SDL_Window* window;
	bool running;
	
	Actor* p1;
	Character player;
	ARRAY_ID player_id;
	Vector2d cam;

	SpriteScene scene;
	PhysicsWorld physics;

	IdAllocator allocator;
	Tiles tiles;
public:
	Game() : allocator(MAX_ENTITES) {

	}
	bool Init() {
		SDL_Init(SDL_INIT_EVERYTHING);
		window = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
		if(!window) {
			return false;
		}
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | CUSTOM_PRESENT_VSYNC);
		if(!renderer) {
			return false;
		}
		SDL_SetRenderDrawColor(renderer, 255,255,255,255);
		res_manager.Init(renderer, "texture/");

		scene.Init(MAX_ENTITES);
		physics.Init(MAX_ENTITES);
		physics.SetConstantForce(Vector2d(0, 9.8));
		
		Sprite sprite; Actor actor; ARRAY_ID id;
		sprite.texture = res_manager.GetTexture("grass_platform.png");

		//------Add a platform
		for(int i = 0; i < 10; ++i) {
			id = allocator.AllocID();
			sprite.pos = Vector2d(250 + 500 * i, 300 - 160 * i);
			sprite.size = Vector2d(500, 100);
			sprite.source.x = 0;
			sprite.source.y = 0;
			sprite.source.w = 0;
			sprite.source.h = 0;
			scene.AddSprite(id, sprite);

			actor.shape.pos = Vector2d(250 + 500 * i, 300 - 160 * i);
			actor.shape.size = Vector2d(500, 100);
			actor.inv_mass = 0;
			actor.restitution = 0.1;
			physics.AddActor(id, actor);
		}


		//------Add the player
		id = allocator.AllocID();
		sprite.texture = res_manager.GetTexture("character_map.png");
		sprite.pos = Vector2d(150, 150);
		sprite.size = Vector2d(36 * 2, 55 * 2);
		sprite.source.x = 0;
		sprite.source.y = 0;
		sprite.source.w = 50;
		sprite.source.h = 55;
		scene.AddSprite(id, sprite);

		actor.shape.pos = Vector2d(150, 150);
		actor.shape.size = Vector2d(36 * 2, 55 * 2);
		actor.velo = Vector2d(0, 0);
		actor.inv_mass = 1 / 3.0;
		actor.restitution = 0.1;

		physics.AddActor(id, actor);
		p1 = &physics.GetActor(id);
		player.actor = p1;
		player_id = id;
		player.sprite = &scene.GetSpirte(id);

		actor.shape.pos = Vector2d(100, 200);


		return true;
	}
	void Run() {
		running = true;
		SDL_Event e;
		int last_frame = SDL_GetTicks();
		
		//For FPS calc
		int frame_count = 0;
		int last_display = last_frame;
		const int fps_update = 30;

		while(running) {
			frame_count++;

			int now = SDL_GetTicks();
			float delta = (now - last_frame) / 1000.0f;
			last_frame = now;
			while( SDL_PollEvent( &e ) != 0 )
			{ 
				if(e.type == SDL_QUIT) {
					running = false;
				}

			}
			const Uint8* state = SDL_GetKeyboardState(0);
			p1->velo += Vector2d(0, 9.8 * 100) * delta;
			if(state[SDL_SCANCODE_LEFT]) {
				player.Move(-400);
			}
			if(state[SDL_SCANCODE_RIGHT]) {
				player.Move(400);
			}
			if(state[SDL_SCANCODE_UP]) {
				player.Jump();
			}
			player.Update(delta);

			physics.UpdatePositions(delta);
			physics.CollideAndRespond();

			cam.x = p1->shape.pos.x;
			cam.y = std::min(p1->shape.pos.y, 100.0f);

			//get info from physics and feed to the scene
			for(auto i = physics.GetMoves().begin(), end = physics.GetMoves().end();
				i != end; ++i) {
					scene.SetPosition(i->id, i->pos);
			}
			physics.GetMoves().clear();
			for(auto i = physics.GetCollisions().begin(), end = physics.GetCollisions().end();
				i != end; ++i) {
					if(i->first == player_id) {
						player.Collided(i->normal);
					}else if(i->second == player_id) {
						player.Collided(-i->normal);
					}
			}
			physics.GetCollisions().clear();


			SDL_RenderClear(renderer);
			scene.Render(renderer, Vector2d(cam.x - 320, cam.y - 240));

			if(frame_count % fps_update == 0) {
				int diff = SDL_GetTicks() - last_display;
				last_display = SDL_GetTicks();
				float fps = diff * (60 * 60 / fps_update) / 1000.0;
				//printf("%f\n", fps);
			}
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