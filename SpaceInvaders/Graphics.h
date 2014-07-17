#pragma once

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

enum EVENT_TYPE {
	ET_EXIT,
	ET_KEY_DOWN,
	ET_KEY_UP
};
typedef void (*event_callback)(EVENT_TYPE, int);

class IGraphics {
public:
	virtual void Clear() = 0;

	virtual void HandleEvents() = 0;
	virtual void SetCallback(event_callback clb) = 0;

	virtual void RenderTexture(int tex, Vector2d pos) = 0;
	virtual int LoadTexture(char* filename) = 0;

	virtual void Present() = 0;

	virtual int NextDelta() = 0;
};