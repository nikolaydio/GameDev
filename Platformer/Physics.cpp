#include "Physics.h"


bool AABBvsAABB(Rect* first, Rect* second, Vector2d* normal_out, float* pene_out) {
	Vector2d n = second->pos - first->pos;

	float a_extent = first->size.x / 2;
	float b_extent = second->size.x / 2;

	float x_overlap = a_extent + b_extent - abs(n.x);
	if(x_overlap > 0) {
		a_extent = first->size.y / 2;
		b_extent = second->size.y / 2;

		float y_overlap = a_extent + b_extent - abs(n.y);

		if(y_overlap > 0) {
			if(x_overlap < y_overlap) {
				if(n.x < 0) {
					*normal_out = Vector2d(-1, 0);
				}else{
					*normal_out = Vector2d(1, 0);
				}
				*pene_out = x_overlap;
				return true;
			}else{
				if(n.y < 0) {
					*normal_out = Vector2d(0, -1);
				}else{
					*normal_out = Vector2d(0, 1);
				}
				*pene_out = y_overlap;
				return true;
			}
		}
	}
	return false;
}
void ResolveCollision(Actor* first, Actor* second, Vector2d normal, float pene) {
	Vector2d rv = second->velo - first->velo;

	float vel = rv.dot( normal );
	if(vel > 0)
		return;

	float e = std::min( first->restitution, second->restitution );

	float mass_sum = first->inv_mass + second->inv_mass;

	float j = -(1 + e) * vel;
	j /= mass_sum;

	Vector2d impulse = normal * j;
	first->velo -= impulse * first->inv_mass;
	second->velo += impulse * second->inv_mass;

	const float percent = 0.4;
	Vector2d correction = normal * (pene / mass_sum * percent);
	first->shape.pos -= correction * first->inv_mass;
	second->shape.pos += correction * second->inv_mass;
}



void PhysicsWorld::Init(int entity_count) {
	actors.SetCapacity(entity_count);
}
void PhysicsWorld::UpdatePositions(float delta) {
	ARRAY_ID id;
	Actor* actor;
	for(int i = 0; i < actors.GetElementCount(); ++i) {
		actors.get_by_index(i, &id, &actor);

		
		actor->shape.pos += actor->velo * delta;
		if(actor->velo.x != 0 || actor->velo.y != 0) {
			move_events.push_back(MoveEvent(actor->shape.pos, id));
		}
	}
}
void PhysicsWorld::CollideAndRespond() {
	ARRAY_ID first_id, second_id;
	Actor *first, *second;
	
	float pene;
	Vector2d normal;
	
	for(int i = 0; i < actors.GetElementCount(); ++i) {
		actors.get_by_index(i, &first_id, &first);
		
		for(int j = i + 1; j < actors.GetElementCount(); ++j) {
			actors.get_by_index(j, &second_id, &second);

			if(AABBvsAABB(&first->shape, &second->shape, &normal, &pene)) {
				ResolveCollision(first, second, normal, pene);

				CollisionEvent collision;
				collision.first = first_id;
				collision.second = second_id;
				collision.normal = normal;
				collision_events.push_back(collision);
			}
			
		}
	}
}


void PhysicsWorld::AddActor(ARRAY_ID id, Actor actor) {
	actors.AllocateID(id);
	actors.lookup(id) = actor;
}
Actor& PhysicsWorld::GetActor(ARRAY_ID id) {
	return actors.lookup(id);
}
void PhysicsWorld::SetConstantForce(Vector2d force) {
	constant_force = force;
}
std::vector<CollisionEvent>& PhysicsWorld::GetCollisions() {
	return collision_events;
}
std::vector<MoveEvent>& PhysicsWorld::GetMoves() {
	return move_events;
}