#include <pch.h>
#include "GameScene.h"

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
    // �÷��̾ �ʿ��� ������Ʈ �߰� ��...
}