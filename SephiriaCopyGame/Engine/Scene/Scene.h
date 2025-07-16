#pragma once
#include <Engine/Object/Object/Object.h>

class Object;

class Scene
{
public:
    Object* CreateGameObject(const string& name = "GameObject");
    void DestroyGameObject(Object* obj);
    void Update();
private:
    vector<unique_ptr<Object>> m_GameObjects;
    queue<unique_ptr<Object>> m_ObjectsToAdd;
    vector<Object*> m_ObjectsToDestroy;
};

