#include <pch.h>
#include "SceneManager.h"
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
	// GameScene 등록 (필요에 따라 추가 씬들을 여기에 등록)
	RegisterScene("GameScene", make_unique<GameScene>());

	// 기본 씬을 GameScene으로 설정
	SceneLoad("GameScene");
}

void SceneManager::Update(float DeltaTime)
{
	if (m_CurrentScene)
	{
		m_CurrentScene->Update(DeltaTime);
	}
}

bool SceneManager::SceneLoad(const string& sceneName)
{
	// 해당 이름의 씬이 존재하는지 확인
	auto it = m_Scenes.find(sceneName);
	if (it == m_Scenes.end())
	{
		// 씬이 존재하지 않음
		return false;
	}

	// 현재 씬이 있다면 정리
	if (m_CurrentScene && m_CurrentSceneName != sceneName)
	{
		ClearCurrentScene();
	}

	// 새로운 씬으로 변경
	m_CurrentScene = it->second.get();
	m_CurrentSceneName = sceneName;

	return true;
}

void SceneManager::RegisterScene(const string& sceneName, unique_ptr<Scene> scene)
{
	if (scene)
	{
		m_Scenes[sceneName] = move(scene);
	}
}

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