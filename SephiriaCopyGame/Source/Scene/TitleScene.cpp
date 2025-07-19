#include "pch.h"
#include "TitleScene.h"
#include <Engine/Managers/UIManager/UIManager.h>
#include <Engine/Object/Component/SpriteRenderer/SpriteRenderer.h>
#include <Engine/Object/Component/Animation/Animation.h>
#include <Engine/Object/UI/Button/UIButton.h>
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
    Object* background = CreateGameObject("Background");
    background->AddComponent<SpriteRenderer>();

	background->GetComponent<SpriteRenderer>()->SetSprite(L"N_Title_Back");
	background->GetComponent<SpriteRenderer>()->SetPosition(960.0f, 540.0f); // 중앙으로
	background->GetComponent<SpriteRenderer>()->SetSize(1920.0f, 1080.0f); // 배경 크기 설정

    Object* title = CreateGameObject("Title");
    title->AddComponent<SpriteRenderer>();

    title->GetComponent<SpriteRenderer>()->SetSprite(L"N_Title");
    title->GetComponent<SpriteRenderer>()->SetPosition(960.0f, 540.0f); // 중앙으로
    title->GetComponent<SpriteRenderer>()->SetSize(711.0f, 228.0f); // 배경 크기 설정



    //Object* temp = CreateGameObject("temp");
    //temp->AddComponent<SpriteRenderer>();
    //temp->AddComponent<Animation>();

    //vector<wstring> TitleFrames = { L"N_Title, N_Title" };
    //temp->GetComponent<Animation>()->AddAnimationClip(L"Title", TitleFrames, 0.1f, true);

    //// 애니메이션 재생
    //temp->GetComponent<Animation>()->PlayAnimation(L"Title");


    // 카메라 설정
    auto cameraObject = make_unique<Object>("MainCamera");

    // 카메라 컴포넌트 추가
    Camera* camera = cameraObject->AddComponent<Camera>();

    // 카메라 초기 설정
    camera->SetPosition(400.0f, 300.0f);  // 화면 중앙
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
    // UI 매니저 초기화
    UIManager::GetInstance()->Init();

    // 메인 메뉴 버튼 생성
    auto startButton = make_shared<UIButton>();
    startButton->SetText(L"게임 시작");
    startButton->SetPosition(-100.0f, 50.0f); // 중앙에서 왼쪽으로 100, 위로 25
    startButton->SetSize(500.0f, 500.0f);
    startButton->SetAnchor(UIAnchor::Center); // 화면 중앙 기준
    startButton->SetFont(L"맑은 고딕", 18, true);

    // 버튼 색상 설정
    startButton->SetBackgroundColor(ButtonState::Normal, RGB(255, 0, 0));
    startButton->SetBackgroundColor(ButtonState::Hovered, RGB(100, 150, 200));
    startButton->SetBackgroundColor(ButtonState::Pressed, RGB(50, 100, 150));
    startButton->SetTextColor(RGB(255, 255, 255));

    // 클릭 이벤트 설정
    startButton->SetClickCallback([]() 
        {
        // 게임 시작 로직
        SceneManager::GetInstance()->SceneLoad("GameScene");
        });

    // UI 매니저에 추가
    UIManager::GetInstance()->AddUI(startButton);

    // 종료 버튼 생성
    auto exitButton = make_shared<UIButton>();
    exitButton->SetText(L"종료");
    exitButton->SetPosition(100.0f, 35.0f); // 시작 버튼 아래
    exitButton->SetSize(200.0f, 50.0f);
    exitButton->SetAnchor(UIAnchor::Center);
    exitButton->SetFont(L"맑은 고딕", 32, true);

    exitButton->SetBackgroundColor(ButtonState::Normal, RGB(180, 70, 70));
    exitButton->SetBackgroundColor(ButtonState::Hovered, RGB(200, 100, 100));
    exitButton->SetBackgroundColor(ButtonState::Pressed, RGB(150, 50, 50));
    exitButton->SetTextColor(RGB(255, 255, 255));

    exitButton->SetClickCallback([]() {
        // 게임 종료 로직
        PostQuitMessage(0);
        });

    UIManager::GetInstance()->AddUI(exitButton);
}