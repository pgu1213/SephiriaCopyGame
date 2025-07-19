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
    // GameScene �ʱ�ȭ ����
    // ��: �÷��̾�, ��, UI ���� ���� ������Ʈ ����

    // ����: �÷��̾� ������Ʈ ����
    Object* player = CreateGameObject("Player");
	player->AddComponent<SpriteRenderer>();
	player->GetComponent<SpriteRenderer>()->SetSprite(L"Player_Basic_Move_Lower00");
	player->GetComponent<SpriteRenderer>()->SetPosition(960.0f, 540.0f);
    player->AddComponent<Animation>();
    vector<wstring> WalkFrames = { L"Player_Basic_Move_Lower00, Player_Basic_Move_Lower01, Player_Basic_Move_Lower02, Player_Basic_Move_Lower03,Player_Basic_Move_Lower04 ,Player_Basic_Move_Lower05, Player_Basic_Move_Lower06, Player_Basic_Move_Lower07" };
    player->GetComponent<Animation>()->AddAnimationClip(L"DownMove", WalkFrames, 0.5f, true);
	player->GetComponent<Animation>()->PlayAnimation(L"DownMove");
    player->AddComponent<PlayerMovement>();
}