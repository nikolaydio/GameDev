#pragma once
#include "SparseArray.h"
#include "Math.h"
#include <vector>

struct CollisionEvent {
	Vector2d normal;
	ARRAY_ID first;
	ARRAY_ID second;
};
struct MoveEvent {
	MoveEvent(Vector2d p, ARRAY_ID i) : pos(p), id(i) {}

	ARRAY_ID id;
	Vector2d pos;
};
struct Actor {
	//It has:
	//shape
	//velo
	//pos
	//This class will be splitted on need

	Rect shape;

	Vector2d velo;

	float inv_mass;
	float restitution;
};
class PhysicsWorld {
	SparseArray<Actor> actors;
	std::vector<MoveEvent> move_events;
	std::vector<CollisionEvent> collision_events;
	Vector2d constant_force;
public:
	void Init(int entity_count);
	void UpdatePositions(float delta);
	void CollideAndRespond();

	void AddActor(ARRAY_ID id, Actor actor);
	Actor& GetActor(ARRAY_ID id);
	void SetConstantForce(Vector2d force);

	std::vector<CollisionEvent>& GetCollisions();
	std::vector<MoveEvent>& GetMoves();
};