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
	background->GetComponent<SpriteRenderer>()->SetPosition(960.0f, 540.0f); // �߾�����
	background->GetComponent<SpriteRenderer>()->SetSize(1920.0f, 1080.0f); // ��� ũ�� ����

    Object* title = CreateGameObject("Title");
    title->AddComponent<SpriteRenderer>();

    title->GetComponent<SpriteRenderer>()->SetSprite(L"N_Title");
    title->GetComponent<SpriteRenderer>()->SetPosition(960.0f, 540.0f); // �߾�����
    title->GetComponent<SpriteRenderer>()->SetSize(711.0f, 228.0f); // ��� ũ�� ����

    // ī�޶� ����
    auto cameraObject = make_unique<Object>("MainCamera");

    // ī�޶� ������Ʈ �߰�
    Camera* camera = cameraObject->AddComponent<Camera>();

    // ī�޶� �ʱ� ����
    camera->SetPosition(960.0f, 540.0f);  // ȭ�� �߾�
    camera->SetZoom(1.0f);
    camera->SetViewportSize(800.0f, 600.0f);

    // ī�޶� ��� ���� (���� ũ�� 0~1600, 0~1200)
    camera->SetBounds(0.0f, 0.0f, 1600.0f, 1200.0f);

    // ���� ī�޶�� ����
    Camera::SetMainCamera(camera);

    // �÷��̾� ���󰡱� ����
    //Object* player = FindPlayerObject(); // �÷��̾� ��ü ã��
    //if (player)
    //{
    //    camera->SetTarget(player);
    //    camera->SetFollowSpeed(5.0f);
    //    camera->SetFollowOffset(0.0f, -50.0f); // �÷��̾�� �ణ ����
    //}

    SetupUI();
}

void TitleScene::SetupUI()
{

}