#include "pch.h"
#include "TitleScene.h"
#include <Engine/Managers/UIManager/UIManager.h>
#include <Engine/Object/Component/SpriteRenderer/SpriteRenderer.h>
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
    // TitleScene �ʱ�ȭ ����
    // ��: ��� �̹���, Ÿ��Ʋ �ؽ�Ʈ, ��ư �� UI ������Ʈ ����
    // ����: ��� ������Ʈ ����
    Object* background = CreateGameObject("Background");
    background->AddComponent<SpriteRenderer>();
    

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
    startButton->SetPosition(-100.0f, -25.0f); // �߾ӿ��� �������� 100, ���� 25
    startButton->SetSize(500.0f, 500.0f);
    startButton->SetAnchor(UIAnchor::Center); // ȭ�� �߾� ����
    startButton->SetFont(L"���� ���", 18, true);

    // ��ư ���� ����
    startButton->SetBackgroundColor(ButtonState::Normal, RGB(70, 130, 180));
    startButton->SetBackgroundColor(ButtonState::Hovered, RGB(100, 150, 200));
    startButton->SetBackgroundColor(ButtonState::Pressed, RGB(50, 100, 150));
    startButton->SetTextColor(RGB(0, 0, 0));

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
    exitButton->SetPosition(-100.0f, 35.0f); // ���� ��ư �Ʒ�
    exitButton->SetSize(200.0f, 50.0f);
    exitButton->SetAnchor(UIAnchor::Center);
    exitButton->SetFont(L"���� ���", 18, true);

    exitButton->SetBackgroundColor(ButtonState::Normal, RGB(180, 70, 70));
    exitButton->SetBackgroundColor(ButtonState::Hovered, RGB(200, 100, 100));
    exitButton->SetBackgroundColor(ButtonState::Pressed, RGB(150, 50, 50));
    exitButton->SetTextColor(RGB(0, 0, 0));

    exitButton->SetClickCallback([]() {
        // ���� ���� ����
        PostQuitMessage(0);
        });

    UIManager::GetInstance()->AddUI(exitButton);

    // HUD UI ���� (�»�ܿ� ����)
    auto healthUI = make_shared<UI>();
    healthUI->SetPosition(10.0f, 10.0f);
    healthUI->SetSize(200.0f, 30.0f);
    healthUI->SetAnchor(UIAnchor::TopLeft);
    healthUI->SetColor(RGB(200, 50, 50));

    UIManager::GetInstance()->AddUI(healthUI);
}