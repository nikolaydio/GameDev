#pragma once
#include <SDL.h>
#include <string>
#include <unordered_map>
#include "SparseArray.h"
#include "Math.h"



class Texture {
public:
	SDL_Texture* ptr;
	int width;
	int height;
	Texture() { ptr = 0; width = 0; height = 0; }
	Texture& operator=(const Texture& sec);
};


class ResourceManager {
	Texture loadTexture(const std::string& fn) const;

	SDL_Renderer* ren;
	int fails;
	std::unordered_map<std::string, Texture> textures;
	std::string prefix;
public:
	ResourceManager();
	void Init(SDL_Renderer* renderer, char* prefix);
	Texture GetTexture(const std::string& fn);
	int HasFailed();
	void AddTexture(const std::string& key, Texture tex);
	void Cleanup();
};


class Sprite {
public:
	Vector2d pos;
	Vector2d size;
	SDL_Rect source;
	Texture texture;
};
class SpriteScene {
	SparseArray<Sprite> sprites;
public:
	void Init(int entity_count);

	void SetPosition(ARRAY_ID id, Vector2d pos);
	void AddSprite(ARRAY_ID id, Sprite& spr);
	Sprite& GetSpirte(ARRAY_ID id);

	SparseArray<Sprite>& GetSpriteList();
	void Render(SDL_Renderer* ren, Vector2d cam);
};