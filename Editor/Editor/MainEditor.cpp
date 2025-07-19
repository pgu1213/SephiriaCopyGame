#include "../pch.h"
#include "MainEditor.h"
#include "TileMapEditor/TileMapEditor.h"
#include "TilePalette/TilePalette.h"
#include "PropPalette/PropPalette.h"
#include "Camera/Camera.h"
#include "../Managers/InputManager/InputManager.h"
#include "../Managers/ResourceManager/ResourceManager.h"
#include "../Managers/ConfigManager/ConfigManager.h"
#include "../Managers/FileManager/FileManager.h"

MainEditor::MainEditor()
    : m_hDC(nullptr), m_pTileMapEditor(nullptr), m_pTilePalette(nullptr),
    m_pPropPalette(nullptr), m_pCamera(nullptr), m_IsInitialized(false), m_PropMode(false)
{
}

MainEditor::~MainEditor()
{
    Release();
}

void MainEditor::Initialize()
{
    printf("Initializing MainEditor...\n");

    // �Ŵ����� �ʱ�ȭ
    if (!ResourceManager::GetInstance()->Init())
    {
        printf("Failed to initialize ResourceManager\n");
        return;
    }

    // HDC ���
    m_hDC = GetDC(g_hWnd);
    if (!m_hDC)
    {
        printf("Failed to get HDC\n");
        return;
    }

    // ī�޶� ����
    m_pCamera = new Camera();

    // ������ ������Ʈ�� ����
    m_pTileMapEditor = new TileMapEditor();
    m_pTilePalette = new TilePalette();
    m_pPropPalette = new PropPalette();

    // ������Ʈ�� �ʱ�ȭ
    m_pTileMapEditor->Init(50, 50); // 50x50 �׸���� �ʱ�ȭ
    m_pTilePalette->Init(m_pTileMapEditor);
    m_pPropPalette->Init();

    // �ȷ�Ʈ ��ġ ����
    m_pTilePalette->SetPosition(WINCX - -600, 50);
    m_pTilePalette->SetSize(400, 500);

    m_pPropPalette->SetPosition(WINCX - -600, 370);
    m_pPropPalette->SetSize(400, 500);

    m_IsInitialized = true;

    printf("MainEditor initialized successfully\n");
    PrintControls();
}

void MainEditor::Update()
{
    if (!m_IsInitialized) return;

    // �Է� �Ŵ��� ������Ʈ
    InputManager::GetInstance()->Update();

    // ���� Ű �Է� ó��
    HandleGlobalInput();

    // ������Ʈ�� ������Ʈ
    m_pCamera->Update();
    m_pTileMapEditor->Update(m_pCamera);
    m_pTilePalette->Update();
    m_pPropPalette->Update();
}

void MainEditor::Render()
{
    if (!m_IsInitialized || !m_hDC) return;

    // ���� ���۸��� ���� ����� ����
    RECT clientRect;
    GetClientRect(g_hWnd, &clientRect);

    HDC backDC = CreateCompatibleDC(m_hDC);
    HBITMAP backBitmap = CreateCompatibleBitmap(m_hDC, clientRect.right, clientRect.bottom);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(backDC, backBitmap);

    // ��� Ŭ����
    HBRUSH bgBrush = CreateSolidBrush(RGB(32, 32, 32));
    FillRect(backDC, &clientRect, bgBrush);
    DeleteObject(bgBrush);

    // ������Ʈ�� ������
    m_pTileMapEditor->Render(backDC, m_pCamera);
    m_pTilePalette->Render(backDC);
    m_pPropPalette->Render(backDC);

    // UI ���� ������
    RenderUI(backDC);

    // ����۸� ȭ�鿡 ����
    BitBlt(m_hDC, 0, 0, clientRect.right, clientRect.bottom, backDC, 0, 0, SRCCOPY);

    // ����� ����
    SelectObject(backDC, oldBitmap);
    DeleteObject(backBitmap);
    DeleteDC(backDC);
}

void MainEditor::Release()
{
    printf("Releasing MainEditor...\n");

    if (m_pTileMapEditor)
    {
        delete m_pTileMapEditor;
        m_pTileMapEditor = nullptr;
    }

    if (m_pTilePalette)
    {
        delete m_pTilePalette;
        m_pTilePalette = nullptr;
    }

    if (m_pPropPalette)
    {
        delete m_pPropPalette;
        m_pPropPalette = nullptr;
    }

    if (m_pCamera)
    {
        delete m_pCamera;
        m_pCamera = nullptr;
    }

    if (m_hDC)
    {
        ReleaseDC(g_hWnd, m_hDC);
        m_hDC = nullptr;
    }

    // �̱��� �Ŵ����� ����
    ResourceManager::GetInstance()->DestoryInstance();
    InputManager::GetInstance()->DestoryInstance();
    ConfigManager::GetInstance()->DestoryInstance();
    FileManager::GetInstance()->DestoryInstance();

    m_IsInitialized = false;

    printf("MainEditor released\n");
}

void MainEditor::HandleGlobalInput()
{
    InputManager* input = InputManager::GetInstance();

    // ���� ���� (Ctrl + S)
    if (input->IsKeyPressed(VK_CONTROL) && input->IsKeyDown('S'))
    {
        SaveCurrentMap();
    }

    // ���� �ε� (Ctrl + O)
    if (input->IsKeyPressed(VK_CONTROL) && input->IsKeyDown('O'))
    {
        LoadMap();
    }

    // �� �� ���� (Ctrl + N)
    if (input->IsKeyPressed(VK_CONTROL) && input->IsKeyDown('N'))
    {
        CreateNewMap();
    }

    // �׸��� ũ�� ���� (Ctrl + G)
    if (input->IsKeyPressed(VK_CONTROL) && input->IsKeyDown('G'))
    {
        ChangeGridSize();
    }

    // �� Ÿ�� ���� (Ctrl + R)
    if (input->IsKeyPressed(VK_CONTROL) && input->IsKeyDown('R'))
    {
        ChangeRoomType();
    }

    // ���� ǥ�� (F1)
    if (input->IsKeyDown(VK_F1))
    {
        PrintControls();
    }

    // ���� ��ġ ��� ��� (Ctrl + P)
    if (input->IsKeyPressed(VK_CONTROL) && input->IsKeyDown('P'))
    {
        TogglePropMode();
    }

    // ���� ��ġ/���� (���� ����� ��)
    if (m_PropMode)
    {
        POINT mousePos = input->GetMousePosition();
        int gridX, gridY;
        m_pTileMapEditor->ScreenToGrid(mousePos.x, mousePos.y, gridX, gridY, m_pCamera);

        if (input->IsKeyDown(VK_LBUTTON))
        {
            if (gridX >= 0 && gridX < 100 && gridY >= 0 && gridY < 100) // ���� üũ
            {
                m_pPropPalette->PlaceProp(gridX, gridY);
            }
        }

        if (input->IsKeyDown(VK_RBUTTON))
        {
            if (gridX >= 0 && gridX < 100 && gridY >= 0 && gridY < 100) // ���� üũ
            {
                m_pPropPalette->Removeprop(gridX, gridY);
            }
        }
    }

    // �� ���� (Shift + ����Ű)
    if (input->IsKeyPressed(VK_SHIFT))
    {
        if (input->IsKeyDown(VK_UP))
        {
            ConfigManager::GetInstance()->ToggleDoor(DoorDirection::NORTH);
        }
        if (input->IsKeyDown(VK_DOWN))
        {
            ConfigManager::GetInstance()->ToggleDoor(DoorDirection::SOUTH);
        }
        if (input->IsKeyDown(VK_LEFT))
        {
            ConfigManager::GetInstance()->ToggleDoor(DoorDirection::WEST);
        }
        if (input->IsKeyDown(VK_RIGHT))
        {
            ConfigManager::GetInstance()->ToggleDoor(DoorDirection::EAST);
        }
    }
}

void MainEditor::RenderUI(HDC hdc)
{
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);

    // ī�޶� ����
    char cameraInfo[128];
    sprintf_s(cameraInfo, "Camera: (%.1f, %.1f) Zoom: %.2f",
        m_pCamera->GetX(), m_pCamera->GetY(), m_pCamera->GetZoom());
    TextOutA(hdc, 10, WINCY - 80, cameraInfo, strlen(cameraInfo));

    // ���� ���õ� ���̾� ����
    const char* layerNames[] = { "BACKGROUND", "COLLISION", "DECORATION", "INTERACTION" };
    char layerInfo[64];
    sprintf_s(layerInfo, "Current Layer: %s", layerNames[static_cast<int>(m_pTileMapEditor->GetCurrentLayer())]);
    TextOutA(hdc, 10, WINCY - 60, layerInfo, strlen(layerInfo));

    // ���콺 ��ġ�� �׸��� ��ǥ
    POINT mousePos = InputManager::GetInstance()->GetMousePosition();
    int gridX, gridY;
    m_pTileMapEditor->ScreenToGrid(mousePos.x, mousePos.y, gridX, gridY, m_pCamera);

    char gridInfo[64];
    sprintf_s(gridInfo, "Grid: (%d, %d)", gridX, gridY);
    TextOutA(hdc, 10, WINCY - 40, gridInfo, strlen(gridInfo));

    // ����
    const char* helpText = "F1: Help | Ctrl+S: Save | Ctrl+O: Load | Ctrl+N: New";
    TextOutA(hdc, 10, WINCY - 20, helpText, strlen(helpText));
}

void MainEditor::SaveCurrentMap()
{
    printf("Saving current map...\n");

    // FileManager�� ���� �� ������ ����
    FileManager* fileManager = FileManager::GetInstance();

    // ���� ��¥/�ð����� ���ϸ� ����
    time_t now = time(0);
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);

    char filename[128];
    sprintf_s(filename, "map_%04d%02d%02d_%02d%02d%02d.xml",
        timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
        timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    if (fileManager->SaveMap(filename, m_pTileMapEditor))
    {
        printf("Map saved successfully: %s\n", filename);
    }
    else
    {
        printf("Failed to save map: %s\n", filename);
    }
}

void MainEditor::LoadMap()
{
    printf("Loading map... (Feature not implemented yet)\n");
    // TODO: ���� ���� ���̾�α� ���� �Ǵ� ���ϸ� �Է�
    // FileManager�� ���� �� ������ �ε�
}

void MainEditor::CreateNewMap()
{
    printf("Creating new map...\n");

    // �⺻ ũ��� �� �� ����
    m_pTileMapEditor->Init(50, 50);

    // ī�޶� ����
    m_pCamera->SetPosition(0, 0);

    printf("New map created (50x50)\n");
}

void MainEditor::ChangeGridSize()
{
    printf("Grid size change requested\n");
    printf("Current grid size can be changed by modifying TileMapEditor::Init() parameters\n");
    printf("Available sizes: 25x25, 50x50, 75x75, 100x100\n");

    // ������ ũ�� ���� (Ű �Է�����)
    InputManager* input = InputManager::GetInstance();

    if (input->IsKeyPressed('1'))
    {
        m_pTileMapEditor->SetGridSize(25, 25);
        printf("Grid size set to 25x25\n");
    }
    else if (input->IsKeyPressed('2'))
    {
        m_pTileMapEditor->SetGridSize(50, 50);
        printf("Grid size set to 50x50\n");
    }
    else if (input->IsKeyPressed('3'))
    {
        m_pTileMapEditor->SetGridSize(75, 75);
        printf("Grid size set to 75x75\n");
    }
    else if (input->IsKeyPressed('4'))
    {
        m_pTileMapEditor->SetGridSize(100, 100);
        printf("Grid size set to 100x100\n");
    }
}

void MainEditor::ChangeRoomType()
{
    ConfigManager* configManager = ConfigManager::GetInstance();

    printf("Room type change requested\n");
    printf("Available room types:\n");
    printf("1: Entrance Room\n");
    printf("2: Combat Room\n");
    printf("3: Exit Room\n");
    printf("4: Boss Room\n");
    printf("Press number key to select room type\n");

    InputManager* input = InputManager::GetInstance();

    if (input->IsKeyPressed('1'))
    {
        configManager->SetRoomType(RoomType::ENTRANCE);
        printf("Room type set to: Entrance Room\n");
    }
    else if (input->IsKeyPressed('2'))
    {
        configManager->SetRoomType(RoomType::COMBAT);
        printf("Room type set to: Combat Room\n");
    }
    else if (input->IsKeyPressed('3'))
    {
        configManager->SetRoomType(RoomType::EXIT);
        printf("Room type set to: Exit Room\n");
    }
    else if (input->IsKeyPressed('4'))
    {
        configManager->SetRoomType(RoomType::BOSS);
        printf("Room type set to: Boss Room\n");
    }
}

void MainEditor::TogglePropMode()
{
    m_PropMode = !m_PropMode;
    printf("Prop placement mode: %s\n", m_PropMode ? "ON" : "OFF");
}

void MainEditor::PrintControls()
{
    printf("\n=== Editor Controls ===\n");
    printf("Camera Movement:\n");
    printf("  Arrow Keys: Move camera\n");
    printf("  Page Up/Down: Zoom in/out\n");
    printf("  Home: Reset camera\n\n");

    printf("Tile Editing:\n");
    printf("  Left Click: Paint tile\n");
    printf("  Right Click: Erase tile\n");
    printf("  Middle Click: Show tile properties\n");
    printf("  1-4: Switch layers\n");
    printf("  Ctrl+1-4: Toggle layer visibility\n");
    printf("  G: Toggle grid display\n");
    printf("  T: Toggle tile properties display\n\n");

    printf("Prop Editing:\n");
    printf("  Ctrl+P: Toggle prop placement mode\n");
    printf("  In prop mode - Left Click: Place prop\n");
    printf("  In prop mode - Right Click: Remove prop\n\n");

    printf("Quick Tile Selection:\n");
    printf("  Q: Floor tile\n");
    printf("  W: Wall tile\n");
    printf("  E: Cliff tile\n");
    printf("  R: Monster spawn tile\n");
    printf("  T: Monster move tile\n\n");

    printf("Room Configuration:\n");
    printf("  Shift + Arrow Keys: Toggle doors (N/S/E/W)\n");
    printf("  Ctrl+R: Change room type\n");
    printf("  Ctrl+G: Change grid size\n\n");

    printf("File Operations:\n");
    printf("  Ctrl+S: Save map\n");
    printf("  Ctrl+O: Load map\n");
    printf("  Ctrl+N: New map\n\n");

    printf("Palette Navigation:\n");
    printf("  Page Up/Down: Change tile palette page\n");
    printf("  Arrow Keys: Navigate tile selection\n");
    printf("  Click on palette: Select tile/prop\n\n");

    printf("Other:\n");
    printf("  F1: Show this help\n");
    printf("  ESC: Exit editor\n");
    printf("========================\n\n");
}