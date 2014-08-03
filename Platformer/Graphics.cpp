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
void SpriteScene::Render(SDL_Renderer* ren, Vector2d cam) {
	Sprite* sprite;
	ARRAY_ID id;
	SDL_Rect rect;
	for(int i = 0; i < sprites.GetElementCount(); ++i) {
		sprites.get_by_index(i, &id, &sprite);
		Vector2d abs_size(abs(sprite->size.x), abs(sprite->size.y));
		rect.x = sprite->pos.x - abs_size.x / 2 - cam.x;
		rect.y = sprite->pos.y - abs_size.y / 2 - cam.y;
		rect.w = abs_size.x;
		rect.h = abs_size.y;

		int flip_mask = SDL_FLIP_NONE;
		if(sprite->size.x < 0) flip_mask |= SDL_FLIP_HORIZONTAL;
		if(sprite->size.y < 0) flip_mask |= SDL_FLIP_VERTICAL;

		if(sprite->source.x | sprite->source.y | sprite->source.w | sprite->source.h) {
			SDL_RenderCopyEx(ren, sprite->texture.ptr, &sprite->source, &rect, 0, 0, (SDL_RendererFlip)flip_mask);
		}else {
			SDL_RenderCopyEx(ren, sprite->texture.ptr, 0, &rect, 0, 0, (SDL_RendererFlip)flip_mask);
		}
	}
}