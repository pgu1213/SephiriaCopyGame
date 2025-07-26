#include <pch.h>
#include "PlayerMovement.h"
#include <Engine/Managers/InputManager/InputManager.h>
#include <Engine/Managers/SceneManager/SceneManager.h>
#include <Engine/Object/Object/Object.h>
#include <Engine/Object/Component/SpriteRenderer/SpriteRenderer.h>
#include <Engine/Object/Component/Animation/Animation.h>
#include <Engine/Object/Component/Camera/Camera.h>

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
    , m_AttackComboCount(0)      // ���� �޺� ī��Ʈ
    , m_AttackComboMaxTime(1.0f) // �޺� ���� �ð�
    , m_AttackComboTimer(0.0f)   // �޺� Ÿ�̸�
    , m_IsAttacking(false)       // ���� �� ����
    , m_AttackDuration(0.6f)     // ���� ���� �ð�
    , m_AttackTimer(0.2f)        // ���� Ÿ�̸�
    , m_AttackLungeSpeed(400.0f) // ���� �� ���� �ӵ�
    , m_AttackLungeDuration(0.15f) // ���� �� ���� ���� �ð�
    , m_AttackLungeTimer(0.0f)     // ���� ���� Ÿ�̸�
    , m_IsAttackLunging(false)     // ���� ���� �� ����
    , m_AttackLungeDirection({ 0.0f, 0.0f }) // ���� ���� ����
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
    , m_AttackComboCount(other.m_AttackComboCount)
    , m_AttackComboMaxTime(other.m_AttackComboMaxTime)
    , m_AttackComboTimer(other.m_AttackComboTimer)
    , m_IsAttacking(other.m_IsAttacking)
    , m_AttackDuration(other.m_AttackDuration)
    , m_AttackTimer(other.m_AttackTimer)
    , m_AttackLungeSpeed(other.m_AttackLungeSpeed)
    , m_AttackLungeDuration(other.m_AttackLungeDuration)
    , m_AttackLungeTimer(other.m_AttackLungeTimer)
    , m_IsAttackLunging(other.m_IsAttackLunging)
    , m_AttackLungeDirection(other.m_AttackLungeDirection)
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

    // ���� ���� Ÿ�̸� ������Ʈ
    if (m_IsAttackLunging)
    {
        m_AttackLungeTimer -= deltaTime;
        if (m_AttackLungeTimer <= 0.0f)
        {
            m_IsAttackLunging = false;
        }
    }

    // ���� �޺� Ÿ�̸� ������Ʈ
    if (m_AttackComboTimer > 0.0f)
    {
        m_AttackComboTimer -= deltaTime;
        if (m_AttackComboTimer <= 0.0f)
        {
            // �޺� �ð� �ʰ� �� ����
            m_AttackComboCount = 0;
        }
    }

    // ���� Ÿ�̸� ������Ʈ
    if (m_IsAttacking)
    {
        m_AttackTimer -= deltaTime;
        if (m_AttackTimer <= 0.0f)
        {
            m_IsAttacking = false;
        }
    }

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

            // �뽬 �߿��� ���� �����ϹǷ� return ���� ����
        }
    }

    // ���� ���� ���� ��
    if (m_IsAttackLunging)
    {
        // ���� ���� �̵� ó��
        m_Velocity.x = m_AttackLungeDirection.x * m_AttackLungeSpeed;
        m_Velocity.y = m_AttackLungeDirection.y * m_AttackLungeSpeed;

        // ��ġ ������Ʈ
        const Transform& currentTransform = m_pObjOwner->GetTransform();
        Vector2 newPosition = currentTransform.position;
        newPosition.x += m_Velocity.x * deltaTime;
        newPosition.y += m_Velocity.y * deltaTime;
        m_pObjOwner->SetPosition(newPosition);
    }

    // �Ϲ� �̵� �Է� ó��
    Vector2 inputDir = { 0.0f, 0.0f };

    // ���� ���� �ƴ� ���� �̵� �Է� �ޱ�
    if (!m_IsAttacking)
    {
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
    }

    // ���� �Է� ó�� (���콺 ��Ŭ��)
    if (input->IsKeyDown(VK_LBUTTON))
    {
        HandleAttack(inputDir);
    }

    // �뽬 �Է� ó�� (�����̽���) - ���� �߿��� ����
    if (input->IsKeyDown(VK_SPACE) && m_DashCooldownTimer <= 0.0f && !m_IsDashing)
    {
        HandleDash(inputDir);
    }

    Scene* currentScene = SceneManager::GetInstance()->GetCurrentScene();
    Object* FXObject = currentScene->FindGameObject("PlayerSwingFX");

    // ���� ���� �ƴϰ� �뽬/���ݵ��� ���� �ƴ� ���� �Ϲ� �̵��� �ִϸ��̼� ó��
    if (!m_IsAttacking && !m_IsDashing && !m_IsAttackLunging)
    {
        FXObject->SetActive(false);
        UpdateAnimationBasedOnMouse(inputDir);
        HandleMovement(inputDir, deltaTime);
    }
    else if (m_IsAttacking && !m_IsDashing && !m_IsAttackLunging)
    {
        FXObject->SetActive(true);
        // ���� ���� ���� �ִϸ��̼Ǹ� ������Ʈ (�̵� ���� ������Ʈ��)
        UpdateAnimationBasedOnMouse(Vector2::Zero);
    }
}

void PlayerMovement::HandleAttack(const Vector2& inputDir)
{
    Scene* currentScene = SceneManager::GetInstance()->GetCurrentScene();
    Object* FXObject = currentScene->FindGameObject("PlayerSwingFX");
    Animation* animation = FXObject->GetComponent<Animation>();
    if (!animation) return;

    // �޺� ī��Ʈ ����
    m_AttackComboCount++;
    if (m_AttackComboCount > 3)
    {
        m_AttackComboCount = 1; // 3�� �޺� �� �ٽ� 1������
    }

    // ���� ���� ����
    m_IsAttacking = true;
    m_AttackTimer = m_AttackDuration;
    m_AttackComboTimer = m_AttackComboMaxTime;

    // �޺��� ���� �ִϸ��̼� ���
    string attackAnimName = "SwingFX" + to_string(m_AttackComboCount);
    animation->PlayClip(attackAnimName);

    // ���� �� ���� ó��
    Vector2 lungeDirection = CalculateAttackLungeDirection(inputDir);
    PerformAttackLunge(lungeDirection);
}

void PlayerMovement::HandleDash(const Vector2& inputDir)
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
}

void PlayerMovement::HandleMovement(const Vector2& inputDir, float deltaTime)
{
    // ���� ����ȭ
    Vector2 normalizedDir = inputDir;
    float length = sqrt(inputDir.x * inputDir.x + inputDir.y * inputDir.y);
    if (length > 0.0f)
    {
        normalizedDir.x /= length;
        normalizedDir.y /= length;
    }

    // �Ϲ� �̵� �ӵ� ���
    m_Velocity.x = normalizedDir.x * m_Speed;
    m_Velocity.y = normalizedDir.y * m_Speed;

    // ��ġ ������Ʈ
    const Transform& currentTransform = m_pObjOwner->GetTransform();
    Vector2 newPosition = currentTransform.position;
    newPosition.x += m_Velocity.x * deltaTime;
    newPosition.y += m_Velocity.y * deltaTime;
    m_pObjOwner->SetPosition(newPosition);
}

Vector2 PlayerMovement::CalculateAttackLungeDirection(const Vector2& inputDir)
{
    Vector2 lungeDirection = { 0.0f, 0.0f };

    // �Է� ������ ������ �ش� ��������
    if (inputDir.x != 0.0f || inputDir.y != 0.0f)
    {
        float length = sqrt(inputDir.x * inputDir.x + inputDir.y * inputDir.y);
        lungeDirection.x = inputDir.x / length;
        lungeDirection.y = inputDir.y / length;
    }
    else
    {
        // �Է��� ������ ���콺 ��������
        lungeDirection = GetMouseDirection();
    }

    return lungeDirection;
}

Vector2 PlayerMovement::GetMouseDirection()
{
    // ���콺 ��ġ ��������
    POINT mousePos = InputManager::GetInstance()->GetMousePosition();
    float mouseX = static_cast<float>(mousePos.x);
    float mouseY = static_cast<float>(mousePos.y);

    // ī�޶� ���� ȭ�� ��ǥ�� ���� ��ǥ�� ��ȯ
    Scene* currentScene = SceneManager::GetInstance()->GetCurrentScene();
    Object* cameraObject = currentScene->FindGameObject("MainCamera");
    Camera* camera = cameraObject ? cameraObject->GetComponent<Camera>() : nullptr;

    if (camera)
    {
        float worldMouseX, worldMouseY;
        camera->ScreenToWorld(mouseX, mouseY, worldMouseX, worldMouseY);
        mouseX = worldMouseX;
        mouseY = worldMouseY;
    }

    // �÷��̾��� ���� ���� ��ġ ��������
    auto playerPos = m_pObjOwner->GetTransform().position;
    float playerX = static_cast<float>(playerPos.x);
    float playerY = static_cast<float>(playerPos.y);

    // ���콺 ���� ���
    float deltaX = mouseX - playerX;
    float deltaY = mouseY - playerY;

    float length = sqrt(deltaX * deltaX + deltaY * deltaY);
    if (length > 0.0f)
    {
        return { deltaX / length, deltaY / length };
    }

    // ���콺�� �ʹ� ������ �⺻ ���� (������)
    return { 1.0f, 0.0f };
}

void PlayerMovement::PerformAttackLunge(const Vector2& direction)
{
    // ���� ���� ����
    m_IsAttackLunging = true;
    m_AttackLungeDirection = direction;

    // �޺��� ���� ���� ���� �ð��� �ӵ� ����
    float speedMultiplier = 1.0f;
    float durationMultiplier = 1.0f;

    switch (m_AttackComboCount)
    {
    case 1:
        speedMultiplier = 0.8f;
        durationMultiplier = 0.8f;
        break;  // 1��: �۰� ª�� ����
    case 2:
        speedMultiplier = 1.0f;
        durationMultiplier = 1.0f;
        break;  // 2��: ���� ����
    case 3:
        speedMultiplier = 1.3f;
        durationMultiplier = 1.2f;
        break;  // 3��: ������ �� ����
    }

    // ���� ���� �ӵ��� �ð� ����
    m_AttackLungeDirection.x *= speedMultiplier;
    m_AttackLungeDirection.y *= speedMultiplier;
    m_AttackLungeTimer = m_AttackLungeDuration * durationMultiplier;
}

void PlayerMovement::UpdateAnimationBasedOnMouse(const Vector2& inputDir)
{
    Animation* animation = m_pObjOwner->GetComponent<Animation>();
    if (!animation) return;

    if (m_IsAttacking) return;

    // ���콺 ��ġ ��������
    POINT mousePos = InputManager::GetInstance()->GetMousePosition();
    float mouseX = static_cast<float>(mousePos.x);
    float mouseY = static_cast<float>(mousePos.y);

    // ī�޶� ���� ȭ�� ��ǥ�� ���� ��ǥ�� ��ȯ
    Scene* currentScene = SceneManager::GetInstance()->GetCurrentScene();
    Object* cameraObject = currentScene->FindGameObject("MainCamera");
    Camera* camera = cameraObject->GetComponent<Camera>();

    if (camera)
    {
        float worldMouseX, worldMouseY;
        camera->ScreenToWorld(mouseX, mouseY, worldMouseX, worldMouseY);
        mouseX = worldMouseX;
        mouseY = worldMouseY;
    }

    // �÷��̾��� ���� ���� ��ġ ��������
    auto playerPos = m_pObjOwner->GetTransform().position;
    float playerX = static_cast<float>(playerPos.x);
    float playerY = static_cast<float>(playerPos.y);

    // ���콺�� �÷��̾� ���� Y�� ���� ���
    float deltaX = mouseX - playerX;
    float deltaY = mouseY - playerY;

    // ���콺�� �÷��̾�� ���ʿ� ������ UpperWalk, �Ʒ��ʿ� ������ LowerWalk
    if (deltaY < 0.0f)
    {
        // ���콺�� �÷��̾�� ���� ����
        animation->PlayClip("UpperWalk");
    }
    else
    {
        // ���콺�� �÷��̾�� �Ʒ��� ����
        animation->PlayClip("LowerWalk");
    }

    // ����
    m_pObjOwner->GetComponent<SpriteRenderer>()->SetFlipX(deltaX < 0.0f);

    // �̵� ���� ���� �ִϸ��̼� ��� - ����
    if (inputDir.x != 0.0f || inputDir.y != 0.0f)
    {
        
    }
    else
    {
        // Idle ���� ó�� (�ʿ�� �߰�)
    }
}