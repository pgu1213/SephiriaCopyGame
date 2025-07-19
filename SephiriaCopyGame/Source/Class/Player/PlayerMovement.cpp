#include <pch.h>
#include "PlayerMovement.h"
#include <Engine/Managers/InputManager/InputManager.h>
#include <Engine/Object/Object/Object.h>
#include <Engine/Object/Component/SpriteRenderer/SpriteRenderer.h>

PlayerMovement::PlayerMovement(Object* owner)
    : Component(owner)
	, m_Speed(200.0f)  // 픽셀/초
	, m_Velocity({ 0.0f, 0.0f })
{
}

PlayerMovement::PlayerMovement(const PlayerMovement& other)
    : Component(other.m_pOwner)
    , m_Speed(other.m_Speed)
    , m_Velocity(other.m_Velocity)
{
}

Component* PlayerMovement::CloneImpl() const
{
    return new PlayerMovement(*this);
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

    if (inputDir.x != 0.0f)
    {
        SpriteRenderer* spriteRenderer = m_pOwner->GetComponent<SpriteRenderer>();
        if (spriteRenderer)
        {
            spriteRenderer->SetFlipX(inputDir.x < 0.0f);
        }
    }

    // 벡터 정규화
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
    const Transform& currentTransform = m_pOwner->GetTransform();
    Vector2 newPosition = currentTransform.position;
    newPosition.x += m_Velocity.x * deltaTime;
    newPosition.y += m_Velocity.y * deltaTime;
    m_pOwner->SetPosition(newPosition);
	printf("Player Position: (%f, %f)\n", newPosition.x, newPosition.y);
}