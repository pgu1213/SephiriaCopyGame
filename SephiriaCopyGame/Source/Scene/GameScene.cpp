#include <pch.h>
#include "GameScene.h"
#include <Engine/Managers/SceneManager/SceneManager.h>
#include <Engine/Object/Object/Object.h>
#include <Engine/Object/Component/SpriteRenderer/SpriteRenderer.h>
#include <Engine/Object/Component/Animation/Animation.h>
#include <Engine/Object/Component/Camera/Camera.h>
#include <Engine/Object/Component/Collider/BoxCollider.h>
#include <Engine/Object/Component/Collider/CircleCollider.h>
#include <Source/Class/Dungeon/DungeonGenerator.h>
#include <Engine/Object/Component/UIButton/UIButton.h>
#include <Source/Class/Player/PlayerMovement.h>
#include <Source/Class/Player/PlayerWeapon.h>
#include <Source/Class/Player/PlayerFX.h>

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
    cameraComponent->SetZoom(2.0f);
    cameraComponent->SetViewportSize(WINCX, WINCY);
    Camera::SetMainCamera(cameraComponent);

    Object* dungeon = CreateGameObject("Dungeon");
	dungeon->AddComponent<DungeonGenerator>();
    dungeon->GetComponent<DungeonGenerator>()->SetTileSize(64.0f); // Ÿ�� ũ�� ����
    dungeon->GetComponent<DungeonGenerator>()->LoadMapFromFile(L"Resource/MapFile/map_20250727_061249.txt"); // �� ���� �ε�
	dungeon->GetComponent<DungeonGenerator>()->GenerateDungeon(); // ���� ����

    // ���� ���� (�÷��̾� �ڿ� ����������)
    Object* playerweapon = CreateGameObject("PlayerWeapon");
    playerweapon->AddComponent<SpriteRenderer>();
    playerweapon->GetComponent<SpriteRenderer>()->SetSprite(L"Weapon_Sword0");
    playerweapon->GetComponent<SpriteRenderer>()->SetSize(27.0f, 51.0f); // �÷��̾� ũ�� ����
	playerweapon->GetComponent<SpriteRenderer>()->SetAnchor(0.5f, 0.7f);

	// �÷��̾� ����
    Object* player = CreateGameObject("Player");
	player->SetPosition(500.0f, 500.0f);
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

	// �÷��̾�� ���� ����
    playerweapon->AddComponent<PlayerWeapon>();

    // �÷��̾� ����Ʈ
    Object* playerFX = CreateGameObject("PlayerSwingFX");
    playerFX->AddComponent<SpriteRenderer>();
    playerFX->GetComponent<SpriteRenderer>()->SetSprite(L"Sword0_Swing0");
    playerFX->GetComponent<SpriteRenderer>()->SetSize(64.0f, 64.0f);
    playerFX->AddComponent<PlayerFX>();
	playerFX->AddComponent<BoxCollider>();
    playerFX->GetComponent<BoxCollider>()->SetSize(Vector2(64.0f, 64.0f));
	playerFX->GetComponent<BoxCollider>()->SetLayer(CollisionLayer::Player);
	playerFX->GetComponent<BoxCollider>()->SetLayerMask(CollisionLayer::All);
	playerFX->GetComponent<BoxCollider>()->SetIsTrigger(true);
    Animation* SwingFXanim = playerFX->AddComponent<Animation>();

    AnimationClip SwingFX1("SwingFX1", false);
    SwingFX1.frames.push_back(AnimationFrame(L"Sword0_Swing0", 0.3f));
    SwingFX1.frames.push_back(AnimationFrame(L"Sword0_Swing1", 0.3f));
    SwingFXanim->AddClip(SwingFX1);

    AnimationClip SwingFX2("SwingFX2", false);
    SwingFX2.frames.push_back(AnimationFrame(L"Sword0_Swing2_0", 0.3f));
    SwingFX2.frames.push_back(AnimationFrame(L"Sword0_Swing2_1", 0.3f));
    SwingFXanim->AddClip(SwingFX2);

	playerFX->SetActive(false);

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