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
	// �� ��ȯ ��û�� ������ ó��
	if (m_bisSceneChangeRequested)
	{
		ProcessSceneChange();
		m_bisSceneChangeRequested = false;
	}

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
	m_NextSceneName = sceneName;
	m_bisSceneChangeRequested = true;
	return true;
}

void SceneManager::RegisterScene(const string& sceneName, unique_ptr<Scene> scene)
{
	if (scene)
	{
		m_Scenes[sceneName] = move(scene);
	}
}

void SceneManager::ProcessSceneChange()
{
	// �̸��� ���� �����ϴ��� Ȯ��
	auto it = m_Scenes.find(m_NextSceneName);
	if (it == m_Scenes.end())
	{
		// �̸��� ���� ������
		MessageBoxA(nullptr, ("Scene '" + m_NextSceneName + "' not found!").c_str(), "Error", MB_OK | MB_ICONERROR);
	}

	// ���� �������� ���� �ִٸ� ����
	if (m_CurrentScene && m_CurrentSceneName != m_NextSceneName)
	{
		ClearCurrentScene();

		m_CurrentScene = nullptr;
	}

	// ���ο� ������ ����
	m_CurrentScene = it->second.get();
	m_CurrentSceneName = m_NextSceneName;

	if (m_CurrentScene)
	{
		m_CurrentScene->Init();
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