#include <pch.h>
#include "PlayerMovement.h"
#include <Engine/Managers/InputManager/InputManager.h>
#include <Engine/Object/Object/Object.h>
#include <Engine/Object/Component/SpriteRenderer/SpriteRenderer.h>
#include <Engine/Object/Component/Animation/Animation.h>

PlayerMovement::PlayerMovement(Object* owner)
    : Component(owner)
    , m_Speed(200.0f)        // �⺻ �̵� �ӵ�
    , m_Velocity({ 0.0f, 0.0f })
    , m_DashSpeed(600.0f)    // �뽬 �ӵ�
    , m_DashDuration(0.2f)   // �뽬 ���� �ð�
    , m_DashCooldown(1.0f)   // �뽬 ��ٿ�
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

    // �뽬 ��ٿ� ������Ʈ
    if (m_DashCooldownTimer > 0.0f)
    {
        m_DashCooldownTimer -= deltaTime;
    }

    // �뽬 ���� ��
    if (m_IsDashing)
    {
        m_DashTimer -= deltaTime;

        if (m_DashTimer <= 0.0f)
        {
            // �뽬 ����
            m_IsDashing = false;
            m_DashCooldownTimer = m_DashCooldown;
        }
        else
        {
            // �뽬 �̵� ó��
            m_Velocity.x = m_DashDirection.x * m_DashSpeed;
            m_Velocity.y = m_DashDirection.y * m_DashSpeed;

            // ��ġ ������Ʈ
            const Transform& currentTransform = m_pObjOwner->GetTransform();
            Vector2 newPosition = currentTransform.position;
            newPosition.x += m_Velocity.x * deltaTime;
            newPosition.y += m_Velocity.y * deltaTime;
            m_pObjOwner->SetPosition(newPosition);

            return; // �뽬 �߿��� �Ϲ� �̵� ó������ ����
        }
    }

    // �Ϲ� �̵� �Է� ó��
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

    // �뽬 �Է� ó�� (�����̽���)
    if (input->IsKeyDown(VK_SPACE) && m_DashCooldownTimer <= 0.0f && !m_IsDashing)
    {
        // �뽬 ���� ����
        if (inputDir.x != 0.0f || inputDir.y != 0.0f)
        {
            // ���� �Է� �������� �뽬
            float length = sqrt(inputDir.x * inputDir.x + inputDir.y * inputDir.y);
            m_DashDirection.x = inputDir.x / length;
            m_DashDirection.y = inputDir.y / length;
        }
        else
        {
            // �Է��� ������ ���������� ���ߴ� �������� �뽬 (�⺻��: ������)
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

        // �뽬 ����
        m_IsDashing = true;
        m_DashTimer = m_DashDuration;

        return; // �뽬 ���� �� �Ϲ� �̵� ó������ ����
    }

    // ��������Ʈ ���� ó��
    if (inputDir.x != 0.0f)
    {
        SpriteRenderer* spriteRenderer = m_pObjOwner->GetComponent<SpriteRenderer>();
        if (spriteRenderer)
        {
            spriteRenderer->SetFlipX(inputDir.x < 0.0f);
        }
    }

    // �Ϲ� �̵� �ִϸ��̼�
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

    // ���� ����ȭ
    float length = sqrt(inputDir.x * inputDir.x + inputDir.y * inputDir.y);
    if (length > 0.0f)
    {
        inputDir.x /= length;
        inputDir.y /= length;
    }

    // �Ϲ� �̵� �ӵ� ���
    m_Velocity.x = inputDir.x * m_Speed;
    m_Velocity.y = inputDir.y * m_Speed;

    // ��ġ ������Ʈ
    const Transform& currentTransform = m_pObjOwner->GetTransform();
    Vector2 newPosition = currentTransform.position;
    newPosition.x += m_Velocity.x * deltaTime;
    newPosition.y += m_Velocity.y * deltaTime;
    m_pObjOwner->SetPosition(newPosition);
}