#pragma once
#include <Engine/Scene/Scene.h>

class GameScene : public Scene
{
public:
    GameScene();
    virtual ~GameScene();

public:
    void Init() override;
};