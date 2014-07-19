#pragma once

#include <vector>
#include "Graphics.h"


struct Type {
	float fire_chance;
	int bounty;
	int tex;
};
class Projectile {
public:
	bool enemy_projectile;
	Vector2d pos;
};
class SpaceInvaders;

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
	InvadersGrid();
	int InvadersLeft();

	void Init(int default_texture, int col_spacing, int row_spacing, int inv_size);
	void Update(float delta, SpaceInvaders* inv);
	void Render(IGraphics& graphics);
	bool CheckCollision(Vector2d& pos);
};

enum GameStatus {
	GS_PLAYING,
	GS_VICTORY,
	GS_DEFEAT
};
class SpaceInvaders {
	InvadersGrid invaders;
	std::vector<Projectile> projectiles;
	int projectile_texture;
	float projectile_speed;
	Vector2d player_position;
	Vector2d player_size;
	Vector2d player_velocity;
	int player_texture;
	GameStatus status;
	int win_screen;
	int lose_screen;
	int invader_texture;
public:
	void LoadResources(IGraphics& graphics);
	void Reset();
	void RemoveProjectile(int index);
	void AddProjectile(const Projectile& proj);
	void Update(float delta);
	void Render(IGraphics& graphics);
	void MovePlayer(int dir);
	void Fire();
};