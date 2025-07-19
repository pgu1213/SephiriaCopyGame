#include <pch.h>
#include "GameScene.h"
#include <Engine/Object/Object/Object.h>
#include <Engine/Object/Component/SpriteRenderer/SpriteRenderer.h>

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

void GameScene::Init()
{
    // GameScene 초기화 로직
    // 예: 플레이어, 적, UI 등의 게임 오브젝트 생성

    // 예시: 플레이어 오브젝트 생성
    Object* player = CreateGameObject("Player");
	printf("Player Object Created\n");
	player->AddComponent<SpriteRenderer>();
    // 플레이어에 필요한 컴포넌트 추가 등...
}