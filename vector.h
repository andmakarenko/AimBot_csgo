#pragma once
#pragma once
#include <numbers>
#include <cmath>

using namespace std;

struct Vector
{
	float x, y, z;

	constexpr Vector(
		const float x = 0.f,
		const float y = 0.f,
		const float z = 0.f) noexcept : x(x), y(y), z(z)
	{}

	constexpr const Vector& operator-(const Vector& other) const noexcept
	{
		return Vector{ x - other.x, y - other.y, z - other.z };
	}

	constexpr const Vector& operator+(const Vector& other) const noexcept
	{
		return Vector{ x + other.x, y + other.y, z + other.z };
	}

	constexpr const Vector& operator/(const float factor) const noexcept
	{
		return Vector{ x / factor, y / factor, z / factor };
	}

	constexpr const Vector& operator*(const float factor) const noexcept
	{
		return Vector{ x * factor, y * factor, z * factor };
	}


	constexpr const Vector& ToAngle() const noexcept
	{
		return Vector{
			atan2(-z, hypot(x, y)) * (180.0f / numbers::pi_v<float>),
			atan2(y, x) * (180.0f / numbers::pi_v<float>),
			0.0f
		};
	}

	constexpr const bool IsZero() const noexcept
	{
		return x == 0.f && y == 0.f && z == 0.f;
	}
};