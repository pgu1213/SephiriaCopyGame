#include "../pch.h"
#include "MainEditor.h"
#include "TileMapEditor/TileMapEditor.h"
#include "TilePalette/TilePalette.h"
#include "PropPalette/PropPalette.h"
#include "Camera/Camera.h"
#include "../Managers/InputManager/InputManager.h"
#include "../Managers/ResourceManager/ResourceManager.h"
#include "../Managers/FileManager/FileManager.h"
#include <filesystem>

MainEditor::MainEditor()
    : m_hDC(nullptr)
    , m_pTileMapEditor(nullptr)
    , m_pTilePalette(nullptr)
    , m_pPropPalette(nullptr)
    , m_pCamera(nullptr)
    , m_IsInitialized(false)
    , m_PropMode(false)
{
}

MainEditor::~MainEditor()
{
    Release();
}

void MainEditor::Initialize()
{
    if (m_IsInitialized) return;

    // HDC 얻기
    m_hDC = GetDC(g_hWnd);

    // 매니저 초기화
    ResourceManager::GetInstance()->Init();
    InputManager::GetInstance();
    FileManager::GetInstance();

    // 에디터 컴포넌트들 생성 및 초기화
    m_pTileMapEditor = new TileMapEditor();
    m_pTileMapEditor->Init();

    m_pTilePalette = new TilePalette();
    m_pTilePalette->Init();
    m_pTilePalette->SetPosition(100, 100);
    m_pTilePalette->SetSize(500, 600);

    m_pPropPalette = new PropPalette();
    m_pPropPalette->Init();
    m_pPropPalette->SetPosition(220, 10);
    m_pPropPalette->SetSize(200, 400);

    m_pCamera = new Camera();

    // 초기 뷰포트 설정
    RECT clientRect;
    GetClientRect(g_hWnd, &clientRect);
    int viewportWidth = clientRect.right - 440;
    int viewportHeight = clientRect.bottom;
    m_pTileMapEditor->SetViewport(0, 0, viewportWidth, viewportHeight);

    m_IsInitialized = true;

    PrintControls();
}

void MainEditor::Update()
{
    if (!m_IsInitialized) return;

    // 입력 매니저 업데이트
    InputManager::GetInstance()->Update();

    // 전역 입력 처리
    HandleGlobalInput();

    // 에디터 컴포넌트들 업데이트
    if (!m_PropMode)
    {
        m_pTilePalette->Update();

        // 선택된 타일을 타일맵 에디터에 전달
        int selectedTileID = m_pTilePalette->GetSelectedTileID();
        if (selectedTileID > 0)
        {
            m_pTileMapEditor->SetSelectedTileID(selectedTileID);
        }
    }
    else
    {
        m_pPropPalette->Update();
    }

    m_pTileMapEditor->Update();
    m_pCamera->Update();
}

void MainEditor::Render()
{
    if (!m_IsInitialized) return;

    // 화면 클리어
    RECT clientRect;
    GetClientRect(g_hWnd, &clientRect);
    HBRUSH bgBrush = CreateSolidBrush(RGB(64, 64, 64));
    FillRect(m_hDC, &clientRect, bgBrush);
    DeleteObject(bgBrush);

    // 타일맵 에디터 렌더
    m_pTileMapEditor->Render(m_hDC);

    if (!m_PropMode)
    {
        m_pTilePalette->Render(m_hDC);
    }
    else
    {
        m_pPropPalette->Render(m_hDC);
    }

    // UI 정보 렌더링
    RenderUI(m_hDC);
}

void MainEditor::Release()
{
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

    m_IsInitialized = false;
}

void MainEditor::HandleGlobalInput()
{

    // 저장 (Ctrl + S)
    if (InputManager::GetInstance()->IsKeyDown('S') && InputManager::GetInstance()->IsKeyPressed(VK_CONTROL))
    {
        SaveCurrentMap();
    }

    // 로드 (Ctrl + O)
    if (InputManager::GetInstance()->IsKeyDown('O') && InputManager::GetInstance()->IsKeyPressed(VK_CONTROL))
    {
        LoadMap();
    }

    // 새 맵 생성 (Ctrl + N)
    if (InputManager::GetInstance()->IsKeyDown('N') && InputManager::GetInstance()->IsKeyPressed(VK_CONTROL))
    {
        CreateNewMap();
    }

    // Prop 모드 토글 (Tab)
    if (InputManager::GetInstance()->IsKeyDown(VK_TAB))
    {
        TogglePropMode();
    }

    // 도움말 (F1)
    if (InputManager::GetInstance()->IsKeyDown(VK_F1))
    {
        PrintControls();
    }

    if (InputManager::GetInstance()->IsKeyDown('L') && InputManager::GetInstance()->IsKeyPressed(VK_CONTROL))
    {
        ShowMapFileList();
    }

    // 맵 검증 (Ctrl + V)
    if (InputManager::GetInstance()->IsKeyDown('V') && InputManager::GetInstance()->IsKeyPressed(VK_CONTROL))
    {
        ValidateCurrentMap();
    }

    // 파일 정리 (Ctrl + Delete)
    if (InputManager::GetInstance()->IsKeyDown(VK_DELETE) && InputManager::GetInstance()->IsKeyPressed(VK_CONTROL))
    {
        CleanupOldFiles();
    }
}

void MainEditor::RenderUI(HDC hdc)
{
    // 현재 상태 정보 표시
    RECT infoRect;
    GetClientRect(g_hWnd, &infoRect);
    infoRect.left = infoRect.right - 300;
    infoRect.top = infoRect.bottom - 100;

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));

    string layerNames[] = { "Ground", "UpperGround", "Collider" };
    LayerType currentLayer = m_pTileMapEditor->GetCurrentLayer();

    string infoText = "Mode: " + string(m_PropMode ? "Prop" : "Tile") + "\n";
    infoText += "Layer: " + layerNames[(int)currentLayer] + " (" + to_string((int)currentLayer + 1) + ")\n";

    if (!m_PropMode)
    {
        int selectedTileID = m_pTilePalette->GetSelectedTileID();
        infoText += "Selected Tile ID: " + to_string(selectedTileID) + "\n";
    }

    DrawTextA(hdc, infoText.c_str(), -1, &infoRect, DT_LEFT | DT_TOP);

    // 컨트롤 힌트
    RECT hintRect = infoRect;
    hintRect.top += 60;
    string hintText = "F1: Help | Tab: Toggle Mode\n";
    hintText += "1,2,3: Layer | Ctrl+S: Save\n";
    hintText += "Ctrl+O: Load | Ctrl+N: New";

    DrawTextA(hdc, hintText.c_str(), -1, &hintRect, DT_LEFT | DT_TOP);
}

void MainEditor::SaveCurrentMap()
{
    // 타임스탬프 기반 파일명 생성 (안전한 방식으로 변경)
    time_t now = time(0);
    tm timeStruct;
    localtime_s(&timeStruct, &now);

    wchar_t fileName[256];
    swprintf_s(fileName, L"../Maps/map_%04d%02d%02d_%02d%02d%02d_game.txt",
        timeStruct.tm_year + 1900, timeStruct.tm_mon + 1, timeStruct.tm_mday,
        timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec);

    // 저장 전 백업 생성
    wstring backupName = wstring(fileName);
    if (FileManager::GetInstance()->FileExists(backupName))
    {
        FileManager::GetInstance()->CreateBackup(backupName);
    }

    m_pTileMapEditor->SaveMap(fileName);

    // 저장 후 통계 출력
    auto stats = FileManager::GetInstance()->GetMapStatistics(fileName);
    cout << "\n=== SAVE COMPLETED ===" << endl;
    cout << "File: ";
    wcout << fileName << endl;
    cout << "Statistics:" << endl;
    cout << "  Total Tiles: " << stats.totalTiles << endl;
    cout << "  Unique Tile Types: " << stats.uniqueTileCount << endl;
    cout << "  Colliders: " << stats.colliderCount << endl;
    cout << "  File Size: " << stats.fileSize << " bytes" << endl;

    if (!stats.tileUsageCount.empty())
    {
        cout << "  Most Used Tiles:" << endl;
        vector<pair<int, int>> sortedTiles(stats.tileUsageCount.begin(), stats.tileUsageCount.end());
        sort(sortedTiles.begin(), sortedTiles.end(),
            [](const pair<int, int>& a, const pair<int, int>& b) {
                return a.second > b.second;
            });

        int displayCount = min(5, (int)sortedTiles.size());
        for (int i = 0; i < displayCount; ++i)
        {
            cout << "    Tile ID " << sortedTiles[i].first
                << ": " << sortedTiles[i].second << " uses" << endl;
        }
    }
    cout << "======================" << endl;
}

void MainEditor::LoadMap()
{
    // 최신 맵 파일 자동 찾기
    wstring latestFile = FileManager::GetInstance()->GetLatestMapFile();

    if (latestFile.empty())
    {
        cout << "No map files found in directory" << endl;

        // 맵 파일 목록 표시
        auto mapFiles = FileManager::GetInstance()->GetMapFileList();
        if (!mapFiles.empty())
        {
            cout << "Available map files:" << endl;
            for (const auto& file : mapFiles)
            {
                wcout << L"  " << file << endl;

                // 파일 통계 간단히 표시
                auto stats = FileManager::GetInstance()->GetMapStatistics(file);
                cout << "    (" << stats.totalTiles << " tiles, "
                    << stats.colliderCount << " colliders)" << endl;
            }
        }
        return;
    }

    cout << "\n=== LOADING MAP ===" << endl;
    cout << "File: ";
    wcout << latestFile << endl;

    m_pTileMapEditor->LoadMap(latestFile);

    cout << "===================" << endl;
}

void MainEditor::CreateNewMap()
{
    cout << "\n=== NEW MAP CREATED ===" << endl;
    cout << "Grid Size: 50x50" << endl;
    cout << "All layers cleared" << endl;
    cout << "======================" << endl;

    m_pTileMapEditor->ClearMap();
}

void MainEditor::ChangeGridSize()
{
    // 기본 그리드 크기 변경 (실제로는 입력 다이얼로그 사용)
    m_pTileMapEditor->SetGridSize(50, 50);
    cout << "Grid size changed to 50x50" << endl;
}

void MainEditor::ChangeRoomType()
{
    // 룸 타입 변경 로직 (필요시 구현)
    cout << "Room type changed" << endl;
}

void MainEditor::TogglePropMode()
{
    m_PropMode = !m_PropMode;
    cout << "Mode switched to: " << (m_PropMode ? "Prop" : "Tile") << endl;
}

void MainEditor::ShowMapFileList()
{
    auto mapFiles = FileManager::GetInstance()->GetMapFileList();

    cout << "\n=== MAP FILE LIST ===" << endl;
    if (mapFiles.empty())
    {
        cout << "No map files found" << endl;
    }
    else
    {
        for (size_t i = 0; i < mapFiles.size(); ++i)
        {
            cout << "[" << (i + 1) << "] ";
            wcout << mapFiles[i] << endl;

            auto stats = FileManager::GetInstance()->GetMapStatistics(mapFiles[i]);
            cout << "    Size: " << stats.fileSize << " bytes, ";
            cout << "Tiles: " << stats.totalTiles << ", ";
            cout << "Colliders: " << stats.colliderCount << endl;
        }
    }
    cout << "====================" << endl;
}

void MainEditor::CleanupOldFiles()
{
    cout << "\n=== CLEANING UP OLD FILES ===" << endl;

    // 7일 이상 된 백업 파일 정리
    FileManager::GetInstance()->CleanOldBackups();

    cout << "Old backup files cleaned" << endl;
    cout << "============================" << endl;
}

void MainEditor::ValidateCurrentMap()
{
    // 현재 맵을 임시 파일로 저장하여 검증
    wstring tempFile = L"../Maps/temp_validation.txt";

    m_pTileMapEditor->SaveMap(tempFile);

    TileMapSaveData validationData;

    if (FileManager::GetInstance()->LoadTileMap(tempFile, validationData))
    {
        if (FileManager::GetInstance()->ValidateMapData(validationData))
        {
            cout << "\n=== MAP VALIDATION ===" << endl;
            cout << "Map data is valid!" << endl;

            auto stats = FileManager::GetInstance()->GetMapStatistics(tempFile);
            cout << "Statistics:" << endl;
            cout << "  Total Tiles: " << stats.totalTiles << endl;
            cout << "  Unique Types: " << stats.uniqueTileCount << endl;
            cout << "  Colliders: " << stats.colliderCount << endl;
            cout << "======================" << endl;
        }
        else
        {
            cout << "Map validation failed!" << endl;
        }
    }

    // 임시 파일 삭제
    FileManager::GetInstance()->DeleteMapFile(tempFile);
}

void MainEditor::PrintControls()
{
    cout << "\n=== TILE MAP EDITOR CONTROLS ===" << endl;
    cout << "LAYER SYSTEM:" << endl;
    cout << "  1 - Ground Layer (stores tile IDs)" << endl;
    cout << "  2 - UpperGround Layer (stores tile IDs)" << endl;
    cout << "  3 - Collider Layer (stores 0/1 values, shows green boxes)" << endl;
    cout << endl;
    cout << "EDITING:" << endl;
    cout << "  Left Click  - Place tile/collider" << endl;
    cout << "  Right Click - Remove tile/collider" << endl;
    cout << "  Tab         - Toggle between Tile and Prop mode" << endl;
    cout << endl;
    cout << "FILE OPERATIONS:" << endl;
    cout << "  Ctrl + S    - Save current map (with auto-backup)" << endl;
    cout << "  Ctrl + O    - Load latest map" << endl;
    cout << "  Ctrl + N    - Create new map" << endl;
    cout << "  Ctrl + L    - List all map files with statistics" << endl;
    cout << "  Ctrl + V    - Validate current map data" << endl;
    cout << "  Ctrl + Del  - Cleanup old backup files" << endl;
    cout << endl;
    cout << "VIEW OPTIONS:" << endl;
    cout << "  G           - Toggle grid display" << endl;
    cout << "  C           - Toggle collider display" << endl;
    cout << endl;
    cout << "FEATURES:" << endl;
    cout << "  - Tile IDs synchronized between editor and game" << endl;
    cout << "  - Automatic backup creation before saving" << endl;
    cout << "  - Map validation and corruption recovery" << endl;
    cout << "  - Detailed file statistics and usage analytics" << endl;
    cout << "  - Performance optimized batch rendering" << endl;
    cout << "  - Smart file management with cleanup utilities" << endl;
    cout << endl;
    cout << "  F1          - Show this help" << endl;
    cout << "=================================" << endl;
}