#include "Math.h"


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
float Vector2d::dot(Vector2d pos) const {
	return x * pos.x + y * pos.y;
}