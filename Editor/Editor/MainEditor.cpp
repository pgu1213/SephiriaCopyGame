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

    // 매니저들 초기화
    if (!ResourceManager::GetInstance()->Init())
    {
        printf("Failed to initialize ResourceManager\n");
        return;
    }

    // HDC 얻기
    m_hDC = GetDC(g_hWnd);
    if (!m_hDC)
    {
        printf("Failed to get HDC\n");
        return;
    }

    // 카메라 생성
    m_pCamera = new Camera();

    // 에디터 컴포넌트들 생성
    m_pTileMapEditor = new TileMapEditor();
    m_pTilePalette = new TilePalette();
    m_pPropPalette = new PropPalette();

    // 컴포넌트들 초기화
    m_pTileMapEditor->Init(50, 50); // 50x50 그리드로 초기화
    m_pTilePalette->Init(m_pTileMapEditor);
    m_pPropPalette->Init();

    // 팔레트 위치 설정
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

    // 입력 매니저 업데이트
    InputManager::GetInstance()->Update();

    // 전역 키 입력 처리
    HandleGlobalInput();

    // 컴포넌트들 업데이트
    m_pCamera->Update();
    m_pTileMapEditor->Update(m_pCamera);
    m_pTilePalette->Update();
    m_pPropPalette->Update();
}

void MainEditor::Render()
{
    if (!m_IsInitialized || !m_hDC) return;

    // 더블 버퍼링을 위한 백버퍼 생성
    RECT clientRect;
    GetClientRect(g_hWnd, &clientRect);

    HDC backDC = CreateCompatibleDC(m_hDC);
    HBITMAP backBitmap = CreateCompatibleBitmap(m_hDC, clientRect.right, clientRect.bottom);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(backDC, backBitmap);

    // 배경 클리어
    HBRUSH bgBrush = CreateSolidBrush(RGB(32, 32, 32));
    FillRect(backDC, &clientRect, bgBrush);
    DeleteObject(bgBrush);

    // 컴포넌트들 렌더링
    m_pTileMapEditor->Render(backDC, m_pCamera);
    m_pTilePalette->Render(backDC);
    m_pPropPalette->Render(backDC);

    // UI 정보 렌더링
    RenderUI(backDC);

    // 백버퍼를 화면에 복사
    BitBlt(m_hDC, 0, 0, clientRect.right, clientRect.bottom, backDC, 0, 0, SRCCOPY);

    // 백버퍼 정리
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

    // 싱글톤 매니저들 해제
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

    // 파일 저장 (Ctrl + S)
    if (input->IsKeyPressed(VK_CONTROL) && input->IsKeyDown('S'))
    {
        SaveCurrentMap();
    }

    // 파일 로드 (Ctrl + O)
    if (input->IsKeyPressed(VK_CONTROL) && input->IsKeyDown('O'))
    {
        LoadMap();
    }

    // 새 맵 생성 (Ctrl + N)
    if (input->IsKeyPressed(VK_CONTROL) && input->IsKeyDown('N'))
    {
        CreateNewMap();
    }

    // 그리드 크기 변경 (Ctrl + G)
    if (input->IsKeyPressed(VK_CONTROL) && input->IsKeyDown('G'))
    {
        ChangeGridSize();
    }

    // 방 타입 변경 (Ctrl + R)
    if (input->IsKeyPressed(VK_CONTROL) && input->IsKeyDown('R'))
    {
        ChangeRoomType();
    }

    // 도움말 표시 (F1)
    if (input->IsKeyDown(VK_F1))
    {
        PrintControls();
    }

    // 프롭 배치 모드 토글 (Ctrl + P)
    if (input->IsKeyPressed(VK_CONTROL) && input->IsKeyDown('P'))
    {
        TogglePropMode();
    }

    // 프롭 배치/제거 (프롭 모드일 때)
    if (m_PropMode)
    {
        POINT mousePos = input->GetMousePosition();
        int gridX, gridY;
        m_pTileMapEditor->ScreenToGrid(mousePos.x, mousePos.y, gridX, gridY, m_pCamera);

        if (input->IsKeyDown(VK_LBUTTON))
        {
            if (gridX >= 0 && gridX < 100 && gridY >= 0 && gridY < 100) // 범위 체크
            {
                m_pPropPalette->PlaceProp(gridX, gridY);
            }
        }

        if (input->IsKeyDown(VK_RBUTTON))
        {
            if (gridX >= 0 && gridX < 100 && gridY >= 0 && gridY < 100) // 범위 체크
            {
                m_pPropPalette->Removeprop(gridX, gridY);
            }
        }
    }

    // 문 설정 (Shift + 방향키)
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

    // 카메라 정보
    char cameraInfo[128];
    sprintf_s(cameraInfo, "Camera: (%.1f, %.1f) Zoom: %.2f",
        m_pCamera->GetX(), m_pCamera->GetY(), m_pCamera->GetZoom());
    TextOutA(hdc, 10, WINCY - 80, cameraInfo, strlen(cameraInfo));

    // 현재 선택된 레이어 정보
    const char* layerNames[] = { "BACKGROUND", "COLLISION", "DECORATION", "INTERACTION" };
    char layerInfo[64];
    sprintf_s(layerInfo, "Current Layer: %s", layerNames[static_cast<int>(m_pTileMapEditor->GetCurrentLayer())]);
    TextOutA(hdc, 10, WINCY - 60, layerInfo, strlen(layerInfo));

    // 마우스 위치의 그리드 좌표
    POINT mousePos = InputManager::GetInstance()->GetMousePosition();
    int gridX, gridY;
    m_pTileMapEditor->ScreenToGrid(mousePos.x, mousePos.y, gridX, gridY, m_pCamera);

    char gridInfo[64];
    sprintf_s(gridInfo, "Grid: (%d, %d)", gridX, gridY);
    TextOutA(hdc, 10, WINCY - 40, gridInfo, strlen(gridInfo));

    // 도움말
    const char* helpText = "F1: Help | Ctrl+S: Save | Ctrl+O: Load | Ctrl+N: New";
    TextOutA(hdc, 10, WINCY - 20, helpText, strlen(helpText));
}

void MainEditor::SaveCurrentMap()
{
    printf("Saving current map...\n");

    // FileManager를 통해 맵 데이터 저장
    FileManager* fileManager = FileManager::GetInstance();

    // 현재 날짜/시간으로 파일명 생성
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
    // TODO: 파일 선택 다이얼로그 구현 또는 파일명 입력
    // FileManager를 통해 맵 데이터 로드
}

void MainEditor::CreateNewMap()
{
    printf("Creating new map...\n");

    // 기본 크기로 새 맵 생성
    m_pTileMapEditor->Init(50, 50);

    // 카메라 리셋
    m_pCamera->SetPosition(0, 0);

    printf("New map created (50x50)\n");
}

void MainEditor::ChangeGridSize()
{
    printf("Grid size change requested\n");
    printf("Current grid size can be changed by modifying TileMapEditor::Init() parameters\n");
    printf("Available sizes: 25x25, 50x50, 75x75, 100x100\n");

    // 간단한 크기 변경 (키 입력으로)
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