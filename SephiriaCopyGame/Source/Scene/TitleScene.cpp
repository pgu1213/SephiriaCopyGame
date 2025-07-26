#include "pch.h"
#include "TitleScene.h"
#include <Engine/Object/Component/SpriteRenderer/SpriteRenderer.h>
#include <Engine/Object/Component/Animation/Animation.h>
#include <Engine/Object/Component/UIButton/UIButton.h>
#include <Engine/Object/Component/UIImage/UIImage.h>
#include <Engine/Object/Component/UIText/UIText.h>
#include <Engine/Object/Component/Camera/Camera.h>
#include <Engine/Managers/SceneManager/SceneManager.h>

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init()
{

    // 카메라 설정
    auto cameraObject = make_unique<Object>("MainCamera");

    // 카메라 컴포넌트 추가
    Camera* camera = cameraObject->AddComponent<Camera>();

    // 카메라 초기 설정
    camera->SetPosition(960.0f, 540.0f);  // 화면 중앙
    camera->SetZoom(1.0f);
    camera->SetViewportSize(800.0f, 600.0f);

    // 카메라 경계 설정 (월드 크기 0~1600, 0~1200)
    camera->SetBounds(0.0f, 0.0f, 1600.0f, 1200.0f);

    // 메인 카메라로 설정
    Camera::SetMainCamera(camera);

    // 플레이어 따라가기 설정
    //Object* player = FindPlayerObject(); // 플레이어 객체 찾기
    //if (player)
    //{
    //    camera->SetTarget(player);
    //    camera->SetFollowSpeed(5.0f);
    //    camera->SetFollowOffset(0.0f, -50.0f); // 플레이어보다 약간 위쪽
    //}

    SetupUI();
}

void TitleScene::SetupUI()
{
    // 백그라운드
    UI* background = CreateUI("Background");
    background->AddComponent<UIImage>();

    background->GetComponent<UIImage>()->SetSprite(L"N_Title_Back");
	background->SetPosition(960.0f, 540.0f); // 중앙으로
	background->SetSize(1920, 1080.0f); // 배경 크기 설정

    // 타이틀
    UI* title = CreateUI("Title");
    title->AddComponent<UIImage>();

    title->GetComponent<UIImage>()->SetSprite(L"N_Title");
    title->SetPosition(960.0f, 540.0f);
    title->SetSize(711.0f, 228.0f);

    // 스타트 버튼
    UI* startbutton = CreateUI("StartButton");
	startbutton->AddComponent<UIButton>();

    startbutton->GetComponent<UIButton>()->SetText("Start");
    startbutton->GetComponent<UIButton>()->SetFont(L"Arial", 24, true);
    startbutton->GetComponent<UIButton>()->SetTextColor(RGB(255, 255, 255));
    startbutton->GetComponent<UIButton>()->SetColors(RGB(255, 255, 255), RGB(100, 100, 100), RGB(200, 200, 200), RGB(150, 150, 150));
    startbutton->SetPosition(810.0f, 700.0f);
    startbutton->SetSize(200.0f, 50.0f);
    // 클릭 이벤트 설정
    startbutton->m_OnClick = []() {
        SceneManager::GetInstance()->SceneLoad("GameScene");
		};

    //종료 버튼
    UI* endbutton = CreateUI("EndButton");
    endbutton->AddComponent<UIButton>();

    endbutton->GetComponent<UIButton>()->SetText("End");
    endbutton->GetComponent<UIButton>()->SetFont(L"Arial", 24, true);
    endbutton->GetComponent<UIButton>()->SetTextColor(RGB(255, 255, 255));
    endbutton->GetComponent<UIButton>()->SetColors(RGB(255, 255, 255), RGB(100, 100, 100), RGB(200, 200, 200), RGB(150, 150, 150));
    endbutton->SetPosition(1110.0f, 700.0f);
    endbutton->SetSize(200.0f, 50.0f);
    // 클릭 이벤트 설정
    endbutton->m_OnClick = []() {
		PostQuitMessage(0);
        };
}