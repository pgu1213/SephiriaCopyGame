#include <pch.h>
#include "PlayerMovement.h"
#include <Engine/Managers/InputManager/InputManager.h>
#include <Engine/Object/Object/Object.h>

PlayerMovement::PlayerMovement(Object* owner) : Component(owner)
{
}

void PlayerMovement::Update(float deltaTime)
{
    if (!m_pOwner) return;

    Vector2 inputDir = { 0.0f, 0.0f };

    // 입력 처리
    InputManager* input = InputManager::GetInstance();

    if (input->IsKeyPressed('A') || input->IsKeyPressed(VK_LEFT))
        inputDir.x -= 1.0f;
    if (input->IsKeyPressed('D') || input->IsKeyPressed(VK_RIGHT))
        inputDir.x += 1.0f;
    if (input->IsKeyPressed('W') || input->IsKeyPressed(VK_UP))
        inputDir.y -= 1.0f;
    if (input->IsKeyPressed('S') || input->IsKeyPressed(VK_DOWN))
        inputDir.y += 1.0f;

    // 벡터 정규화 (대각선 이동 시 속도 일정하게)
    float length = sqrt(inputDir.x * inputDir.x + inputDir.y * inputDir.y);
    if (length > 0.0f)
    {
        inputDir.x /= length;
        inputDir.y /= length;
    }

    // 속도 계산
    m_Velocity.x = inputDir.x * m_Speed;
    m_Velocity.y = inputDir.y * m_Speed;

    // 위치 업데이트
    /*Transform& transform = m_pOwner->GetTransform();
    transform.position.x += m_Velocity.x * deltaTime;
    transform.position.y += m_Velocity.y * deltaTime;*/
}