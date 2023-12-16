#pragma once
namespace lyra{
namespace Math{
class Vector2f;
class Vector2i
{
public:
	static const Vector2i Zero;
	static const Vector2i UnitX;
	static const Vector2i UnitY;
	static const Vector2i One;

	Vector2i() = default; 
	Vector2i(int x, int y);
	Vector2i(const Vector2f& v);

	Vector2i& operator+=(const Vector2i&);
	Vector2i operator+(const Vector2i&) const;
	Vector2i operator-(const Vector2i&) const;
	Vector2i operator*(const int&)const;
	Vector2i operator*(const float&)const;
	Vector2i operator*(const Vector2i&)const;
	Vector2i operator/(const int&) const;
	bool operator==(const Vector2i&) const;
	bool operator!=(const Vector2i&) const;

	Vector2i operator-() const;

	union 
	{
		int v[2];
		struct 
		{
			int x;
			int y;
		};
	};

};
//----------------------------------------------------------------------------------------------------------------------
inline Vector2i::Vector2i(int x, int y)
	: x(x)
	, y(y) 
{
}
//----------------------------------------------------------------------------------------------------------------------
inline Vector2i& Vector2i::operator+=(const Vector2i& other)
{
	x += other.x;
	y += other.y;
	return *this;
}
//----------------------------------------------------------------------------------------------------------------------
inline bool Vector2i::operator==(const Vector2i& other) const
{
	return (x == other.x && y == other.y);
}
//----------------------------------------------------------------------------------------------------------------------
inline bool Vector2i::operator!=(const Vector2i& other) const
{
	return (x != other.x || y != other.y);
}
//----------------------------------------------------------------------------------------------------------------------
inline Vector2i Vector2i::operator+(const Vector2i& other) const
{
	return {x + other.x, y + other.y};
}
//----------------------------------------------------------------------------------------------------------------------
inline Vector2i Vector2i::operator-(const Vector2i& other) const
{
	return {x - other.x, y - other.y};
}
//----------------------------------------------------------------------------------------------------------------------
inline Vector2i Vector2i::operator*(const int& value) const
{
	return {x * value, y * value};
}
//----------------------------------------------------------------------------------------------------------------------
inline Vector2i Vector2i::operator*(const float& value) const
{
return {static_cast<int>(static_cast<float>(x) * value), static_cast<int>(static_cast<float>(y) * value)};
}
//----------------------------------------------------------------------------------------------------------------------
	inline Vector2i Vector2i::operator*(const Vector2i& value) const
{
	return {x * value.x, y * value.y};
}
//----------------------------------------------------------------------------------------------------------------------
inline Vector2i Vector2i::operator/(const int& value) const
{
	return {x / value, y / value};
}
//----------------------------------------------------------------------------------------------------------------------
inline Vector2i Vector2i::operator-() const
{
	return {-x, -y};
}
}}