#include <pch.h>
#include "PlayerMovement.h"
#include <Engine/Managers/InputManager/InputManager.h>
#include <Engine/Object/Object/Object.h>
#include <Engine/Object/Component/SpriteRenderer/SpriteRenderer.h>
#include <Engine/Object/Component/Animation/Animation.h>

PlayerMovement::PlayerMovement(Object* owner)
    : Component(owner)
    , m_Speed(200.0f)        // 기본 이동 속도
    , m_Velocity({ 0.0f, 0.0f })
    , m_DashSpeed(600.0f)    // 대쉬 속도
    , m_DashDuration(0.2f)   // 대쉬 지속 시간
    , m_DashCooldown(1.0f)   // 대쉬 쿨다운
    , m_DashTimer(0.0f)
    , m_DashCooldownTimer(0.0f)
    , m_IsDashing(false)
    , m_DashDirection({ 0.0f, 0.0f })
{
}

PlayerMovement::PlayerMovement(const PlayerMovement& other)
    : Component(other.m_pObjOwner)
    , m_Speed(other.m_Speed)
    , m_Velocity(other.m_Velocity)
    , m_DashSpeed(other.m_DashSpeed)
    , m_DashDuration(other.m_DashDuration)
    , m_DashCooldown(other.m_DashCooldown)
    , m_DashTimer(other.m_DashTimer)
    , m_DashCooldownTimer(other.m_DashCooldownTimer)
    , m_IsDashing(other.m_IsDashing)
    , m_DashDirection(other.m_DashDirection)
{
}

Component* PlayerMovement::CloneImpl() const
{
    return new PlayerMovement(*this);
}

void PlayerMovement::Update(float deltaTime)
{
    if (!m_pObjOwner) return;

    InputManager* input = InputManager::GetInstance();

    // 대쉬 쿨다운 업데이트
    if (m_DashCooldownTimer > 0.0f)
    {
        m_DashCooldownTimer -= deltaTime;
    }

    // 대쉬 중일 때
    if (m_IsDashing)
    {
        m_DashTimer -= deltaTime;

        if (m_DashTimer <= 0.0f)
        {
            // 대쉬 종료
            m_IsDashing = false;
            m_DashCooldownTimer = m_DashCooldown;
        }
        else
        {
            // 대쉬 이동 처리
            m_Velocity.x = m_DashDirection.x * m_DashSpeed;
            m_Velocity.y = m_DashDirection.y * m_DashSpeed;

            // 위치 업데이트
            const Transform& currentTransform = m_pObjOwner->GetTransform();
            Vector2 newPosition = currentTransform.position;
            newPosition.x += m_Velocity.x * deltaTime;
            newPosition.y += m_Velocity.y * deltaTime;
            m_pObjOwner->SetPosition(newPosition);

            return; // 대쉬 중에는 일반 이동 처리하지 않음
        }
    }

    // 일반 이동 입력 처리
    Vector2 inputDir = { 0.0f, 0.0f };

    if (input->IsKeyPressed('A'))
    {
        inputDir.x -= 1.0f;
    }
    if (input->IsKeyPressed('D'))
    {
        inputDir.x += 1.0f;
    }
    if (input->IsKeyPressed('W'))
    {
        inputDir.y -= 1.0f;
    }
    if (input->IsKeyPressed('S'))
    {
        inputDir.y += 1.0f;
    }

    // 대쉬 입력 처리 (스페이스바)
    if (input->IsKeyDown(VK_SPACE) && m_DashCooldownTimer <= 0.0f && !m_IsDashing)
    {
        // 대쉬 방향 설정
        if (inputDir.x != 0.0f || inputDir.y != 0.0f)
        {
            // 현재 입력 방향으로 대쉬
            float length = sqrt(inputDir.x * inputDir.x + inputDir.y * inputDir.y);
            m_DashDirection.x = inputDir.x / length;
            m_DashDirection.y = inputDir.y / length;
        }
        else
        {
            // 입력이 없으면 마지막으로 향했던 방향으로 대쉬 (기본값: 오른쪽)
            SpriteRenderer* spriteRenderer = m_pObjOwner->GetComponent<SpriteRenderer>();
            if (spriteRenderer && spriteRenderer->GetFlipX())
            {
                m_DashDirection.x = -1.0f;
            }
            else
            {
                m_DashDirection.x = 1.0f;
            }
            m_DashDirection.y = 0.0f;
        }

        // 대쉬 시작
        m_IsDashing = true;
        m_DashTimer = m_DashDuration;

        return; // 대쉬 시작 시 일반 이동 처리하지 않음
    }

    // 스프라이트 반전 처리
    if (inputDir.x != 0.0f)
    {
        SpriteRenderer* spriteRenderer = m_pObjOwner->GetComponent<SpriteRenderer>();
        if (spriteRenderer)
        {
            spriteRenderer->SetFlipX(inputDir.x < 0.0f);
        }
    }

    // 일반 이동 애니메이션
    Animation* animation = m_pObjOwner->GetComponent<Animation>();
    if (animation)
    {
        if (inputDir.x != 0.0f || inputDir.y != 0.0f)
        {
            if (inputDir.y < 0.0f)
            {
                animation->PlayClip("UpperWalk");
            }
            else
            {
                animation->PlayClip("LowerWalk");
            }
        }
        else
        {
            // Idle
        }
    }

    // 벡터 정규화
    float length = sqrt(inputDir.x * inputDir.x + inputDir.y * inputDir.y);
    if (length > 0.0f)
    {
        inputDir.x /= length;
        inputDir.y /= length;
    }

    // 일반 이동 속도 계산
    m_Velocity.x = inputDir.x * m_Speed;
    m_Velocity.y = inputDir.y * m_Speed;

    // 위치 업데이트
    const Transform& currentTransform = m_pObjOwner->GetTransform();
    Vector2 newPosition = currentTransform.position;
    newPosition.x += m_Velocity.x * deltaTime;
    newPosition.y += m_Velocity.y * deltaTime;
    m_pObjOwner->SetPosition(newPosition);
}