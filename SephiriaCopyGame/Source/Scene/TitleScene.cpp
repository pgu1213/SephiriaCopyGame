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
    // TitleScene �ʱ�ȭ ����
    // ��: ��� �̹���, Ÿ��Ʋ �ؽ�Ʈ, ��ư �� UI ������Ʈ ����
    // ����: ��� ������Ʈ ����
    Object* background = CreateGameObject("Background");
    //background->AddComponent<SpriteRenderer>()->SetTexture(L"BackgroundImage.png");
    // ����: Ÿ��Ʋ �ؽ�Ʈ ������Ʈ ����
    Object* titleText = CreateGameObject("TitleText");
    //titleText->AddComponent<TextRenderer>()->SetText(L"Welcome to Sephiria Copy Game");
    
    // ����: ���� ��ư ������Ʈ ����
    Object* startButton = CreateGameObject("StartButton");
	//startButton->AddComponent<Button>()->SetText(L"Start Game");
}
