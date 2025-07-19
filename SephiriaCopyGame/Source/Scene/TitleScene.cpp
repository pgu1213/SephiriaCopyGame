#include "pch.h"
#include "TitleScene.h"

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init()
{
    // TitleScene 초기화 로직
    // 예: 배경 이미지, 타이틀 텍스트, 버튼 등 UI 오브젝트 생성
    // 예시: 배경 오브젝트 생성
    Object* background = CreateGameObject("Background");
    //background->AddComponent<SpriteRenderer>()->SetTexture(L"BackgroundImage.png");
    // 예시: 타이틀 텍스트 오브젝트 생성
    Object* titleText = CreateGameObject("TitleText");
    //titleText->AddComponent<TextRenderer>()->SetText(L"Welcome to Sephiria Copy Game");
    
    // 예시: 시작 버튼 오브젝트 생성
    Object* startButton = CreateGameObject("StartButton");
	//startButton->AddComponent<Button>()->SetText(L"Start Game");
}
