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
	// 모든 씬들을 로드하고 등록
	RegisterScene("TitleScene", make_unique<TitleScene>());
	RegisterScene("GameScene", make_unique<GameScene>());

	// 기본 씬 설정
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
	// 이름의 씬이 존재하는지 확인
	auto it = m_Scenes.find(sceneName);
	if (it == m_Scenes.end())
	{
		// 이름의 씬이 없으면
		MessageBoxA(nullptr, ("Scene '" + sceneName + "' not found!").c_str(), "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// 현재 진행중인 씬이 있다면 정리
	if (m_CurrentScene && m_CurrentSceneName != sceneName)
	{
		ClearCurrentScene();

		m_CurrentScene = nullptr;
	}

	// 새로운 씬으로 변경
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

// 현재 씬 정리
void SceneManager::ClearCurrentScene()
{
	if (m_CurrentScene)
	{
		// 현재 씬의 모든 오브젝트 정리
		m_CurrentScene->Clear();
		m_CurrentScene = nullptr;
		m_CurrentSceneName = "";
	}
}