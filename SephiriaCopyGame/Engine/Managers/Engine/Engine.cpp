#include <pch.h>
#include "Engine.h"
#include <Engine/Managers/TimeManager/TimeManager.h>
#include <Engine/Managers/InputManager/InputManager.h>
#include <Engine/Managers/ResourceManager/ResourceManager.h>


Engine::Engine()
{
}

Engine::~Engine()
{
}

// 하위 매니저들의 Init을 담당
void Engine::Init()
{
	printf("Engine Init\n");

	// 타임, 인풋, 오디오, 리소스 매니저 등
	if (!TimeManager::GetInstance()->Init())
	{
		return;
	}

	if (!ResourceManager::GetInstance()->Init())
	{
		return;
	}

	/*if (!SoundManager::GetInstance()->Init())
	{
		return;
	}*/

	//MainManager::GetInstance()->Init();

	printf("Engine Init Complete\n");
}

// 늦게 Init되어야 하는 것들
void Engine::LateInit()
{
	printf("Engine LateInit\n");

	printf("Engine LateInit Complete\n");
}

void Engine::Update()
{
	TimeManager::GetInstance()->Update();

	float DeltaTime = TimeManager::GetInstance()->GetDeltaTime();

	InputManager::GetInstance()->Update();

	//MainManager::GetInstance()->Update(DeltaTime); // 게임 매니저 업데이트
}

void Engine::Render(HDC hdc)
{
	//MainManager::GetInstance()->Render(hdc); // 게임 매니저 렌더링
}
