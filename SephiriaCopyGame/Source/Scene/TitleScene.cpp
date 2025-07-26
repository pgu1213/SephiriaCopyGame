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
    // ��׶���
    UI* background = CreateUI("Background");
    background->AddComponent<UIImage>();

    background->GetComponent<UIImage>()->SetSprite(L"N_Title_Back");
	background->SetPosition(960.0f, 540.0f); // �߾�����
	background->SetSize(1920, 1080.0f); // ��� ũ�� ����

    // Ÿ��Ʋ
    UI* title = CreateUI("Background");
    title->AddComponent<UIImage>();

    title->GetComponent<UIImage>()->SetSprite(L"N_Title");
    title->SetPosition(960.0f, 540.0f);
    title->SetSize(711.0f, 228.0f);


}