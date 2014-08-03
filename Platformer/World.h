#pragma once
#include <vector>
#include "Graphics.h"
#include "Math.h"

class World;
class Entity {
public:
	virtual void Update(const World& world, float delta) {}
	virtual void Render(SDL_Renderer* renderer) = 0;
	virtual Vector2d VsRect(Vector2d pos, Vector2d size);
	virtual ~Entity() {}

	int GetCollisionMask();
protected:
	int mask;
};

class World {
	std::vector<Entity*> entities;
public:
	void AddEntity(Entity* ent);
	void RemoveEntity(Entity* ent);
	Vector2d VsRect(Vector2d pos, Vector2d size, int mask) const;
	void Update(float delta);
	void Render(SDL_Renderer* renderer);
};