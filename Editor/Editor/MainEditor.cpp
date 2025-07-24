#include "../pch.h"
#include "MainEditor.h"
#include "TileMapEditor/TileMapEditor.h"
#include "TilePalette/TilePalette.h"
#include "PropPalette/PropPalette.h"
#include "Camera/Camera.h"
#include "../Managers/InputManager/InputManager.h"
#include "../Managers/ResourceManager/ResourceManager.h"
#include "../Managers/FileManager/FileManager.h"

extern HWND g_hWnd;

MainEditor::MainEditor()
    : m_hDC(nullptr)
    , m_memDC(nullptr)
    , m_memBitmap(nullptr)
    , m_oldBitmap(nullptr)
    , m_pTileMapEditor(nullptr)
    , m_pTilePalette(nullptr)
    , m_pPropPalette(nullptr)
    , m_pCamera(nullptr)
    , m_IsInitialized(false)
    , m_PropMode(false)
    , m_ScreenWidth(1280)
    , m_ScreenHeight(960)
{
}

MainEditor::~MainEditor()
{
    Release();
}

void MainEditor::Initialize()
{
    if (m_IsInitialized) return;

    InputManager::GetInstance()->Update();

    // 리소스 매니저 초기화
    ResourceManager::GetInstance()->Init();
    ResourceManager::GetInstance()->LoadAllResources();

    // 더블 버퍼링 설정
    m_hDC = GetDC(g_hWnd);
    RECT clientRect;
    GetClientRect(g_hWnd, &clientRect);
    m_ScreenWidth = clientRect.right;
    m_ScreenHeight = clientRect.bottom;

    m_memDC = CreateCompatibleDC(m_hDC);
    m_memBitmap = CreateCompatibleBitmap(m_hDC, m_ScreenWidth, m_ScreenHeight);
    m_oldBitmap = (HBITMAP)SelectObject(m_memDC, m_memBitmap);

    // 각 컴포넌트 초기화 (순서 중요)
    m_pCamera = new Camera();
    m_pCamera->SetPosition(0, 0);

    m_pTilePalette = new TilePalette();
    m_pTilePalette->Initialize();

    m_pPropPalette = new PropPalette();
    m_pPropPalette->Initialize();

    m_pTileMapEditor = new TileMapEditor();
    m_pTileMapEditor->Initialize(m_pCamera);
    m_pTileMapEditor->SetTilePalette(m_pTilePalette);  // 타일 팔레트 연결

    m_IsInitialized = true;
    cout << "에디터 초기화 완료!" << endl;
    PrintControls();
}

void MainEditor::Update()
{
    if (!m_IsInitialized) return;

    InputManager::GetInstance()->Update();

    HandleGlobalInput();

    m_pCamera->Update();
    m_pTileMapEditor->Update();

    if (!m_PropMode)
    {
        m_pTilePalette->Update();
    }
    else
    {
        m_pPropPalette->Update();
    }
}

void MainEditor::Render()
{
    if (!m_IsInitialized) return;

    // 백버퍼 클리어
    RECT clearRect = { 0, 0, m_ScreenWidth, m_ScreenHeight };
    FillRect(m_memDC, &clearRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    // 메인 렌더링
    m_pTileMapEditor->Render(m_memDC);

    if (!m_PropMode)
    {
        m_pTilePalette->Render(m_memDC);
    }
    else
    {
        m_pPropPalette->Render(m_memDC);
    }

    RenderUI(m_memDC);

    // 프론트 버퍼로 복사 (더블 버퍼링)
    BitBlt(m_hDC, 0, 0, m_ScreenWidth, m_ScreenHeight, m_memDC, 0, 0, SRCCOPY);
}

void MainEditor::HandleGlobalInput()
{
    auto inputMgr = InputManager::GetInstance();

    // 카메라 이동
    float cameraSpeed = 5.0f / m_pCamera->GetZoom();
    if (inputMgr->IsKeyPressed(VK_LEFT))
    {
        m_pCamera->SetPosition(m_pCamera->GetX() - cameraSpeed, m_pCamera->GetY());
    }
    if (inputMgr->IsKeyPressed(VK_RIGHT))
    {
        m_pCamera->SetPosition(m_pCamera->GetX() + cameraSpeed, m_pCamera->GetY());
    }
    if (inputMgr->IsKeyPressed(VK_UP))
    {
        m_pCamera->SetPosition(m_pCamera->GetX(), m_pCamera->GetY() - cameraSpeed);
    }
    if (inputMgr->IsKeyPressed(VK_DOWN))
    {
        m_pCamera->SetPosition(m_pCamera->GetX(), m_pCamera->GetY() + cameraSpeed);
    }

    // 카메라 줌
    if (inputMgr->IsKeyDown(VK_PRIOR))
    {
        float currentZoom = m_pCamera->GetZoom();
        m_pCamera->SetZoom(currentZoom * 1.2f);
    }

    if (inputMgr->IsKeyDown(VK_NEXT))
    {
        float currentZoom = m_pCamera->GetZoom();
        m_pCamera->SetZoom(currentZoom / 1.2f);
    }

    // 기능 키들
    if (inputMgr->IsKeyDown('S'))
    {
        SaveCurrentMap();
    }

    if (inputMgr->IsKeyDown('L'))
    {
        LoadMap();
    }

    if (inputMgr->IsKeyDown('T'))
    {
        TogglePropMode();
    }

    if (inputMgr->IsKeyDown('N'))
    {
        CreateNewMap();
    }

    if (inputMgr->IsKeyDown('R'))
    {
        ChangeRoomSize();
    }

    // 방 타입 변경 (Y키)
    if (inputMgr->IsKeyDown('Y'))
    {
        ChangeRoomType();
    }

    // 빠른 방 타입 변경 (숫자키)
    if (inputMgr->IsKeyPressed(VK_CONTROL))
    {
        if (inputMgr->IsKeyDown('1'))
        {
            if (m_pTileMapEditor) m_pTileMapEditor->SetRoomType(RoomType::ENTRANCE);
        }
        if (inputMgr->IsKeyDown('2'))
        {
            if (m_pTileMapEditor) m_pTileMapEditor->SetRoomType(RoomType::BATTLE);
        }
        if (inputMgr->IsKeyDown('3'))
        {
            if (m_pTileMapEditor) m_pTileMapEditor->SetRoomType(RoomType::EXIT);
        }
        if (inputMgr->IsKeyDown('4'))
        {
            if (m_pTileMapEditor) m_pTileMapEditor->SetRoomType(RoomType::BOSS);
        }
    }
}

void MainEditor::RenderUI(HDC hdc)
{
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);

    wstring modeText = m_PropMode ? L"Mode: Prop" : L"Mode: Tile";
    TextOut(hdc, 10, 10, modeText.c_str(), modeText.length());

    wstring cameraInfo = L"Camera: (" + to_wstring((int)m_pCamera->GetX()) +
        L", " + to_wstring((int)m_pCamera->GetY()) + L")";
    TextOut(hdc, 10, 30, cameraInfo.c_str(), cameraInfo.length());

    wstring zoomInfo = L"Zoom: " + to_wstring((int)(m_pCamera->GetZoom() * 100)) + L"%";
    TextOut(hdc, 10, 50, zoomInfo.c_str(), zoomInfo.length());

    wstring gridInfo = L"Grid: 16px (Fixed)";
    TextOut(hdc, 10, 90, gridInfo.c_str(), gridInfo.length());

    // 컨트롤 가이드 (방 타입 관련 추가)
    TextOut(hdc, 10, m_ScreenHeight - 180, L"Controls:", 9);
    TextOut(hdc, 10, m_ScreenHeight - 160, L"Arrow Keys: Move Camera", 23);
    TextOut(hdc, 10, m_ScreenHeight - 140, L"Page Up/Down: Zoom", 18);
    TextOut(hdc, 10, m_ScreenHeight - 120, L"S: Save, L: Load, T: Toggle Mode", 32);
    TextOut(hdc, 10, m_ScreenHeight - 100, L"G: Grid, B: Bounds, R: Room Size", 32);
    TextOut(hdc, 10, m_ScreenHeight - 80, L"Y: Change Room Type", 19);
    TextOut(hdc, 10, m_ScreenHeight - 60, L"Ctrl+1~4: Quick Room Type", 25);
    TextOut(hdc, 10, m_ScreenHeight - 40, L"N: New Map", 10);
}

void MainEditor::SaveCurrentMap()
{
    if (m_pTileMapEditor)
    {
        m_pTileMapEditor->SaveMap();
        cout << "맵을 저장했습니다." << endl;
    }
}

void MainEditor::LoadMap()
{
    if (m_pTileMapEditor)
    {
        m_pTileMapEditor->LoadMap();
        cout << "맵을 불러왔습니다." << endl;
    }
}

void MainEditor::CreateNewMap()
{
    if (!m_pTileMapEditor) return;

    // 기존 맵 데이터 클리어
    m_pTileMapEditor->Release();
    m_pTileMapEditor->Initialize(m_pCamera);
    m_pTileMapEditor->SetTilePalette(m_pTilePalette);

    // 카메라 초기 위치로 이동
    m_pCamera->SetPosition(0, 0);

    cout << "새로운 맵을 생성했습니다." << endl;
}

void MainEditor::ChangeRoomType()
{
    if (!m_pTileMapEditor) return;

    cout << "\n=== 방 타입 변경 ===" << endl;
    cout << "사용 가능한 방 타입:" << endl;
    cout << "1. 입구방 (Entrance Room) - 플레이어가 시작하는 방" << endl;
    cout << "2. 전투방 (Battle Room) - 몬스터와 전투하는 방" << endl;
    cout << "3. 출구방 (Exit Room) - 다음 스테이지로 이동하는 방" << endl;
    cout << "4. 보스방 (Boss Room) - 보스 몬스터와 전투하는 방" << endl;
    cout << "방 타입을 선택하세요 (1~4): ";

    bool inputReceived = false;
    RoomType newType = RoomType::ENTRANCE;

    while (!inputReceived)
    {
        InputManager::GetInstance()->Update();
        auto inputMgr = InputManager::GetInstance();

        if (inputMgr->IsKeyDown('1'))
        {
            newType = RoomType::ENTRANCE;
            inputReceived = true;
        }
        else if (inputMgr->IsKeyDown('2'))
        {
            newType = RoomType::BATTLE;
            inputReceived = true;
        }
        else if (inputMgr->IsKeyDown('3'))
        {
            newType = RoomType::EXIT;
            inputReceived = true;
        }
        else if (inputMgr->IsKeyDown('4'))
        {
            newType = RoomType::BOSS;
            inputReceived = true;
        }
        else if (inputMgr->IsKeyDown(VK_ESCAPE))
        {
            cout << "방 타입 변경을 취소했습니다." << endl;
            return;
        }

        Sleep(16);
    }

    m_pTileMapEditor->SetRoomType(newType);
}

void MainEditor::ChangeRoomSize()
{
    if (!m_pTileMapEditor) return;

    cout << "\n=== 방 크기 변경 ===" << endl;
    cout << "사용 가능한 방 크기:" << endl;
    cout << "1. 소형 (30x20 그리드 = 480x320px)" << endl;
    cout << "2. 중형 (50x30 그리드 = 800x480px)" << endl;
    cout << "3. 대형 (70x40 그리드 = 1120x640px)" << endl;
    cout << "4. 초대형 (100x60 그리드 = 1600x960px)" << endl;
    cout << "방 크기를 선택하세요 (1~4): ";

    bool inputReceived = false;

    while (!inputReceived)
    {
        InputManager::GetInstance()->Update();
        auto inputMgr = InputManager::GetInstance();

        if (inputMgr->IsKeyDown('1'))
        {
            m_pTileMapEditor->SetRoomBounds(30, 20);
            inputReceived = true;
        }
        else if (inputMgr->IsKeyDown('2'))
        {
            m_pTileMapEditor->SetRoomBounds(50, 30);
            inputReceived = true;
        }
        else if (inputMgr->IsKeyDown('3'))
        {
            m_pTileMapEditor->SetRoomBounds(70, 40);
            inputReceived = true;
        }
        else if (inputMgr->IsKeyDown('4'))
        {
            m_pTileMapEditor->SetRoomBounds(100, 60);
            inputReceived = true;
        }
        else if (inputMgr->IsKeyDown(VK_ESCAPE))
        {
            cout << "방 크기 변경을 취소했습니다." << endl;
            return;
        }

        Sleep(16);
    }
}

void MainEditor::TogglePropMode()
{
    m_PropMode = !m_PropMode;
    cout << "모드 변경: " << (m_PropMode ? "Prop" : "Tile") << endl;
}

void MainEditor::ShowMapFileList()
{
    cout << "\n=== 저장된 맵 파일 목록 ===" << endl;

    try
    {
        if (!filesystem::exists(L"Maps"))
        {
            cout << "Maps 폴더가 존재하지 않습니다." << endl;
            return;
        }

        vector<filesystem::path> mapFiles;

        // 맵 파일들 수집
        for (const auto& entry : filesystem::directory_iterator(L"Maps"))
        {
            if (entry.is_regular_file())
            {
                wstring extension = entry.path().extension().wstring();
                if (extension == L".txt")
                {
                    mapFiles.push_back(entry.path());
                }
            }
        }

        if (mapFiles.empty())
        {
            cout << "저장된 맵 파일이 없습니다." << endl;
            return;
        }

        // 파일 정렬 (최신 순)
        sort(mapFiles.begin(), mapFiles.end(), [](const filesystem::path& a, const filesystem::path& b) {
            return filesystem::last_write_time(a) > filesystem::last_write_time(b);
            });

        cout << "총 " << mapFiles.size() << "개의 맵 파일:" << endl;
        cout << "----------------------------------------" << endl;

        int index = 1;
        for (const auto& mapFile : mapFiles)
        {
            wstring filename = mapFile.filename().wstring();
            auto writeTime = filesystem::last_write_time(mapFile);
            auto size = filesystem::file_size(mapFile);

            wcout << index << L". " << filename;
            cout << " (" << size << " bytes)" << endl;

            index++;
            if (index > 10) // 최대 10개만 표시
            {
                cout << "... 그리고 " << (mapFiles.size() - 10) << "개 더" << endl;
                break;
            }
        }

        cout << "----------------------------------------" << endl;
        cout << "L키를 눌러 최신 맵을 로드할 수 있습니다." << endl;
    }
    catch (const filesystem::filesystem_error& ex)
    {
        cout << "파일 시스템 오류: " << ex.what() << endl;
    }
}

void MainEditor::CleanupOldFiles()
{
    cout << "\n=== 오래된 파일 정리 ===" << endl;

    try
    {
        if (!filesystem::exists(L"Maps"))
        {
            cout << "Maps 폴더가 존재하지 않습니다." << endl;
            return;
        }

        vector<filesystem::path> mapFiles;

        // 맵 파일들 수집
        for (const auto& entry : filesystem::directory_iterator(L"Maps"))
        {
            if (entry.is_regular_file())
            {
                wstring extension = entry.path().extension().wstring();
                wstring filename = entry.path().filename().wstring();

                // latest_map.txt는 제외
                if (extension == L".txt" && filename != L"latest_map.txt")
                {
                    mapFiles.push_back(entry.path());
                }
            }
        }

        if (mapFiles.size() <= 5) // 5개 이하면 정리하지 않음
        {
            cout << "정리할 파일이 없습니다. (총 " << mapFiles.size() << "개 파일)" << endl;
            return;
        }

        // 파일 정렬 (오래된 순)
        sort(mapFiles.begin(), mapFiles.end(), [](const filesystem::path& a, const filesystem::path& b) {
            return filesystem::last_write_time(a) < filesystem::last_write_time(b);
            });

        // 가장 오래된 파일들 삭제 (최신 5개만 유지)
        int filesToDelete = mapFiles.size() - 5;
        int deletedCount = 0;

        cout << "가장 오래된 " << filesToDelete << "개의 파일을 삭제합니다..." << endl;

        for (int i = 0; i < filesToDelete; i++)
        {
            try
            {
                wstring filename = mapFiles[i].filename().wstring();
                filesystem::remove(mapFiles[i]);
                wcout << L"삭제됨: " << filename << endl;
                deletedCount++;
            }
            catch (const filesystem::filesystem_error& ex)
            {
                cout << "파일 삭제 실패: " << ex.what() << endl;
            }
        }

        cout << "정리 완료: " << deletedCount << "개 파일 삭제됨" << endl;
        cout << "남은 파일: " << (mapFiles.size() - deletedCount) << "개" << endl;
    }
    catch (const filesystem::filesystem_error& ex)
    {
        cout << "파일 시스템 오류: " << ex.what() << endl;
    }
}


void MainEditor::ValidateCurrentMap()
{
    cout << "\n=== 현재 맵 검증 ===" << endl;

    if (!m_pTileMapEditor)
    {
        cout << "타일맵 에디터가 초기화되지 않았습니다." << endl;
        return;
    }

    // 기본 검증 정보들
    int gridSize = m_pTileMapEditor->GetGridSize();
    cout << "그리드 크기: " << gridSize << "px" << endl;

    // 카메라 위치
    cout << "카메라 위치: (" << (int)m_pCamera->GetX() << ", " << (int)m_pCamera->GetY() << ")" << endl;

    // 리소스 상태 검증
    cout << "\n--- 리소스 상태 ---" << endl;
    auto tileNames = ResourceManager::GetInstance()->GetTileNames();
    auto propNames = ResourceManager::GetInstance()->GetPropNames();

    cout << "로드된 타일: " << tileNames.size() << "개" << endl;
    cout << "로드된 프롭: " << propNames.size() << "개" << endl;

    if (tileNames.empty())
    {
        cout << "⚠️ 경고: 타일이 로드되지 않았습니다. Resources/Tiles/ 폴더를 확인하세요." << endl;
    }

    // 선택된 타일 검증
    if (m_pTilePalette)
    {
        wstring selectedTile = m_pTilePalette->GetSelectedTile();
        if (!selectedTile.empty())
        {
            wcout << L"선택된 타일: " << selectedTile << endl;

            Bitmap* sprite = ResourceManager::GetInstance()->GetSprite(selectedTile);
            if (sprite)
            {
                cout << "✓ 선택된 타일이 정상적으로 로드됨" << endl;
            }
            else
            {
                cout << "⚠️ 경고: 선택된 타일을 찾을 수 없습니다." << endl;
            }
        }
        else
        {
            cout << "선택된 타일 없음" << endl;
        }
    }

    // 현재 모드
    cout << "현재 모드: " << (m_PropMode ? "Prop" : "Tile") << endl;

    // 시스템 상태
    cout << "\n--- 시스템 상태 ---" << endl;
    cout << "더블 버퍼링: " << (m_memDC ? "활성화" : "비활성화") << endl;
    cout << "초기화 상태: " << (m_IsInitialized ? "완료" : "미완료") << endl;

    cout << "\n검증 완료!" << endl;
}

void MainEditor::PrintControls()
{
    cout << "\n=== 에디터 컨트롤 ===" << endl;
    cout << "방향키: 카메라 이동" << endl;
    cout << "S: 맵 저장" << endl;
    cout << "L: 맵 로드" << endl;
    cout << "T: 타일/프롭 모드 전환" << endl;
    cout << "===================" << endl;
}

void MainEditor::Release()
{
    if (m_oldBitmap)
    {
        SelectObject(m_memDC, m_oldBitmap);
        m_oldBitmap = nullptr;
    }

    if (m_memBitmap)
    {
        DeleteObject(m_memBitmap);
        m_memBitmap = nullptr;
    }

    if (m_memDC)
    {
        DeleteDC(m_memDC);
        m_memDC = nullptr;
    }

    if (m_hDC)
    {
        ReleaseDC(g_hWnd, m_hDC);
        m_hDC = nullptr;
    }

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

    ResourceManager::GetInstance()->Release();
}