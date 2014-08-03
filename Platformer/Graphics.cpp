#include "Graphics.h"


Vector2d::Vector2d() : x(0), y(0) {}
Vector2d::Vector2d(float X, float Y) : x(X), y(Y) {}
Vector2d Vector2d::operator+(const Vector2d& sec) const {
	return Vector2d(x + sec.x, y + sec.y);
}
Vector2d& Vector2d::operator+=(const Vector2d& sec) {
	*this = *this + sec;
	return *this;
}
Vector2d Vector2d::operator-(const Vector2d& sec) const {
	return Vector2d(x - sec.x, y - sec.y);
}
Vector2d& Vector2d::operator-=(const Vector2d& sec) {
	*this = *this - sec;
	return *this;
}
Vector2d Vector2d::operator*(float delta) const {
	return Vector2d(x * delta, y * delta);
}











Texture& Texture::operator=(const Texture& sec) {
	ptr = sec.ptr;
	width = sec.width;
	height = sec.height;
	return *this;
}

Texture ResourceManager::loadTexture(const std::string& fn) const {
	Texture tex;
	SDL_Surface* img = SDL_LoadBMP((prefix + fn).c_str());
	if(img != 0) {

		tex.ptr = SDL_CreateTextureFromSurface(ren, img);
		tex.width = img->w;
		tex.height = img->h;
		SDL_FreeSurface(img);
		if(!tex.ptr) {
			printf("Failed to convert surface of %s", fn.c_str());
		}
	}else {
		printf("Failed to load %s", fn.c_str());
	}
	return tex;
}
ResourceManager::ResourceManager() : fails(0) {}
void ResourceManager::Init(SDL_Renderer* renderer) {
	ren = renderer;
}
Texture ResourceManager::getTexture(const std::string& fn) {
	auto i = textures.find(fn);
	if(i != textures.end()) {
		return (*i).second;
	}
	Texture tex = loadTexture(fn);
	if(tex.ptr != 0) {
		textures[fn] = tex;
	}
	if(tex.ptr == 0)
		++fails;
	return tex;
}
int ResourceManager::HasFailed() {
	int fail_count = fails;
	fails = 0;
	return fail_count;
}
void ResourceManager::addTexture(const std::string& key, Texture tex) {
	textures[key] = tex;
}
void ResourceManager::Cleanup() {
	for(auto i = textures.begin(), end = textures.end(); i != end; ++i) {
		SDL_DestroyTexture(i->second.ptr);
	}
	textures.clear();
}