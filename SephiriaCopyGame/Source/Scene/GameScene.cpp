#include <pch.h>
#include "GameScene.h"
#include <Engine/Object/Object/Object.h>
#include <Engine/Object/Component/SpriteRenderer/SpriteRenderer.h>
#include <Engine/Object/Component/Animation/Animation.h>
#include <Engine/Object/Component/Camera/Camera.h>
#include <Source/Class/Player/PlayerMovement.h>

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