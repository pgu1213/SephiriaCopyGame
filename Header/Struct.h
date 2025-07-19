#pragma once

struct Vector2
{
public:
	Vector2() : x(0.0f), y(0.0f) {}
	Vector2(float x, float y) : x(x), y(y) {}

	// 복사 생성자 및 대입 연산자 (기본값 사용)
	Vector2(const Vector2& other) = default;
	Vector2& operator=(const Vector2& other) = default;

public:
	float x;
	float y;

public:
	// 산술 연산자
	Vector2 operator+(const Vector2& other) const
	{
		return Vector2(x + other.x, y + other.y);
	}

	Vector2 operator-(const Vector2& other) const
	{
		return Vector2(x - other.x, y - other.y);
	}

	Vector2 operator*(float scalar) const
	{
		return Vector2(x * scalar, y * scalar);
	}

	Vector2 operator/(float scalar) const
	{
		if (scalar != 0.0f)
			return Vector2(x / scalar, y / scalar);
		return Vector2(0.0f, 0.0f);
	}

	// 단항 연산자
	Vector2 operator-() const
	{
		return Vector2(-x, -y);
	}

	// 복합 대입 연산자
	Vector2& operator+=(const Vector2& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	Vector2& operator-=(const Vector2& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vector2& operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	Vector2& operator/=(float scalar)
	{
		if (scalar != 0.0f)
		{
			x /= scalar;
			y /= scalar;
		}
		return *this;
	}

	// 비교 연산자
	bool operator==(const Vector2& other) const
	{
		return (abs(x - other.x) < 0.001f && abs(y - other.y) < 0.001f);
	}

	bool operator!=(const Vector2& other) const
	{
		return !(*this == other);
	}

	// 벡터 관련 함수
	float Length() const
	{
		return sqrtf(x * x + y * y);
	}

	float LengthSquared() const
	{
		return x * x + y * y;
	}

	Vector2 Normalize() const
	{
		float length = Length();
		if (length > 0.0f)
			return Vector2(x / length, y / length);
		return Vector2(0.0f, 0.0f);
	}

	void NormalizeThis()
	{
		float length = Length();
		if (length > 0.0f)
		{
			x /= length;
			y /= length;
		}
	}

	float Dot(const Vector2& other) const
	{
		return x * other.x + y * other.y;
	}

	// 두 벡터 사이의 거리
	float Distance(const Vector2& other) const
	{
		return (*this - other).Length();
	}

	float DistanceSquared(const Vector2& other) const
	{
		return (*this - other).LengthSquared();
	}

	// 벡터를 각도로 변환 (라디안)
	float ToAngle() const
	{
		return atan2f(y, x);
	}

	// 각도에서 벡터 생성 (라디안)
	static Vector2 FromAngle(float angle, float length = 1.0f)
	{
		return Vector2(cosf(angle) * length, sinf(angle) * length);
	}

	// 선형 보간
	static Vector2 Lerp(const Vector2& start, const Vector2& end, float t)
	{
		t = max(0.0f, min(1.0f, t)); // t를 0~1 범위로 제한
		return start + (end - start) * t;
	}

	// 유용한 정적 상수들
	static const Vector2 Zero;
	static const Vector2 One;
	static const Vector2 Up;
	static const Vector2 Down;
	static const Vector2 Left;
	static const Vector2 Right;
};

inline Vector2 operator*(float scalar, const Vector2& vector)
{
	return vector * scalar;
}

struct Transform
{
	Vector2 position = { 0.f, 0.f };
	float rotation = 0.0f;
	Vector2 scale = { 1.0f, 1.0f };
};