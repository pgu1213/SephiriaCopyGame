#include "pch.h"
#include "TitleScene.h"
#include <Engine/Object/Component/SpriteRenderer/SpriteRenderer.h>
#include <Engine/Object/Component/Animation/Animation.h>
#include <Engine/Object/Component/UIButton/UIButton.h>
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

}