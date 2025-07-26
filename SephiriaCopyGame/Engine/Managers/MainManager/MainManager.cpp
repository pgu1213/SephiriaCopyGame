#include "pch.h"
#include "MainManager.h"
#include <Engine/Managers/SceneManager/SceneManager.h>


MainManager::MainManager()
{
}

MainManager::~MainManager()
{
}

// 하위 매니저들의 Init을 담당
void MainManager::Init()
{
	SceneManager::GetInstance()->Init();

	printf("MainManager Init Complete\n");
}

// 늦게 Init되어야 하는 것들
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
