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
	float dot(Vector2d pos) const;
	Vector2d operator-() const;
};

struct Rect {
	Vector2d pos;
	Vector2d size;
};