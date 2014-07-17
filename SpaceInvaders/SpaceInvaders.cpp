#include <vector>
#include "Graphics.h"

#include <iostream>



struct Type {
	int fire_chance;
	int bounty;
	int tex;
};


struct Invader {
	Invader(Type& t) : type(t) {}
	Vector2d position;
	Type& type;
};

class InvadersGrid {
	std::vector<Invader> invaders;
	bool next_direction_left;
	bool current_direction_left;
	float speed;
	int most_right;
	Type default_type;
public:
	InvadersGrid() { 
		next_direction_left = false;
		current_direction_left = false;
		speed = 0.2;
		most_right = 640;

	}

	void Init(int default_texture, int col_spacing, int row_spacing) {
		default_type.bounty = 40;
		default_type.fire_chance = 3;
		default_type.tex = default_texture;
		for(int i = 0; i < 7; ++i) {
			for(int u = 0; u < 3; ++u) {
				Invader inv(default_type);
				inv.position = Vector2d(i * col_spacing, u * row_spacing);
				invaders.push_back(inv);
			}
		}
	}

	void Update(float delta) {
		//current_direction = next_direction;
		Vector2d move(0,0);
		if(current_direction_left) {
			move.x = -speed * delta;
		}else{
			move.x = speed * delta;
		}
		if(current_direction_left != next_direction_left) {
			move.y = speed * delta;
		}
		current_direction_left = next_direction_left;
		for(auto i = invaders.begin(), end = invaders.end(); i != end; ++i) {
			i->position += move;

			if(next_direction_left != current_direction_left)
				continue;
			
			if(current_direction_left) {
				if(i->position.x <= 0) {
					next_direction_left = false;
				}
			}else{
				if(i->position.x >= most_right) {
					next_direction_left = true;
				}
			}
		}
	}
	void Render(IGraphics& graphics) {
		for(auto i = invaders.begin(), end = invaders.end(); i != end; ++i) {
			graphics.RenderTexture(i->type.tex, i->position);
		}
	}
};
class SpaceInvaders {
	InvadersGrid invaders;
public:
	void Setup(IGraphics& graphics) {
		int inv_tex = graphics.LoadTexture("invader.bmp");
		invaders.Init(inv_tex, 70, 70);
	}

	void Update(float delta) {
		invaders.Update(delta);
	}
	void Render(IGraphics& graphics) {
		invaders.Render(graphics);
	}
};

IGraphics* CreateTextGraphics();
IGraphics* CreateSDLGraphics();

int main(int argc, char* argv[]) {
	IGraphics* graphics = 0;
	SpaceInvaders space_invaders;

	std::cout << "Graphic or Text renderer(Text might not handle input)? [G/T]";
	char ans;
	std::cin >> ans;
	if(ans == 'G') {
		graphics = CreateSDLGraphics();
	}else{
		graphics = CreateTextGraphics();
	}

	//Creates the level. Calling this again would reset the game.
	space_invaders.Setup(*graphics);
	bool done = false;
	while(!done) {
		float delta = graphics->NextDelta();

		graphics->HandleEvents();

		space_invaders.Update(delta);

		graphics->Clear();
		space_invaders.Render(*graphics);
		graphics->Present();
	}
}