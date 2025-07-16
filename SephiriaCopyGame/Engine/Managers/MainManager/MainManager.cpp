#include "../../../pch.h"
#include "MainManager.h"
#include <Engine/Managers/TimeManager/TimeManager.h>
#include <Engine/Managers/ResourceManager/ResourceManager.h>


MainManager::MainManager()
{
}

MainManager::~MainManager()
{
}

// 하위 매니저들의 Init을 담당
void MainManager::Init()
{

}

// 늦게 Init되어야 하는 것들
void MainManager::LateInit()
{

}

void MainManager::Update(float DeltaTime)
{

}

void MainManager::Render(HDC hdc)
{

}
