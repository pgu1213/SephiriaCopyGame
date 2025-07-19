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
    // GameScene �ʱ�ȭ ����
    // ��: �÷��̾�, ��, UI ���� ���� ������Ʈ ����

    // ����: �÷��̾� ������Ʈ ����
    Object* player = CreateGameObject("Player");
	printf("Player Object Created\n");
	player->AddComponent<SpriteRenderer>();
    // �÷��̾ �ʿ��� ������Ʈ �߰� ��...
}