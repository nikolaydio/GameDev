#pragma once
#include <SDL.h>
#include <string>
#include <unordered_map>

struct Vector2d {
	float x, y;
	Vector2d();
	Vector2d(float X, float Y);
	Vector2d operator+(const Vector2d& sec) const;
	Vector2d& operator+=(const Vector2d& sec);
	Vector2d operator-(const Vector2d& sec) const;
	Vector2d& operator-=(const Vector2d& sec);
	Vector2d operator*(float delta) const;
};


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
	void Init(SDL_Renderer* renderer);
	Texture getTexture(const std::string& fn);
	int HasFailed();
	void addTexture(const std::string& key, Texture tex);
	void Cleanup();
};