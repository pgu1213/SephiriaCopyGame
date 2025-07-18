#pragma once
#include <Engine/Managers/SingletonManager/SingletonManager.h>
#include <Engine/Scene/Scene.h>

class SceneManager : public SingleTon<SceneManager>
{
	friend class SingleTon<SceneManager>;
private:
	explicit SceneManager();
public:
	virtual ~SceneManager();
public:
	void Init();
	void Update();
	bool SceneLoad(const string& sceneName);
	void RegisterScene(const string& sceneName, unique_ptr<Scene> scene);
	Scene* GetCurrentScene() const { return m_CurrentScene; }

private:
	void ClearCurrentScene();

private:
	map<string, unique_ptr<Scene>> m_Scenes;
	Scene* m_CurrentScene;
	string m_CurrentSceneName;
};