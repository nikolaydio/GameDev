#include <vector>
#include "Graphics.h"

#include <iostream>
#include "SpaceInvaders.h"



inline bool PositionInside(Vector2d pos, Vector2d box_pos, Vector2d box_size) {
	if(pos.x < box_pos.x)
		return false;
	if(pos.x > box_pos.x + box_size.x)
		return false;
	if(pos.y < box_pos.y - box_size.y)
		return false;
	if(pos.y > box_size.y)
		return false;
	return true;
}
InvadersGrid::InvadersGrid() { 

}
int InvadersGrid::InvadersLeft() {
	return invaders.size();
}

void InvadersGrid::Init(int default_texture, int col_spacing, int row_spacing, int inv_size) {
	invaders.clear();
	next_direction_left = false;
	current_direction_left = false;
	speed = 0.2;
	most_right = 640;

	default_type.bounty = 40;
	default_type.fire_chance = 0.75;
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

void InvadersGrid::Update(float delta, SpaceInvaders* inv) {
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

		if(rand() % 10000 <= i->type.fire_chance * delta) {
			Projectile p;
			p.enemy_projectile = true;
			p.pos = i->position;
			inv->AddProjectile(p);
		}

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
void InvadersGrid::Render(IGraphics& graphics) {
	for(auto i = invaders.begin(), end = invaders.end(); i != end; ++i) {
		graphics.RenderTexture(i->type.tex, i->position);
	}
}
bool InvadersGrid::CheckCollision(Vector2d& pos) {
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


void SpaceInvaders::LoadResources(IGraphics& graphics) {
	win_screen = graphics.LoadTexture("victory.bmp");
	lose_screen = graphics.LoadTexture("defeat.bmp");
	player_texture = graphics.LoadTexture("ship.bmp");
	projectile_texture = graphics.LoadTexture("projectile.bmp");
	invader_texture = graphics.LoadTexture("invader.bmp");
	player_bullet_sound = graphics.LoadSound("PlayerBullet.wav");
}
void SpaceInvaders::Reset() {
	invaders.Init(invader_texture, 65, 65, 64);
	projectiles.clear();
	player_position = Vector2d(50, 50);
	player_size = Vector2d(104, 64);
	projectile_speed = 0.75;
	status = GS_PLAYING;
}
void SpaceInvaders::RemoveProjectile(int index) {
	projectiles[index] = projectiles.back();
	projectiles.pop_back();
}
void SpaceInvaders::AddProjectile(const Projectile& proj) {
	projectiles.push_back(proj);
}
void SpaceInvaders::Update(float delta) {
	if(status == GS_PLAYING) {
		for(auto i = 0; i != projectiles.size(); ++i) {
			if(projectiles[i].enemy_projectile) {
				projectiles[i].pos += Vector2d(0, -projectile_speed) * delta;
				//check for collision with player
				if(PositionInside(projectiles[i].pos, player_position, player_size)) {
					status = GS_DEFEAT;
				}
				continue;
			}else{
				projectiles[i].pos += Vector2d(0, projectile_speed) * delta;
				//check for collision with invaders
				if(invaders.CheckCollision(projectiles[i].pos)){
					RemoveProjectile(i);
					--i;
					if(invaders.InvadersLeft() == 0) {
						status = GS_VICTORY;
					}
					continue;
				}
			}
			if(!PositionInside(projectiles[i].pos, Vector2d(0, 0), Vector2d(640, 480))) {
				RemoveProjectile(i);
				--i;
			}
		}
		invaders.Update(delta, this);
		player_position += player_velocity * delta;
	}
}
void SpaceInvaders::Render(IGraphics& graphics) {
	invaders.Render(graphics);
	for(auto i = projectiles.begin(); i != projectiles.end(); ++i) {
		graphics.RenderTexture(projectile_texture, i->pos);
	}
	graphics.RenderTexture(player_texture, player_position);

	if(status == GS_VICTORY) {
		graphics.RenderTexture(win_screen, Vector2d(20, 400));
	}else if(status == GS_DEFEAT) {
		graphics.RenderTexture(lose_screen, Vector2d(20, 400));
	}
}
void SpaceInvaders::MovePlayer(int dir) {
	player_velocity.x = dir;
}
void SpaceInvaders::Fire(IGraphics& gr) {
	if(status == GS_VICTORY || status == GS_DEFEAT) {
		Reset();
		return;
	}
	Projectile p;
	p.enemy_projectile = false;
	p.pos = player_position;
	projectiles.push_back(p);
	gr.PlaySoundFile(player_bullet_sound);
}


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
	space_invaders.LoadResources(*graphics);
	space_invaders.Reset();

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
					space_invaders.Fire(*graphics);
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
	delete graphics;
}