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
	background->GetComponent<SpriteRenderer>()->SetPosition(960.0f, 540.0f); // �߾�����
	background->GetComponent<SpriteRenderer>()->SetSize(1920.0f, 1080.0f); // ��� ũ�� ����

    Object* title = CreateGameObject("Title");
    title->AddComponent<SpriteRenderer>();

    title->GetComponent<SpriteRenderer>()->SetSprite(L"N_Title");
    title->GetComponent<SpriteRenderer>()->SetPosition(960.0f, 540.0f); // �߾�����
    title->GetComponent<SpriteRenderer>()->SetSize(711.0f, 228.0f); // ��� ũ�� ����



    //Object* temp = CreateGameObject("temp");
    //temp->AddComponent<SpriteRenderer>();
    //temp->AddComponent<Animation>();

    //vector<wstring> TitleFrames = { L"N_Title, N_Title" };
    //temp->GetComponent<Animation>()->AddAnimationClip(L"Title", TitleFrames, 0.1f, true);

    //// �ִϸ��̼� ���
    //temp->GetComponent<Animation>()->PlayAnimation(L"Title");


    // ī�޶� ����
    auto cameraObject = make_unique<Object>("MainCamera");

    // ī�޶� ������Ʈ �߰�
    Camera* camera = cameraObject->AddComponent<Camera>();

    // ī�޶� �ʱ� ����
    camera->SetPosition(400.0f, 300.0f);  // ȭ�� �߾�
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
    // UI �Ŵ��� �ʱ�ȭ
    UIManager::GetInstance()->Init();

    // ���� �޴� ��ư ����
    auto startButton = make_shared<UIButton>();
    startButton->SetText(L"���� ����");
    startButton->SetPosition(-100.0f, 50.0f); // �߾ӿ��� �������� 100, ���� 25
    startButton->SetSize(500.0f, 500.0f);
    startButton->SetAnchor(UIAnchor::Center); // ȭ�� �߾� ����
    startButton->SetFont(L"���� ���", 18, true);

    // ��ư ���� ����
    startButton->SetBackgroundColor(ButtonState::Normal, RGB(255, 0, 0));
    startButton->SetBackgroundColor(ButtonState::Hovered, RGB(100, 150, 200));
    startButton->SetBackgroundColor(ButtonState::Pressed, RGB(50, 100, 150));
    startButton->SetTextColor(RGB(255, 255, 255));

    // Ŭ�� �̺�Ʈ ����
    startButton->SetClickCallback([]() 
        {
        // ���� ���� ����
        SceneManager::GetInstance()->SceneLoad("GameScene");
        });

    // UI �Ŵ����� �߰�
    UIManager::GetInstance()->AddUI(startButton);

    // ���� ��ư ����
    auto exitButton = make_shared<UIButton>();
    exitButton->SetText(L"����");
    exitButton->SetPosition(100.0f, 35.0f); // ���� ��ư �Ʒ�
    exitButton->SetSize(200.0f, 50.0f);
    exitButton->SetAnchor(UIAnchor::Center);
    exitButton->SetFont(L"���� ���", 32, true);

    exitButton->SetBackgroundColor(ButtonState::Normal, RGB(180, 70, 70));
    exitButton->SetBackgroundColor(ButtonState::Hovered, RGB(200, 100, 100));
    exitButton->SetBackgroundColor(ButtonState::Pressed, RGB(150, 50, 50));
    exitButton->SetTextColor(RGB(255, 255, 255));

    exitButton->SetClickCallback([]() {
        // ���� ���� ����
        PostQuitMessage(0);
        });

    UIManager::GetInstance()->AddUI(exitButton);
}