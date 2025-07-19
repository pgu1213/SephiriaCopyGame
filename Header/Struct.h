#pragma once

struct Vector2
{
public:
	Vector2(float x, float y) : x(0.f), y(0.f) {}
public:
	float x;
	float y;
};

struct Transform
{
	Vector2 position = { 0.f, 0.f };
	float rotation = 0.0f;
	Vector2 scale = { 1.0f, 1.0f };
};