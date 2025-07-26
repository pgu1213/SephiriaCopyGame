#include <pch.h>
#include "GameScene.h"
#include <Engine/Object/Object/Object.h>
#include <Engine/Object/Component/SpriteRenderer/SpriteRenderer.h>
#include <Engine/Object/Component/Animation/Animation.h>
#include <Engine/Object/Component/Camera/Camera.h>
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
    // GameScene 하이어라키
    // 게임 오브젝트 생성

    Object* player = CreateGameObject("Player");
	player->AddComponent<SpriteRenderer>();
	player->GetComponent<SpriteRenderer>()->SetSprite(L"Player_Basic_Move_Lower00");
	player->GetComponent<SpriteRenderer>()->SetPosition(960.0f, 540.0f);
	player->GetComponent<SpriteRenderer>()->SetSize(64.0f, 64.0f); // 플레이어 크기 설정
	SetupPlayerAnimation(player);
    player->AddComponent<PlayerMovement>();

    Object* playerweapon = CreateGameObject("PlayerWeapon");
    playerweapon->AddComponent<SpriteRenderer>();
    playerweapon->GetComponent<SpriteRenderer>()->SetSprite(L"Weapon_Sword0");
    playerweapon->GetComponent<SpriteRenderer>()->SetPosition(960.0f, 540.0f);
    playerweapon->GetComponent<SpriteRenderer>()->SetSize(32.0f, 32.0f); // 플레이어 크기 설정
    SetupPlayerAnimation(player);
    playerweapon->AddComponent<PlayerWeapon>();

    Object* cube = CreateGameObject("Cube");
    cube->AddComponent<SpriteRenderer>();
    cube->GetComponent<SpriteRenderer>()->SetSprite(L"Duelist_Mole_Airborne00_0");
    cube->GetComponent<SpriteRenderer>()->SetPosition(960.0f, 540.0f);
    cube->GetComponent<SpriteRenderer>()->SetSize(64.0f, 64.0f);

    // 카메라 설정
    Object* camera = CreateGameObject("MainCamera");
    camera->AddComponent<Camera>();

	camera->GetComponent<Camera>()->SetPosition(400.0f, 300.0f);
	camera->GetComponent<Camera>()->SetZoom(1.0f);
	camera->GetComponent<Camera>()->SetViewportSize(800.0f, 600.0f);
	camera->GetComponent<Camera>()->SetBounds(0.0f, 0.0f, 1600.0f, 1200.0f);
	
	//camera->GetComponent<Camera>()->SetMainCamera();
    camera->GetComponent<Camera>()->SetTarget(player);

	camera->GetComponent<Camera>()->SetFollowSpeed(5.0f);
	camera->GetComponent<Camera>()->SetFollowOffset(Vector2(0.0f, -50.0f));
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