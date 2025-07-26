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
    , m_Speed(200.0f)        // 기본 이동 속도
    , m_Velocity({ 0.0f, 0.0f })
    , m_DashSpeed(600.0f)    // 대쉬 속도
    , m_DashDuration(0.2f)   // 대쉬 지속 시간
    , m_DashCooldown(1.0f)   // 대쉬 쿨다운
    , m_DashTimer(0.0f)
    , m_DashCooldownTimer(0.0f)
    , m_IsDashing(false)
    , m_DashDirection({ 0.0f, 0.0f })
    , m_AttackComboCount(0)      // 공격 콤보 카운트
    , m_AttackComboMaxTime(1.0f) // 콤보 유지 시간
    , m_AttackComboTimer(0.0f)   // 콤보 타이머
    , m_IsAttacking(false)       // 공격 중 여부
    , m_AttackDuration(0.6f)     // 공격 지속 시간
    , m_AttackTimer(0.2f)        // 공격 타이머
    , m_AttackLungeSpeed(400.0f) // 공격 시 돌진 속도
    , m_AttackLungeDuration(0.15f) // 공격 시 돌진 지속 시간
    , m_AttackLungeTimer(0.0f)     // 공격 돌진 타이머
    , m_IsAttackLunging(false)     // 공격 돌진 중 여부
    , m_AttackLungeDirection({ 0.0f, 0.0f }) // 공격 돌진 방향
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

    // 공격 돌진 타이머 업데이트
    if (m_IsAttackLunging)
    {
        m_AttackLungeTimer -= deltaTime;
        if (m_AttackLungeTimer <= 0.0f)
        {
            m_IsAttackLunging = false;
        }
    }

    // 공격 콤보 타이머 업데이트
    if (m_AttackComboTimer > 0.0f)
    {
        m_AttackComboTimer -= deltaTime;
        if (m_AttackComboTimer <= 0.0f)
        {
            // 콤보 시간 초과 시 리셋
            m_AttackComboCount = 0;
        }
    }

    // 공격 타이머 업데이트
    if (m_IsAttacking)
    {
        m_AttackTimer -= deltaTime;
        if (m_AttackTimer <= 0.0f)
        {
            m_IsAttacking = false;
        }
    }

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

            // 대쉬 중에도 공격 가능하므로 return 하지 않음
        }
    }

    // 공격 돌진 중일 때
    if (m_IsAttackLunging)
    {
        // 공격 돌진 이동 처리
        m_Velocity.x = m_AttackLungeDirection.x * m_AttackLungeSpeed;
        m_Velocity.y = m_AttackLungeDirection.y * m_AttackLungeSpeed;

        // 위치 업데이트
        const Transform& currentTransform = m_pObjOwner->GetTransform();
        Vector2 newPosition = currentTransform.position;
        newPosition.x += m_Velocity.x * deltaTime;
        newPosition.y += m_Velocity.y * deltaTime;
        m_pObjOwner->SetPosition(newPosition);
    }

    // 일반 이동 입력 처리
    Vector2 inputDir = { 0.0f, 0.0f };

    // 공격 중이 아닐 때만 이동 입력 받기
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

    // 공격 입력 처리 (마우스 좌클릭)
    if (input->IsKeyDown(VK_LBUTTON))
    {
        HandleAttack(inputDir);
    }

    // 대쉬 입력 처리 (스페이스바) - 공격 중에도 가능
    if (input->IsKeyDown(VK_SPACE) && m_DashCooldownTimer <= 0.0f && !m_IsDashing)
    {
        HandleDash(inputDir);
    }

    Scene* currentScene = SceneManager::GetInstance()->GetCurrentScene();
    Object* FXObject = currentScene->FindGameObject("PlayerSwingFX");

    // 공격 중이 아니고 대쉬/공격돌진 중이 아닐 때만 일반 이동과 애니메이션 처리
    if (!m_IsAttacking && !m_IsDashing && !m_IsAttackLunging)
    {
        FXObject->SetActive(false);
        UpdateAnimationBasedOnMouse(inputDir);
        HandleMovement(inputDir, deltaTime);
    }
    else if (m_IsAttacking && !m_IsDashing && !m_IsAttackLunging)
    {
        FXObject->SetActive(true);
        // 공격 중일 때는 애니메이션만 업데이트 (이동 방향 업데이트용)
        UpdateAnimationBasedOnMouse(Vector2::Zero);
    }
}

void PlayerMovement::HandleAttack(const Vector2& inputDir)
{
    Scene* currentScene = SceneManager::GetInstance()->GetCurrentScene();
    Object* FXObject = currentScene->FindGameObject("PlayerSwingFX");
    Animation* animation = FXObject->GetComponent<Animation>();
    if (!animation) return;

    // 콤보 카운트 증가
    m_AttackComboCount++;
    if (m_AttackComboCount > 3)
    {
        m_AttackComboCount = 1; // 3단 콤보 후 다시 1단으로
    }

    // 공격 상태 설정
    m_IsAttacking = true;
    m_AttackTimer = m_AttackDuration;
    m_AttackComboTimer = m_AttackComboMaxTime;

    // 콤보에 따른 애니메이션 재생
    string attackAnimName = "SwingFX" + to_string(m_AttackComboCount);
    animation->PlayClip(attackAnimName);

    // 공격 시 돌진 처리
    Vector2 lungeDirection = CalculateAttackLungeDirection(inputDir);
    PerformAttackLunge(lungeDirection);
}

void PlayerMovement::HandleDash(const Vector2& inputDir)
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
}

void PlayerMovement::HandleMovement(const Vector2& inputDir, float deltaTime)
{
    // 벡터 정규화
    Vector2 normalizedDir = inputDir;
    float length = sqrt(inputDir.x * inputDir.x + inputDir.y * inputDir.y);
    if (length > 0.0f)
    {
        normalizedDir.x /= length;
        normalizedDir.y /= length;
    }

    // 일반 이동 속도 계산
    m_Velocity.x = normalizedDir.x * m_Speed;
    m_Velocity.y = normalizedDir.y * m_Speed;

    // 위치 업데이트
    const Transform& currentTransform = m_pObjOwner->GetTransform();
    Vector2 newPosition = currentTransform.position;
    newPosition.x += m_Velocity.x * deltaTime;
    newPosition.y += m_Velocity.y * deltaTime;
    m_pObjOwner->SetPosition(newPosition);
}

Vector2 PlayerMovement::CalculateAttackLungeDirection(const Vector2& inputDir)
{
    Vector2 lungeDirection = { 0.0f, 0.0f };

    // 입력 방향이 있으면 해당 방향으로
    if (inputDir.x != 0.0f || inputDir.y != 0.0f)
    {
        float length = sqrt(inputDir.x * inputDir.x + inputDir.y * inputDir.y);
        lungeDirection.x = inputDir.x / length;
        lungeDirection.y = inputDir.y / length;
    }
    else
    {
        // 입력이 없으면 마우스 방향으로
        lungeDirection = GetMouseDirection();
    }

    return lungeDirection;
}

Vector2 PlayerMovement::GetMouseDirection()
{
    // 마우스 위치 가져오기
    POINT mousePos = InputManager::GetInstance()->GetMousePosition();
    float mouseX = static_cast<float>(mousePos.x);
    float mouseY = static_cast<float>(mousePos.y);

    // 카메라를 통해 화면 좌표를 월드 좌표로 변환
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

    // 플레이어의 현재 월드 위치 가져오기
    auto playerPos = m_pObjOwner->GetTransform().position;
    float playerX = static_cast<float>(playerPos.x);
    float playerY = static_cast<float>(playerPos.y);

    // 마우스 방향 계산
    float deltaX = mouseX - playerX;
    float deltaY = mouseY - playerY;

    float length = sqrt(deltaX * deltaX + deltaY * deltaY);
    if (length > 0.0f)
    {
        return { deltaX / length, deltaY / length };
    }

    // 마우스가 너무 가까우면 기본 방향 (오른쪽)
    return { 1.0f, 0.0f };
}

void PlayerMovement::PerformAttackLunge(const Vector2& direction)
{
    // 공격 돌진 시작
    m_IsAttackLunging = true;
    m_AttackLungeDirection = direction;

    // 콤보에 따라 돌진 지속 시간과 속도 조정
    float speedMultiplier = 1.0f;
    float durationMultiplier = 1.0f;

    switch (m_AttackComboCount)
    {
    case 1:
        speedMultiplier = 0.8f;
        durationMultiplier = 0.8f;
        break;  // 1단: 작고 짧은 돌진
    case 2:
        speedMultiplier = 1.0f;
        durationMultiplier = 1.0f;
        break;  // 2단: 보통 돌진
    case 3:
        speedMultiplier = 1.3f;
        durationMultiplier = 1.2f;
        break;  // 3단: 빠르고 긴 돌진
    }

    // 실제 돌진 속도와 시간 설정
    m_AttackLungeDirection.x *= speedMultiplier;
    m_AttackLungeDirection.y *= speedMultiplier;
    m_AttackLungeTimer = m_AttackLungeDuration * durationMultiplier;
}

void PlayerMovement::UpdateAnimationBasedOnMouse(const Vector2& inputDir)
{
    Animation* animation = m_pObjOwner->GetComponent<Animation>();
    if (!animation) return;

    if (m_IsAttacking) return;

    // 마우스 위치 가져오기
    POINT mousePos = InputManager::GetInstance()->GetMousePosition();
    float mouseX = static_cast<float>(mousePos.x);
    float mouseY = static_cast<float>(mousePos.y);

    // 카메라를 통해 화면 좌표를 월드 좌표로 변환
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

    // 플레이어의 현재 월드 위치 가져오기
    auto playerPos = m_pObjOwner->GetTransform().position;
    float playerX = static_cast<float>(playerPos.x);
    float playerY = static_cast<float>(playerPos.y);

    // 마우스와 플레이어 간의 Y축 차이 계산
    float deltaX = mouseX - playerX;
    float deltaY = mouseY - playerY;

    // 마우스가 플레이어보다 위쪽에 있으면 UpperWalk, 아래쪽에 있으면 LowerWalk
    if (deltaY < 0.0f)
    {
        // 마우스가 플레이어보다 위에 있음
        animation->PlayClip("UpperWalk");
    }
    else
    {
        // 마우스가 플레이어보다 아래에 있음
        animation->PlayClip("LowerWalk");
    }

    // 반전
    m_pObjOwner->GetComponent<SpriteRenderer>()->SetFlipX(deltaX < 0.0f);

    // 이동 중일 때만 애니메이션 재생 - 추후
    if (inputDir.x != 0.0f || inputDir.y != 0.0f)
    {
        
    }
    else
    {
        // Idle 상태 처리 (필요시 추가)
    }
}