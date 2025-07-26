#include "pch.h"
#include "MainManager.h"
#include <Engine/Managers/SceneManager/SceneManager.h>


MainManager::MainManager()
{
}

MainManager::~MainManager()
{
}

// ���� �Ŵ������� Init�� ���
void MainManager::Init()
{
	SceneManager::GetInstance()->Init();

	printf("MainManager Init Complete\n");
}

// �ʰ� Init�Ǿ�� �ϴ� �͵�
void MainManager::LateInit()
{

}

void MainManager::Update(float DeltaTime)
{
	SceneManager::GetInstance()->Update(DeltaTime);
}

void MainManager::Render(HDC hdc)
{
	SceneManager::GetInstance()->Render(hdc);

}
