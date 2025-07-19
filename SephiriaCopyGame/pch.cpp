// pch.cpp: 미리 컴파일된 헤더에 해당하는 소스 파일

#include "pch.h"

// 미리 컴파일된 헤더를 사용하는 경우 컴파일이 성공하려면 이 소스 파일이 필요합니다.

// Vector2 정적 상수 정의
const Vector2 Vector2::Zero(0.0f, 0.0f);
const Vector2 Vector2::One(1.0f, 1.0f);
const Vector2 Vector2::Up(0.0f, -1.0f);
const Vector2 Vector2::Down(0.0f, 1.0f);
const Vector2 Vector2::Left(-1.0f, 0.0f);
const Vector2 Vector2::Right(1.0f, 0.0f);