#include <pch.h>
#include "SceneManager.h"
#include <Source/Scene/TitleScene.h>
#include <Source/Scene/GameScene.h>

SceneManager::SceneManager() : m_CurrentScene(nullptr), m_CurrentSceneName("")
{
}

SceneManager::~SceneManager()
{
	ClearCurrentScene();
}

void SceneManager::Init()
{
	// ��� ������ �ε��ϰ� ���
	RegisterScene("TitleScene", make_unique<TitleScene>());
	RegisterScene("GameScene", make_unique<GameScene>());

	// �⺻ �� ����
	SceneLoad("TitleScene");

	printf("SceneManager Init Complete\n");
}

void SceneManager::Update(float DeltaTime)
{
	if (m_CurrentScene)
	{
		m_CurrentScene->Update(DeltaTime);
	}
}

void SceneManager::Render(HDC hdc)
{
	if (m_CurrentScene)
	{
		m_CurrentScene->Render(hdc);
	}
}

bool SceneManager::SceneLoad(const string& sceneName)
{
	// �̸��� ���� �����ϴ��� Ȯ��
	auto it = m_Scenes.find(sceneName);
	if (it == m_Scenes.end())
	{
		// �̸��� ���� ������
		MessageBoxA(nullptr, ("Scene '" + sceneName + "' not found!").c_str(), "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// ���� �������� ���� �ִٸ� ����
	if (m_CurrentScene && m_CurrentSceneName != sceneName)
	{
		ClearCurrentScene();

		m_CurrentScene = nullptr;
	}

	// ���ο� ������ ����
	m_CurrentScene = it->second.get();
	m_CurrentSceneName = sceneName;

	if (m_CurrentScene)
	{
		m_CurrentScene->Init();
	}

	return true;
}

void SceneManager::RegisterScene(const string& sceneName, unique_ptr<Scene> scene)
{
	if (scene)
	{
		m_Scenes[sceneName] = move(scene);
	}
}

// ���� �� ����
void SceneManager::ClearCurrentScene()
{
	if (m_CurrentScene)
	{
		// ���� ���� ��� ������Ʈ ����
		m_CurrentScene->Clear();
		m_CurrentScene = nullptr;
		m_CurrentSceneName = "";
	}
}