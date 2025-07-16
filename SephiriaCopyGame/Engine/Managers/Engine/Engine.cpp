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

// ���� �Ŵ������� Init�� ���
void Engine::Init()
{
	printf("Engine Init\n");

	// Ÿ��, ��ǲ, �����, ���ҽ� �Ŵ��� ��
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

// �ʰ� Init�Ǿ�� �ϴ� �͵�
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

	//MainManager::GetInstance()->Update(DeltaTime); // ���� �Ŵ��� ������Ʈ
}

void Engine::Render(HDC hdc)
{
	//MainManager::GetInstance()->Render(hdc); // ���� �Ŵ��� ������
}
