#pragma once
#include "../SingletonManager/SingletonManager.h"

class InputManager : public SingleTon<InputManager>
{
    friend class SingleTon<InputManager>;
private:
    explicit InputManager();
public:
    virtual ~InputManager();
public:
    void Update();
    bool IsKeyPressed(int key);
    bool IsKeyDown(int key);
    bool IsKeyUp(int key);
    POINT GetMousePosition();
private:
    bool m_KeyStates[256];
    bool m_PrevKeyStates[256];
    POINT m_MousePos;
};

