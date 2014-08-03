
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


void LoadFromFile(ResourceManager& res, World& world, const char* fn) {
	//Texture tex = res.GetTexture("test.bmp");
	//world.AddEntity(new Platform(Vector2d(125,225), Vector2d(120, 65), tex));
}





class Game {

	ResourceManager res_manager;
	
	SDL_Renderer* renderer;
	SDL_Window* window;
	bool running;
	
	Actor* p1;

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
		id = allocator.AllocID();
		sprite.pos = Vector2d(250, 300);
		sprite.size = Vector2d(500, 90);
		sprite.source.x = 0;
		sprite.source.y = 0;
		sprite.source.w = 0;
		sprite.source.h = 0;
		scene.AddSprite(id, sprite);

		actor.shape.pos = Vector2d(250, 300);
		actor.shape.size = Vector2d(500, 90);
		actor.inv_mass = 0;
		actor.restitution = 0.1;
		physics.AddActor(id, actor);


		//------Add the player
		id = allocator.AllocID();
		sprite.texture = res_manager.GetTexture("character.png");
		sprite.pos = Vector2d(150, 150);
		sprite.size = Vector2d(46 * 2, 55 * 2);
		//sprite.source.x = 0;
		//sprite.source.y = 3 * 51;
		//sprite.source.w = 46;
		//sprite.source.h = 55;
		scene.AddSprite(id, sprite);

		actor.shape.pos = Vector2d(150, 150);
		actor.shape.size = Vector2d(46 * 2, 55 * 2);
		actor.velo = Vector2d(0, 0);
		actor.inv_mass = 1 / 3.0;
		actor.restitution = 0.1;

		physics.AddActor(id, actor);
		p1 = &physics.GetActor(id);

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
			p1->velo += Vector2d(0, 9.8) * delta;
			if(state[SDL_SCANCODE_LEFT]) {
				p1->velo.x -= 35 * delta;
			}
			if(state[SDL_SCANCODE_RIGHT]) {
				p1->velo.x += 35 * delta;
			}

			physics.UpdatePositions(delta);
			physics.CollideAndRespond();

			//get info from physics and feed to the scene
			for(auto i = physics.GetMoves().begin(), end = physics.GetMoves().end();
				i != end; ++i) {
					scene.SetPosition(i->id, i->pos);
			}
			physics.GetMoves().clear();


			SDL_RenderClear(renderer);
			scene.Render(renderer);

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