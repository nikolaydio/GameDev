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
	
	Invader& operator=(const Invader& inv) {
		position = inv.position;
		type = inv.type;
		return *this;
	}
};

class InvadersGrid {
	std::vector<Invader> invaders;
	bool next_direction_left;
	bool current_direction_left;
	float speed;
	int most_right;
	Type default_type;
	int inv_size;
public:
	InvadersGrid() { 
		next_direction_left = false;
		current_direction_left = false;
		speed = 0.2;
		most_right = 640;

	}

	void Init(int default_texture, int col_spacing, int row_spacing, int inv_size) {
		default_type.bounty = 40;
		default_type.fire_chance = 3;
		default_type.tex = default_texture;
		most_right -= inv_size;
		this->inv_size = inv_size;
		for(int i = 0; i < 7; ++i) {
			for(int u = 0; u < 3; ++u) {
				Invader inv(default_type);
				inv.position = Vector2d(i * col_spacing, 480 - u * row_spacing);
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
			move.y = -speed * delta;
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
	bool CheckCollision(Vector2d& pos) {
		for(auto i = invaders.begin(), end = invaders.end(); i != end; ++i) {
			if(pos.x < i->position.x)
				continue;
			if(pos.x > i->position.x + inv_size)
				continue;
			if(pos.y < i->position.y - inv_size)
				continue;
			if(pos.y > i->position.y)
				continue;
			//remove invader
			*i = invaders.back();
			invaders.pop_back();
			return true;
		}
		return false;
	}
};
class Projectile {
public:
	bool enemy_projectile;
	Vector2d pos;
};
class SpaceInvaders {
	InvadersGrid invaders;
	std::vector<Projectile> projectiles;
	int projectile_texture;
	float projectile_speed;
	Vector2d player_position;
	Vector2d player_velocity;
	int player_texture;
public:
	void Setup(IGraphics& graphics) {
		int inv_tex = graphics.LoadTexture("invader.bmp");
		invaders.Init(inv_tex, 65, 65, 64);
		player_position = Vector2d(50, 50);
		player_texture = graphics.LoadTexture("ship.bmp");
		projectile_texture = graphics.LoadTexture("projectile.bmp");
		projectile_speed = 2;
	}
	void RemoveProjectile(int index) {
		projectiles[index] = projectiles.back();
		projectiles.pop_back();
	}
	void Update(float delta) {
		for(auto i = 0; i != projectiles.size(); ++i) {
			if(projectiles[i].enemy_projectile) {
				projectiles[i].pos += Vector2d(0, -projectile_speed) * delta;
				//check for collision with player
			}else{
				projectiles[i].pos += Vector2d(0, projectile_speed) * delta;
				//check for collision with invaders
				if(invaders.CheckCollision(projectiles[i].pos)){
					RemoveProjectile(i);
					--i;
				}
			}
		}
		invaders.Update(delta);
		player_position += player_velocity * delta;
	}
	void Render(IGraphics& graphics) {
		invaders.Render(graphics);
		for(auto i = projectiles.begin(); i != projectiles.end(); ++i) {
			graphics.RenderTexture(projectile_texture, i->pos);
		}
		graphics.RenderTexture(player_texture, player_position);
	}
	void MovePlayer(int dir) {
		player_velocity.x = dir;
	}
	void Fire() {
		Projectile p;
		p.enemy_projectile = false;
		p.pos = player_position;
		projectiles.push_back(p);
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
	GrEvent ev;
	while(!done) {
		float delta = graphics->NextDelta();

		while(graphics->HandleEvent(&ev)) {
			if(ev.type == ET_EXIT) {
				done = true;
			} else if(ev.type == ET_KEY_DOWN) {
				if(ev.data == EK_RIGHT) {
					space_invaders.MovePlayer(1);
				}else if(ev.data == EK_LEFT) {
					space_invaders.MovePlayer(-1);
				}else if(ev.data == EK_UP) {
					space_invaders.Fire();
				}
			} else if(ev.type == ET_KEY_UP) {
				if(ev.data == EK_RIGHT) {
					space_invaders.MovePlayer(0);
				}else if(ev.data == EK_LEFT) {
					space_invaders.MovePlayer(0);
				}
			}
		}

		space_invaders.Update(delta);

		graphics->Clear();
		space_invaders.Render(*graphics);
		graphics->Present();
	}
}