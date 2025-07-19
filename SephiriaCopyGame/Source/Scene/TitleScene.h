#pragma once
#include <Engine/Scene/Scene.h>

class TitleScene : public Scene
{
public:
    TitleScene();
    virtual ~TitleScene();

public:
    void Init() override;
	void SetupUI(); // UI 설정 함수
};