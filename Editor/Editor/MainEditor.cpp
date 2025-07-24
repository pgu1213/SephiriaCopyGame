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

    // 카메라 이동 (방향키)
    float cameraSpeed = 5.0f / m_pCamera->GetZoom(); // 줌에 따라 속도 조절
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

    // 카메라 줌 (Page Up/Down)
    if (inputMgr->IsKeyDown(VK_PRIOR)) // Page Up - 줌 인
    {
        float currentZoom = m_pCamera->GetZoom();
        m_pCamera->SetZoom(currentZoom * 1.2f);
    }

    if (inputMgr->IsKeyDown(VK_NEXT)) // Page Down - 줌 아웃
    {
        float currentZoom = m_pCamera->GetZoom();
        m_pCamera->SetZoom(currentZoom / 1.2f);
    }

    // 기능 키들 (KeyDown 사용으로 중복 입력 방지)
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


    if (inputMgr->IsKeyDown('Z'))
    {
        ChangeRoomType();
    }

    if (inputMgr->IsKeyDown('X'))
    {
        ChangeGridSize();
    }
}

void MainEditor::RenderUI(HDC hdc)
{
    // UI 정보 표시
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);

    wstring modeText = m_PropMode ? L"Mode: Prop" : L"Mode: Tile";
    TextOut(hdc, 10, 10, modeText.c_str(), modeText.length());

    wstring cameraInfo = L"Camera: (" + to_wstring((int)m_pCamera->GetX()) +
        L", " + to_wstring((int)m_pCamera->GetY()) + L")";
    TextOut(hdc, 10, 30, cameraInfo.c_str(), cameraInfo.length());

    // 줌 정보 추가
    wstring zoomInfo = L"Zoom: " + to_wstring((int)(m_pCamera->GetZoom() * 100)) + L"%";
    TextOut(hdc, 10, 50, zoomInfo.c_str(), zoomInfo.length());

    // 컨트롤 가이드
    TextOut(hdc, 10, m_ScreenHeight - 140, L"Controls:", 9);
    TextOut(hdc, 10, m_ScreenHeight - 120, L"Arrow Keys: Move Camera", 23);
    TextOut(hdc, 10, m_ScreenHeight - 100, L"Page Up/Down: Zoom In/Out", 25);
    TextOut(hdc, 10, m_ScreenHeight - 80, L"S: Save Map", 11);
    TextOut(hdc, 10, m_ScreenHeight - 60, L"L: Load Map", 11);
    TextOut(hdc, 10, m_ScreenHeight - 40, L"T: Toggle Tile/Prop Mode", 24);
    TextOut(hdc, 10, m_ScreenHeight - 20, L"G: Toggle Grid", 14);
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

void MainEditor::ChangeGridSize()
{
    if (!m_pTileMapEditor) return;

    int currentGridSize = m_pTileMapEditor->GetGridSize();
    int newGridSize = currentGridSize;

    cout << "\n=== 그리드 크기 변경 ===" << endl;
    cout << "현재 그리드 크기: " << currentGridSize << "px" << endl;
    cout << "사용 가능한 그리드 크기: 16, 32, 48, 64" << endl;
    cout << "새로운 그리드 크기를 선택하세요 (1~4): ";

    // 키보드 입력 대기
    bool inputReceived = false;
    while (!inputReceived)
    {
        InputManager::GetInstance()->Update(); // 매 프레임 업데이트
        auto inputMgr = InputManager::GetInstance();

        if (inputMgr->IsKeyDown('1'))
        {
            newGridSize = 16;
            inputReceived = true;
        }
        else if (inputMgr->IsKeyDown('2'))
        {
            newGridSize = 32;
            inputReceived = true;
        }
        else if (inputMgr->IsKeyDown('3'))
        {
            newGridSize = 48;
            inputReceived = true;
        }
        else if (inputMgr->IsKeyDown('4'))
        {
            newGridSize = 64;
            inputReceived = true;
        }
        else if (inputMgr->IsKeyDown(VK_ESCAPE))
        {
            cout << "그리드 크기 변경을 취소했습니다." << endl;
            return;
        }

        Sleep(16); // ~60FPS
    }

    m_pTileMapEditor->SetGridSize(newGridSize);
    cout << "그리드 크기를 " << newGridSize << "px로 변경했습니다." << endl;
}

void MainEditor::ChangeRoomType()
{
    cout << "\n=== 방 타입 변경 ===" << endl;
    cout << "사용 가능한 방 타입:" << endl;
    cout << "1. 던전방 (어두운 테마)" << endl;
    cout << "2. 동굴방 (자연 테마)" << endl;
    cout << "3. 성방 (밝은 테마)" << endl;
    cout << "4. 보스방 (특별한 테마)" << endl;
    cout << "방 타입을 선택하세요 (1~4): ";

    bool inputReceived = false;
    wstring roomType = L"Unknown";

    while (!inputReceived)
    {
        InputManager::GetInstance()->Update(); // 매 프레임 업데이트
        auto inputMgr = InputManager::GetInstance();

        if (inputMgr->IsKeyDown('1'))
        {
            roomType = L"Dungeon";
            inputReceived = true;
        }
        else if (inputMgr->IsKeyDown('2'))
        {
            roomType = L"Cave";
            inputReceived = true;
        }
        else if (inputMgr->IsKeyDown('3'))
        {
            roomType = L"Castle";
            inputReceived = true;
        }
        else if (inputMgr->IsKeyDown('4'))
        {
            roomType = L"Boss";
            inputReceived = true;
        }
        else if (inputMgr->IsKeyDown(VK_ESCAPE))
        {
            cout << "방 타입 변경을 취소했습니다." << endl;
            return;
        }

        Sleep(16); // ~60FPS
    }

    wcout << L"방 타입을 " << roomType << L"으로 설정했습니다." << endl;
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