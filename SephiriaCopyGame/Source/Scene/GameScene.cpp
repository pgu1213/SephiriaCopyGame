#include <pch.h>
#include "GameScene.h"
#include <Engine/Object/Object/Object.h>
#include <Engine/Object/Component/SpriteRenderer/SpriteRenderer.h>
#include <Engine/Object/Component/Animation/Animation.h>
#include <Engine/Object/Component/Camera/Camera.h>
#include <Engine/Object/Component/Collider/BoxCollider.h>
#include <Engine/Object/Component/Collider/CircleCollider.h>
#include <Source/Class/Player/PlayerMovement.h>
#include <Source/Class/Player/PlayerWeapon.h>

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

void GameScene::Init()
{
    // GameScene ���̾��Ű
    // ���� ������Ʈ ����

    // ī�޶� ����
    Object* camera = CreateGameObject("MainCamera");
    Camera* cameraComponent = camera->AddComponent<Camera>();
    cameraComponent->SetPosition(0.0f, 0.0f);
    cameraComponent->SetZoom(1.0f);
    cameraComponent->SetViewportSize(WINCX, WINCY);
    Camera::SetMainCamera(cameraComponent);

	// �÷��̾� ����
    Object* player = CreateGameObject("Player");
	player->AddComponent<SpriteRenderer>();
	player->GetComponent<SpriteRenderer>()->SetSprite(L"Player_Basic_Move_Lower00");
	player->GetComponent<SpriteRenderer>()->SetSize(64.0f, 64.0f); // �÷��̾� ũ�� ����
	SetupPlayerAnimation(player);
    player->AddComponent<PlayerMovement>();
	player->AddComponent<BoxCollider>();
	player->GetComponent<BoxCollider>()->SetSize(Vector2(64.0f, 64.0f));
	player->GetComponent<BoxCollider>()->SetLayer(CollisionLayer::Player);
	player->GetComponent<BoxCollider>()->SetLayerMask(CollisionLayer::All);
	player->GetComponent<BoxCollider>()->SetIsTrigger(false);

    Object* playerweapon = CreateGameObject("PlayerWeapon");
    playerweapon->AddComponent<SpriteRenderer>();
    playerweapon->GetComponent<SpriteRenderer>()->SetSprite(L"Weapon_Sword0");
    playerweapon->GetComponent<SpriteRenderer>()->SetSize(32.0f, 32.0f); // �÷��̾� ũ�� ����
    playerweapon->AddComponent<PlayerWeapon>();

    //�׽�Ʈ��
    Object* cube = CreateGameObject("Cube");
	cube->SetPosition(100.0f, 100.0f);
    cube->AddComponent<SpriteRenderer>();
    cube->GetComponent<SpriteRenderer>()->SetSprite(L"Duelist_Mole_Airborne00_0");
    cube->GetComponent<SpriteRenderer>()->SetSize(64.0f, 64.0f);
    cube->AddComponent<BoxCollider>();
    cube->GetComponent<BoxCollider>()->SetSize(Vector2(64.0f, 64.0f));
    cube->GetComponent<BoxCollider>()->SetLayer(CollisionLayer::Enemy);
    cube->GetComponent<BoxCollider>()->SetLayerMask(CollisionLayer::All);
    cube->GetComponent<BoxCollider>()->SetIsTrigger(false);

    // ī�޶� ����
    cameraComponent->SetTarget(player);
    cameraComponent->SetFollowSpeed(5.0f);
    cameraComponent->SetFollowOffset(Vector2(0.0f, -50.0f));
}

void GameScene::SetupPlayerAnimation(Object* player)
{
    Animation* animation = player->AddComponent<Animation>();

    AnimationClip LowerWalkClip("LowerWalk", true);
    LowerWalkClip.frames.push_back(AnimationFrame(L"Player_Basic_Move_Lower00", 0.2f));
    LowerWalkClip.frames.push_back(AnimationFrame(L"Player_Basic_Move_Lower01", 0.2f));
    LowerWalkClip.frames.push_back(AnimationFrame(L"Player_Basic_Move_Lower02", 0.2f));
    LowerWalkClip.frames.push_back(AnimationFrame(L"Player_Basic_Move_Lower03", 0.2f));
    LowerWalkClip.frames.push_back(AnimationFrame(L"Player_Basic_Move_Lower04", 0.2f));
    LowerWalkClip.frames.push_back(AnimationFrame(L"Player_Basic_Move_Lower05", 0.2f));
    LowerWalkClip.frames.push_back(AnimationFrame(L"Player_Basic_Move_Lower06", 0.2f));
    LowerWalkClip.frames.push_back(AnimationFrame(L"Player_Basic_Move_Lower07", 0.2f));
    animation->AddClip(LowerWalkClip);

    AnimationClip UpperWalkClip("UpperWalk", true);
    UpperWalkClip.frames.push_back(AnimationFrame(L"Player_Basic_Move_Upper00", 0.2f));
    UpperWalkClip.frames.push_back(AnimationFrame(L"Player_Basic_Move_Upper01", 0.2f));
    UpperWalkClip.frames.push_back(AnimationFrame(L"Player_Basic_Move_Upper02", 0.2f));
    UpperWalkClip.frames.push_back(AnimationFrame(L"Player_Basic_Move_Upper03", 0.2f));
    UpperWalkClip.frames.push_back(AnimationFrame(L"Player_Basic_Move_Upper04", 0.2f));
    UpperWalkClip.frames.push_back(AnimationFrame(L"Player_Basic_Move_Upper05", 0.2f));
    UpperWalkClip.frames.push_back(AnimationFrame(L"Player_Basic_Move_Upper06", 0.2f));
    UpperWalkClip.frames.push_back(AnimationFrame(L"Player_Basic_Move_Upper07", 0.2f));
    animation->AddClip(UpperWalkClip);

    animation->PlayClip("LowerWalk");
}