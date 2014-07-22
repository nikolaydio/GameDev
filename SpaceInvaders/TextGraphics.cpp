#include "Graphics.h"

#include <iostream>
#include <vector>

//Needed for the Sleep
#include <Windows.h>

#define T_ROWS 24
#define T_COLS 79
#define T_MAX_COORD_X 640
#define T_MAX_COORD_Y 480
class Text_Graphics : public IGraphics {
	std::vector<char> textures;
public:
	bool outside(int min, int max, int val) {
		if(val < min) return true;
		if(val >= max) return true;
		return false;
	}
	void Clear() {
		for(int i = 0; i < T_ROWS; ++i) {
			for(int u = 0; u < T_COLS; ++u)
				buffer[i][u] = '_';
		}
	}

	bool HandleEvent(GrEvent* ev) {
		return false;
	}

	int LoadSound(char* filename) {
		return -1;
	}
	void Text_Graphics::PlaySoundFile(int sound) {
		
	}


	void RenderTexture(int tex, Vector2d pos)  {
		int x = pos.x / (float)T_MAX_COORD_X * T_COLS;
		int y = pos.y / (float)T_MAX_COORD_Y * T_ROWS;
		if(!outside(0, T_COLS, x) && !outside(0, T_ROWS, y))
			buffer[y][x] = textures[tex];
	}
	
	int LoadTexture(char* filename) {
		textures.push_back(textures.size() + 'a');
		return textures.size() - 1;
	}
	void Present() {
		printf("\n\n\n");
		for(int i = 0; i < T_ROWS; ++i) {
			for(int u = 0; u < T_COLS; ++u) {
				printf("%c", buffer[i][u]);
			}
			printf("\n");
		}
		Sleep(100);
	}
	int NextDelta() {
		return 100;
	}
	char buffer[T_ROWS][T_COLS];
};

IGraphics* CreateTextGraphics() {
	return new Text_Graphics;
}