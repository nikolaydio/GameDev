#include "World.h"

Vector2d Entity::VsRect(Vector2d pos, Vector2d size) {
	return Vector2d(0,0);
}

int Entity::GetCollisionMask() {
	return mask;
}


void World::AddEntity(Entity* ent) {
	entities.push_back(ent);
}
void World::RemoveEntity(Entity* ent) {
	for(int i = 0; i < entities.size(); ++i) {
		if(entities[i] == ent) {
			entities[i] = entities.back();
			entities.pop_back();
			break;
		}
	}
	delete ent;
}
Vector2d World::VsRect(Vector2d pos, Vector2d size, int mask) const {
	for(int i = 0; i < entities.size(); ++i) {
		if(entities[i]->GetCollisionMask() & mask) {
			Vector2d overlaps = entities[i]->VsRect(pos, size);
			if(overlaps.x != 0 && overlaps.y != 0) {
				return overlaps;
			}
		}
	}
	return Vector2d(0,0);
}
void World::Update(float delta) {
	for(int i = 0; i < entities.size(); ++i) {
		entities[i]->Update(*this, delta);
	}
}
void World::Render(SDL_Renderer* renderer) {
	for(int i = 0; i < entities.size(); ++i) {
		entities[i]->Render(renderer);
	}
}