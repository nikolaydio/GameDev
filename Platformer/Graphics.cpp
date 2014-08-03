#include "Graphics.h"
#include <SDL_image.h>

Texture& Texture::operator=(const Texture& sec) {
	ptr = sec.ptr;
	width = sec.width;
	height = sec.height;
	return *this;
}

Texture ResourceManager::loadTexture(const std::string& fn) const {
	Texture tex;
	SDL_Surface* img = IMG_Load((prefix + fn).c_str());
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
void ResourceManager::Init(SDL_Renderer* renderer, char* prefix) {
	IMG_Init(IMG_INIT_PNG);
	ren = renderer;
	this->prefix = prefix;
}
Texture ResourceManager::GetTexture(const std::string& fn) {
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
void ResourceManager::AddTexture(const std::string& key, Texture tex) {
	textures[key] = tex;
}
void ResourceManager::Cleanup() {
	for(auto i = textures.begin(), end = textures.end(); i != end; ++i) {
		SDL_DestroyTexture(i->second.ptr);
	}
	textures.clear();
}


void SpriteScene::Init(int entity_count) {
	sprites.SetCapacity(entity_count);
}
void SpriteScene::SetPosition(ARRAY_ID id, Vector2d pos) {
	Sprite& spr = sprites.lookup(id);
	spr.pos = pos;
}
void SpriteScene::AddSprite(ARRAY_ID id, Sprite& spr) {
	sprites.AllocateID(id);
	sprites.lookup(id) = spr;
}
Sprite& SpriteScene::GetSpirte(ARRAY_ID id) {
	return sprites.lookup(id);
}
SparseArray<Sprite>& SpriteScene::GetSpriteList() {
	return sprites;
}
void SpriteScene::Render(SDL_Renderer* ren) {
	Sprite* sprite;
	ARRAY_ID id;
	SDL_Rect rect;
	for(int i = 0; i < sprites.GetElementCount(); ++i) {
		sprites.get_by_index(i, &id, &sprite);
		rect.x = sprite->pos.x - sprite->size.x / 2;
		rect.y = sprite->pos.y + sprite->size.y / 2;
		rect.w = sprite->size.x;
		rect.h = sprite->size.y;
		if(sprite->source.x | sprite->source.y | sprite->source.w | sprite->source.h) {
			SDL_RenderCopy(ren, sprite->texture.ptr, &sprite->source, &rect);
		}else {
			SDL_RenderCopy(ren, sprite->texture.ptr, 0, &rect);
		}
	}
}