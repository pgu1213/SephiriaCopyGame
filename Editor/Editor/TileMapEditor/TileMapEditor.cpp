#include "../../pch.h"
#include "TileMapEditor.h"
#include "../Camera/Camera.h"
#include "../TilePalette/TilePalette.h"
#include "../../Managers/InputManager/InputManager.h"
#include "../../Managers/ResourceManager/ResourceManager.h"
#include "../../Managers/FileManager/FileManager.h"
#include <fstream>
#include <sstream>

TileMapEditor::TileMapEditor()
    : m_pCamera(nullptr)
    , m_pTilePalette(nullptr)
    , m_ShowGrid(true)
    , m_EnableCulling(true)
    , m_RoomWidth(DEFAULT_ROOM_WIDTH)
    , m_RoomHeight(DEFAULT_ROOM_HEIGHT)
    , m_ShowRoomBounds(true)
    , m_RoomType(RoomType::ENTRANCE)
    , m_CurrentLayer(LayerType::GROUND)
    , m_LeftMousePressed(false)
    , m_RightMousePressed(false)
{
    m_LastMousePos = { 0, 0 };

    m_LayerVisibility[LayerType::GROUND] = true;
    m_LayerVisibility[LayerType::DECORATION] = true;
    m_LayerVisibility[LayerType::CLIFF] = true;
    m_LayerVisibility[LayerType::COLLIDER] = true;
}

TileMapEditor::~TileMapEditor()
{
    Release();
}

void TileMapEditor::Initialize(Camera* camera)
{
    m_pCamera = camera;
    cout << "타일맵 에디터 초기화 완료 (그리드: " << GRID_SIZE << "px 고정)" << endl;
    cout << "방 크기: " << m_RoomWidth << " x " << m_RoomHeight << " 그리드" << endl;
}

void TileMapEditor::Update()
{
    HandleInput();
}

void TileMapEditor::HandleInput()
{
    auto inputMgr = InputManager::GetInstance();

    POINT currentMousePos = inputMgr->GetMousePosition();

    // 마우스 입력 처리
    if (inputMgr->IsKeyDown(VK_LBUTTON))
    {
        PlaceTile();
        m_LeftMousePressed = true;
    }
    else if (inputMgr->IsKeyPressed(VK_LBUTTON) && m_LeftMousePressed)
    {
        if (abs(currentMousePos.x - m_LastMousePos.x) > 5 ||
            abs(currentMousePos.y - m_LastMousePos.y) > 5)
        {
            PlaceTile();
        }
    }
    else if (inputMgr->IsKeyUp(VK_LBUTTON))
    {
        m_LeftMousePressed = false;
    }

    if (inputMgr->IsKeyDown(VK_RBUTTON))
    {
        RemoveTile();
        m_RightMousePressed = true;
    }
    else if (inputMgr->IsKeyPressed(VK_RBUTTON) && m_RightMousePressed)
    {
        if (abs(currentMousePos.x - m_LastMousePos.x) > 5 ||
            abs(currentMousePos.y - m_LastMousePos.y) > 5)
        {
            RemoveTile();
        }
    }
    else if (inputMgr->IsKeyUp(VK_RBUTTON))
    {
        m_RightMousePressed = false;
    }

    m_LastMousePos = currentMousePos;

    // 기존 키들
    if (inputMgr->IsKeyDown('G'))
    {
        m_ShowGrid = !m_ShowGrid;
    }

    if (inputMgr->IsKeyDown('B'))
    {
        m_ShowRoomBounds = !m_ShowRoomBounds;
    }

    // 레이어 전환 (1~4 숫자키)
    if (inputMgr->IsKeyDown('1'))
    {
        SetCurrentLayer(LayerType::GROUND);
    }
    if (inputMgr->IsKeyDown('2'))
    {
        SetCurrentLayer(LayerType::DECORATION);
    }
    if (inputMgr->IsKeyDown('3'))
    {
        SetCurrentLayer(LayerType::CLIFF);
    }
    if (inputMgr->IsKeyDown('4'))
    {
        SetCurrentLayer(LayerType::COLLIDER);
    }

    // 레이어 가시성 토글 (Alt + 1~4)
    if (inputMgr->IsKeyPressed(VK_MENU)) // Alt 키
    {
        if (inputMgr->IsKeyDown('1'))
        {
            ToggleLayerVisibility(LayerType::GROUND);
        }
        if (inputMgr->IsKeyDown('2'))
        {
            ToggleLayerVisibility(LayerType::DECORATION);
        }
        if (inputMgr->IsKeyDown('3'))
        {
            ToggleLayerVisibility(LayerType::CLIFF);
        }
        if (inputMgr->IsKeyDown('4'))
        {
            ToggleLayerVisibility(LayerType::COLLIDER);
        }
    }
}

void TileMapEditor::PlaceTile()
{
    if (!m_pTilePalette) return;

    POINT mousePos = InputManager::GetInstance()->GetMousePosition();
    float worldX, worldY;
    m_pCamera->ScreenToWorld(mousePos.x, mousePos.y, worldX, worldY);

    int gridX = (int)(worldX / GRID_SIZE);
    int gridY = (int)(worldY / GRID_SIZE);

    if (!IsWithinRoomBounds(gridX, gridY)) return;

    pair<int, int> gridPos = make_pair(gridX, gridY);

    // 현재 레이어에 따라 다른 처리
    if (m_CurrentLayer == LayerType::GROUND || m_CurrentLayer == LayerType::DECORATION)
    {
        // 타일 레이어 - 선택된 타일 배치
        wstring selectedTile = m_pTilePalette->GetSelectedTile();
        if (selectedTile.empty()) return;

        TileData newTile(selectedTile, gridX * GRID_SIZE, gridY * GRID_SIZE, m_CurrentLayer);

        if (m_CurrentLayer == LayerType::GROUND)
        {
            m_LayerData.groundLayer[gridPos] = newTile;
        }
        else if (m_CurrentLayer == LayerType::DECORATION)
        {
            m_LayerData.decorationLayer[gridPos] = newTile;
        }
    }
    else
    {
        // 색상 박스 레이어 - 색상 박스 배치
        PlaceColorBox();
    }
}

void TileMapEditor::PlaceColorBox()
{
    POINT mousePos = InputManager::GetInstance()->GetMousePosition();
    float worldX, worldY;
    m_pCamera->ScreenToWorld(mousePos.x, mousePos.y, worldX, worldY);

    int gridX = (int)(worldX / GRID_SIZE);
    int gridY = (int)(worldY / GRID_SIZE);

    if (!IsWithinRoomBounds(gridX, gridY)) return;

    pair<int, int> gridPos = make_pair(gridX, gridY);

    if (m_CurrentLayer == LayerType::CLIFF)
    {
        m_LayerData.cliffLayer[gridPos] = true;
    }
    else if (m_CurrentLayer == LayerType::COLLIDER)
    {
        m_LayerData.colliderLayer[gridPos] = true;
    }
}

void TileMapEditor::RemoveTile()
{
    POINT mousePos = InputManager::GetInstance()->GetMousePosition();
    float worldX, worldY;
    m_pCamera->ScreenToWorld(mousePos.x, mousePos.y, worldX, worldY);

    int gridX = (int)(worldX / GRID_SIZE);
    int gridY = (int)(worldY / GRID_SIZE);

    if (!IsWithinRoomBounds(gridX, gridY)) return;

    pair<int, int> gridPos = make_pair(gridX, gridY);

    // 현재 레이어에 따라 다른 처리
    if (m_CurrentLayer == LayerType::GROUND)
    {
        m_LayerData.groundLayer.erase(gridPos);
    }
    else if (m_CurrentLayer == LayerType::DECORATION)
    {
        m_LayerData.decorationLayer.erase(gridPos);
    }
    else
    {
        RemoveColorBox();
    }
}

void TileMapEditor::RemoveColorBox()
{
    POINT mousePos = InputManager::GetInstance()->GetMousePosition();
    float worldX, worldY;
    m_pCamera->ScreenToWorld(mousePos.x, mousePos.y, worldX, worldY);

    int gridX = (int)(worldX / GRID_SIZE);
    int gridY = (int)(worldY / GRID_SIZE);

    if (!IsWithinRoomBounds(gridX, gridY)) return;

    pair<int, int> gridPos = make_pair(gridX, gridY);

    if (m_CurrentLayer == LayerType::CLIFF)
    {
        m_LayerData.cliffLayer.erase(gridPos);
    }
    else if (m_CurrentLayer == LayerType::COLLIDER)
    {
        m_LayerData.colliderLayer.erase(gridPos);
    }
}

void TileMapEditor::Render(HDC hdc)
{
    if (m_ShowGrid)
    {
        RenderGrid(hdc);
    }

    if (m_ShowRoomBounds)
    {
        RenderRoomBounds(hdc);
    }

    // 모든 레이어 렌더링
    RenderAllLayers(hdc);

    // UI 정보 렌더링
    RenderRoomInfo(hdc);
    RenderLayerInfo(hdc);
}

void TileMapEditor::RenderGrid(HDC hdc)
{
    HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(64, 64, 64));
    HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);

    // 방 경계 내에서만 그리드 계산
    float worldLeft = 0;
    float worldTop = 0;
    float worldRight = m_RoomWidth * GRID_SIZE;
    float worldBottom = m_RoomHeight * GRID_SIZE;

    // 수직선 (방 경계 내에서만)
    for (int x = 0; x <= m_RoomWidth; x++)
    {
        float worldX = x * GRID_SIZE;
        POINT start = m_pCamera->WorldToScreen(worldX, worldTop);
        POINT end = m_pCamera->WorldToScreen(worldX, worldBottom);
        MoveToEx(hdc, start.x, start.y, NULL);
        LineTo(hdc, end.x, end.y);
    }

    // 수평선 (방 경계 내에서만)
    for (int y = 0; y <= m_RoomHeight; y++)
    {
        float worldY = y * GRID_SIZE;
        POINT start = m_pCamera->WorldToScreen(worldLeft, worldY);
        POINT end = m_pCamera->WorldToScreen(worldRight, worldY);
        MoveToEx(hdc, start.x, start.y, NULL);
        LineTo(hdc, end.x, end.y);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(gridPen);
}

void TileMapEditor::RenderAllLayers(HDC hdc)
{
    // 레이어 순서: 그라운드 → 장식 → 낭떠러지 → 콜라이더

    // 1. 그라운드 레이어
    if (IsLayerVisible(LayerType::GROUND))
    {
        RenderTileLayer(hdc, m_LayerData.groundLayer);
    }

    // 2. 장식 레이어
    if (IsLayerVisible(LayerType::DECORATION))
    {
        RenderTileLayer(hdc, m_LayerData.decorationLayer);
    }

    // 3. 낭떠러지 레이어 (빨간색)
    if (IsLayerVisible(LayerType::CLIFF))
    {
        RenderColorLayer(hdc, m_LayerData.cliffLayer, RGB(255, 0, 0));
    }

    // 4. 콜라이더 레이어 (초록색)
    if (IsLayerVisible(LayerType::COLLIDER))
    {
        RenderColorLayer(hdc, m_LayerData.colliderLayer, RGB(0, 255, 0));
    }
}

void TileMapEditor::RenderTileLayer(HDC hdc, const map<pair<int, int>, TileData>& layer)
{
    Graphics graphics(hdc);
    float zoom = m_pCamera->GetZoom();
    int scaledGridSize = (int)(GRID_SIZE * zoom);

    for (const auto& tilePair : layer)
    {
        const TileData& tile = tilePair.second;
        if (tile.isEmpty) continue;

        Bitmap* tileSprite = ResourceManager::GetInstance()->GetSprite(tile.tileName);
        if (!tileSprite) continue;

        POINT screenPos = m_pCamera->WorldToScreen((float)tile.x, (float)tile.y);
        graphics.DrawImage(tileSprite, screenPos.x, screenPos.y, scaledGridSize, scaledGridSize);
    }
}

void TileMapEditor::RenderColorLayer(HDC hdc, const map<pair<int, int>, bool>& layer, COLORREF color)
{
    HBRUSH colorBrush = CreateSolidBrush(color);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, colorBrush);

    float zoom = m_pCamera->GetZoom();
    int scaledGridSize = (int)(GRID_SIZE * zoom);

    for (const auto& gridPair : layer)
    {
        if (!gridPair.second) continue; // false인 경우 건너뛰기

        int worldX = gridPair.first.first * GRID_SIZE;
        int worldY = gridPair.first.second * GRID_SIZE;

        POINT screenPos = m_pCamera->WorldToScreen((float)worldX, (float)worldY);

        // 반투명 효과를 위해 색상 박스 그리기
        RECT boxRect = {
            screenPos.x,
            screenPos.y,
            screenPos.x + scaledGridSize,
            screenPos.y + scaledGridSize
        };

        FillRect(hdc, &boxRect, colorBrush);

        // 테두리 그리기
        HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
        Rectangle(hdc, boxRect.left, boxRect.top, boxRect.right, boxRect.bottom);
        SelectObject(hdc, oldPen);
        DeleteObject(borderPen);
    }

    SelectObject(hdc, oldBrush);
    DeleteObject(colorBrush);
}

void TileMapEditor::RenderTiles(HDC hdc)
{
    Graphics graphics(hdc);
    float zoom = m_pCamera->GetZoom();
    int scaledGridSize = (int)(GRID_SIZE * zoom); // 줌에 따른 스프라이트 크기

    for (const auto& tilePair : m_TileMap)
    {
        const TileData& tile = tilePair.second;
        if (tile.isEmpty) continue;

        Bitmap* tileSprite = ResourceManager::GetInstance()->GetSprite(tile.tileName);
        if (!tileSprite) continue;

        POINT screenPos = m_pCamera->WorldToScreen((float)tile.x, (float)tile.y);

        // 줌에 따라 스프라이트 크기 조절
        graphics.DrawImage(tileSprite, screenPos.x, screenPos.y, scaledGridSize, scaledGridSize);
    }
}

void TileMapEditor::RenderCulledTiles(HDC hdc)
{
    RECT clientRect;
    GetClientRect(GetActiveWindow(), &clientRect);

    float worldLeft, worldTop, worldRight, worldBottom;
    m_pCamera->ScreenToWorld(0, 0, worldLeft, worldTop);
    m_pCamera->ScreenToWorld(clientRect.right, clientRect.bottom, worldRight, worldBottom);

    // 여백 추가
    int margin = GRID_SIZE * 2;
    worldLeft -= margin;
    worldTop -= margin;
    worldRight += margin;
    worldBottom += margin;

    Graphics graphics(hdc);
    float zoom = m_pCamera->GetZoom();
    int scaledGridSize = (int)(GRID_SIZE * zoom); // 줌에 따른 스프라이트 크기
    int renderedCount = 0;

    for (const auto& tilePair : m_TileMap)
    {
        const TileData& tile = tilePair.second;
        if (tile.isEmpty) continue;

        // 컬링 검사
        if (tile.x + GRID_SIZE < worldLeft || tile.x > worldRight ||
            tile.y + GRID_SIZE < worldTop || tile.y > worldBottom)
        {
            continue;
        }

        Bitmap* tileSprite = ResourceManager::GetInstance()->GetSprite(tile.tileName);
        if (!tileSprite) continue;

        POINT screenPos = m_pCamera->WorldToScreen((float)tile.x, (float)tile.y);

        // 줌에 따라 스프라이트 크기 조절
        graphics.DrawImage(tileSprite, screenPos.x, screenPos.y, scaledGridSize, scaledGridSize);
        renderedCount++;
    }

    // 렌더링 통계
    SetTextColor(hdc, RGB(255, 255, 0));
    SetBkMode(hdc, TRANSPARENT);
    wstring stats = L"Rendered: " + to_wstring(renderedCount) + L"/" + to_wstring(m_TileMap.size());
    TextOut(hdc, 10, 70, stats.c_str(), stats.length());
}

void TileMapEditor::RenderRoomBounds(HDC hdc)
{
    // 방 타입에 따른 테두리 색상
    COLORREF boundsColor;
    switch (m_RoomType)
    {
    case RoomType::ENTRANCE: boundsColor = RGB(0, 255, 0);   break; // 초록색
    case RoomType::BATTLE:   boundsColor = RGB(255, 255, 0); break; // 노란색
    case RoomType::EXIT:     boundsColor = RGB(0, 255, 255); break; // 청록색
    case RoomType::BOSS:     boundsColor = RGB(255, 0, 255); break; // 마젠타
    default:                 boundsColor = RGB(255, 0, 0);   break; // 빨간색
    }

    HPEN boundsPen = CreatePen(PS_SOLID, 3, boundsColor); // 두께 3으로 증가
    HPEN oldPen = (HPEN)SelectObject(hdc, boundsPen);

    // 방 경계 계산
    float worldLeft = 0;
    float worldTop = 0;
    float worldRight = m_RoomWidth * GRID_SIZE;
    float worldBottom = m_RoomHeight * GRID_SIZE;

    // 화면 좌표로 변환
    POINT topLeft = m_pCamera->WorldToScreen(worldLeft, worldTop);
    POINT topRight = m_pCamera->WorldToScreen(worldRight, worldTop);
    POINT bottomLeft = m_pCamera->WorldToScreen(worldLeft, worldBottom);
    POINT bottomRight = m_pCamera->WorldToScreen(worldRight, worldBottom);

    // 방 경계 그리기
    MoveToEx(hdc, topLeft.x, topLeft.y, NULL);
    LineTo(hdc, topRight.x, topRight.y);
    LineTo(hdc, bottomRight.x, bottomRight.y);
    LineTo(hdc, bottomLeft.x, bottomLeft.y);
    LineTo(hdc, topLeft.x, topLeft.y);

    SelectObject(hdc, oldPen);
    DeleteObject(boundsPen);
}

void TileMapEditor::RenderRoomInfo(HDC hdc)
{
    // 방 타입에 따른 색상 설정
    COLORREF typeColor;
    switch (m_RoomType)
    {
    case RoomType::ENTRANCE: typeColor = RGB(0, 255, 0);   break; // 초록색
    case RoomType::BATTLE:   typeColor = RGB(255, 255, 0); break; // 노란색
    case RoomType::EXIT:     typeColor = RGB(0, 255, 255); break; // 청록색
    case RoomType::BOSS:     typeColor = RGB(255, 0, 255); break; // 마젠타
    default:                 typeColor = RGB(255, 255, 255); break;
    }

    // 방 타입 텍스트 표시
    SetTextColor(hdc, typeColor);
    SetBkMode(hdc, TRANSPARENT);

    wstring roomInfo = L"Room Type: " + GetRoomTypeName() + L" (ID: " + to_wstring(GetRoomTypeID()) + L")";

    // 화면 우상단에 표시
    RECT clientRect;
    GetClientRect(GetActiveWindow(), &clientRect);
    int textX = clientRect.right - 300;
    int textY = 10;

    TextOut(hdc, textX, textY, roomInfo.c_str(), roomInfo.length());

    // 방 크기 정보도 함께 표시
    SetTextColor(hdc, RGB(200, 200, 200));
    wstring sizeInfo = L"Size: " + to_wstring(m_RoomWidth) + L" x " + to_wstring(m_RoomHeight) + L" grids";
    TextOut(hdc, textX, textY + 20, sizeInfo.c_str(), sizeInfo.length());
}

void TileMapEditor::RenderLayerInfo(HDC hdc)
{
    RECT clientRect;
    GetClientRect(GetActiveWindow(), &clientRect);

    // 현재 레이어 정보
    SetTextColor(hdc, RGB(255, 255, 0));
    SetBkMode(hdc, TRANSPARENT);

    wstring currentLayerInfo = L"Current Layer: " + GetLayerName();
    TextOut(hdc, 10, 110, currentLayerInfo.c_str(), currentLayerInfo.length());

    // 레이어 가시성 정보
    int yOffset = 130;
    SetTextColor(hdc, RGB(200, 200, 200));

    TextOut(hdc, 10, yOffset, L"Layer Visibility:", 17);
    yOffset += 20;

    wstring layers[] = { L"1.Ground", L"2.Decoration", L"3.Cliff", L"4.Collider" };
    LayerType layerTypes[] = { LayerType::GROUND, LayerType::DECORATION, LayerType::CLIFF, LayerType::COLLIDER };

    for (int i = 0; i < 4; i++)
    {
        bool visible = IsLayerVisible(layerTypes[i]);
        SetTextColor(hdc, visible ? RGB(0, 255, 0) : RGB(255, 0, 0));

        wstring visibilityText = layers[i] + (visible ? L" [ON]" : L" [OFF]");
        TextOut(hdc, 20, yOffset, visibilityText.c_str(), visibilityText.length());
        yOffset += 15;
    }
}

void TileMapEditor::SetRoomType(RoomType type)
{
    m_RoomType = type;
    wcout << L"방 타입이 " << GetRoomTypeName() << L"으로 변경되었습니다." << endl;
}

wstring TileMapEditor::GetRoomTypeName() const
{
    switch (m_RoomType)
    {
    case RoomType::ENTRANCE: return L"입구방";
    case RoomType::BATTLE:   return L"전투방";
    case RoomType::EXIT:     return L"출구방";
    case RoomType::BOSS:     return L"보스방";
    default:                 return L"입구방"; // 기본값 설정
    }
}

void TileMapEditor::SetCurrentLayer(LayerType layer)
{
    m_CurrentLayer = layer;
    wcout << L"현재 레이어: " << GetLayerName() << endl;
}

wstring TileMapEditor::GetLayerName() const
{
    switch (m_CurrentLayer)
    {
    case LayerType::GROUND:     return L"그라운드";
    case LayerType::DECORATION: return L"장식";
    case LayerType::CLIFF:      return L"낭떠러지";
    case LayerType::COLLIDER:   return L"콜라이더";
    default:                    return L"알 수 없음";
    }
}

void TileMapEditor::ToggleLayerVisibility(LayerType layer)
{
    m_LayerVisibility[layer] = !m_LayerVisibility[layer];

    wstring layerName;
    switch (layer)
    {
    case LayerType::GROUND:     layerName = L"그라운드"; break;
    case LayerType::DECORATION: layerName = L"장식"; break;
    case LayerType::CLIFF:      layerName = L"낭떠러지"; break;
    case LayerType::COLLIDER:   layerName = L"콜라이더"; break;
    }

    wcout << layerName << L" 레이어 " << (m_LayerVisibility[layer] ? L"표시" : L"숨김") << endl;
}

bool TileMapEditor::IsLayerVisible(LayerType layer) const
{
    auto it = m_LayerVisibility.find(layer);
    return (it != m_LayerVisibility.end()) ? it->second : true;
}

void TileMapEditor::SaveMapWithLayers(const wstring& filename)
{
    wcout << L"저장 중: " << filename << endl;

    wofstream file(filename, ios::out | ios::trunc); // 파일을 완전히 덮어쓰기
    if (!file.is_open())
    {
        wcout << L"❌ 파일 열기 실패: " << filename << endl;
        return;
    }

    try
    {
        // UTF-8 BOM 추가 (선택사항)
        file.imbue(locale(""));

        // 헤더 정보 작성
        cout << "헤더 정보 작성 중..." << endl;
        file << L"# Tile Map Data with Layers" << endl;
        file << L"# Generated by Map Editor" << endl;
        file << L"GridSize=" << GRID_SIZE << endl;
        file << L"RoomWidth=" << m_RoomWidth << endl;
        file << L"RoomHeight=" << m_RoomHeight << endl;
        file << L"RoomType=" << (int)m_RoomType << endl;

        // 방 타입 이름 작성 (강제로 설정)
        wstring roomTypeName;
        switch (m_RoomType)
        {
        case RoomType::ENTRANCE: roomTypeName = L"입구방"; break;
        case RoomType::BATTLE:   roomTypeName = L"전투방"; break;
        case RoomType::EXIT:     roomTypeName = L"출구방"; break;
        case RoomType::BOSS:     roomTypeName = L"보스방"; break;
        default:                 roomTypeName = L"입구방"; break;
        }
        file << L"RoomTypeName=" << roomTypeName << endl;
        file << endl;

        file.flush(); // 버퍼 강제 플러시
        cout << "✓ 헤더 정보 작성 완료" << endl;

        // 그라운드 레이어 저장
        cout << "그라운드 레이어 저장 중..." << endl;
        file << L"[GroundLayer]" << endl;
        file << L"TileCount=" << m_LayerData.groundLayer.size() << endl;

        int groundCount = 0;
        for (const auto& tilePair : m_LayerData.groundLayer)
        {
            const TileData& tile = tilePair.second;
            if (!tile.isEmpty && !tile.tileName.empty())
            {
                file << tile.tileName << L" " << tilePair.first.first << L" " << tilePair.first.second << endl;
                groundCount++;
            }
        }
        file << endl;
        file.flush();
        cout << "✓ 그라운드 레이어 저장 완료: " << groundCount << "개" << endl;

        // 장식 레이어 저장
        cout << "장식 레이어 저장 중..." << endl;
        file << L"[DecorationLayer]" << endl;
        file << L"TileCount=" << m_LayerData.decorationLayer.size() << endl;

        int decorationCount = 0;
        for (const auto& tilePair : m_LayerData.decorationLayer)
        {
            const TileData& tile = tilePair.second;
            if (!tile.isEmpty && !tile.tileName.empty())
            {
                file << tile.tileName << L" " << tilePair.first.first << L" " << tilePair.first.second << endl;
                decorationCount++;
            }
        }
        file << endl;
        file.flush();
        cout << "✓ 장식 레이어 저장 완료: " << decorationCount << "개" << endl;

        // 낭떠러지 레이어 저장
        cout << "낭떠러지 레이어 저장 중..." << endl;
        file << L"[CliffLayer]" << endl;

        int cliffCount = 0;
        for (int y = 0; y < m_RoomHeight; y++)
        {
            wstring line = L"";
            for (int x = 0; x < m_RoomWidth; x++)
            {
                pair<int, int> gridPos = make_pair(x, y);
                auto it = m_LayerData.cliffLayer.find(gridPos);
                bool hasCliff = (it != m_LayerData.cliffLayer.end() && it->second);
                line += (hasCliff ? L"1" : L"0");
                if (hasCliff) cliffCount++;
            }
            file << line << endl;
        }
        file << endl;
        file.flush();
        cout << "✓ 낭떠러지 레이어 저장 완료: " << cliffCount << "개" << endl;

        // 콜라이더 레이어 저장
        cout << "콜라이더 레이어 저장 중..." << endl;
        file << L"[ColliderLayer]" << endl;

        int colliderCount = 0;
        for (int y = 0; y < m_RoomHeight; y++)
        {
            wstring line = L"";
            for (int x = 0; x < m_RoomWidth; x++)
            {
                pair<int, int> gridPos = make_pair(x, y);
                auto it = m_LayerData.colliderLayer.find(gridPos);
                bool hasCollider = (it != m_LayerData.colliderLayer.end() && it->second);
                line += (hasCollider ? L"1" : L"0");
                if (hasCollider) colliderCount++;
            }
            file << line << endl;
        }

        file.flush();
        file.close();
        cout << "✓ 콜라이더 레이어 저장 완료: " << colliderCount << "개" << endl;

        // 저장 완료 메시지
        wcout << L"✅ 맵 저장 성공: " << filename << endl;
        wcout << L"📊 저장 통계:" << endl;
        wcout << L"   - 방 타입: " << roomTypeName << endl;
        wcout << L"   - 그라운드 타일: " << groundCount << L"개" << endl;
        wcout << L"   - 장식 타일: " << decorationCount << L"개" << endl;
        wcout << L"   - 낭떠러지: " << cliffCount << L"개" << endl;
        wcout << L"   - 콜라이더: " << colliderCount << L"개" << endl;
    }
    catch (const exception& e)
    {
        cout << "❌ 저장 중 오류 발생: " << e.what() << endl;
        file.close();
    }
}

void TileMapEditor::SaveMap()
{
    // 디렉토리 생성
    filesystem::create_directories(L"Maps");

    // 타임스탬프 기반 파일명 생성
    time_t now = time(0);
    tm* timeinfo = localtime(&now);

    wchar_t buffer[256];
    wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"Maps/map_%Y%m%d_%H%M%S.txt", timeinfo);
    wstring filename = wstring(buffer);

    cout << "=== 맵 저장 시작 ===" << endl;
    wcout << L"저장 파일: " << filename << endl;

    // 메인 파일로 저장
    SaveMapWithLayers(filename);

    // latest_map.txt로도 저장
    SaveMapWithLayers(L"Maps/latest_map.txt");

    cout << "=== 맵 저장 완료 ===" << endl;
}

void TileMapEditor::LoadMap()
{
    wstring filename = L"Maps/latest_map.txt";

    // 파일 존재 확인
    if (!filesystem::exists(filename))
    {
        wcout << L"❌ 파일이 존재하지 않습니다: " << filename << endl;
        return;
    }

    LoadMapWithLayers(filename);
}

void TileMapEditor::SetRoomBounds(int width, int height)
{
    m_RoomWidth = max(10, min(100, width));   // 10~100 그리드로 제한
    m_RoomHeight = max(10, min(100, height)); // 10~100 그리드로 제한
    cout << "방 크기 설정: " << m_RoomWidth << " x " << m_RoomHeight << " 그리드" << endl;
}

bool TileMapEditor::IsWithinRoomBounds(int gridX, int gridY) const
{
    return (gridX >= 0 && gridX < m_RoomWidth && gridY >= 0 && gridY < m_RoomHeight);
}

void TileMapEditor::LoadMapWithLayers(const wstring& filename)
{
    wifstream file(filename);
    if (!file.is_open())
    {
        wcout << L"파일 로드 실패: " << filename << endl;
        return;
    }

    // 모든 레이어 데이터 초기화
    m_LayerData.groundLayer.clear();
    m_LayerData.decorationLayer.clear();
    m_LayerData.cliffLayer.clear();
    m_LayerData.colliderLayer.clear();

    wstring line;
    wstring currentSection = L"";
    int loadedGroundTiles = 0;
    int loadedDecorationTiles = 0;
    int currentRow = 0;

    while (getline(file, line))
    {
        // 주석이나 빈 줄 건너뛰기
        if (line.empty() || line[0] == L'#') continue;

        // 섹션 구분
        if (line[0] == L'[' && line.back() == L']')
        {
            currentSection = line.substr(1, line.length() - 2);
            currentRow = 0; // 새 섹션에서 행 카운터 리셋
            continue;
        }

        // 설정 라인들
        if (line.find(L"GridSize=") == 0) continue;
        if (line.find(L"RoomWidth=") == 0)
        {
            m_RoomWidth = stoi(line.substr(10));
            continue;
        }
        if (line.find(L"RoomHeight=") == 0)
        {
            m_RoomHeight = stoi(line.substr(11));
            continue;
        }
        if (line.find(L"RoomType=") == 0)
        {
            int typeID = stoi(line.substr(9));
            m_RoomType = static_cast<RoomType>(typeID);
            continue;
        }
        if (line.find(L"RoomTypeName=") == 0 || line.find(L"TileCount=") == 0)
        {
            continue;
        }

        // 섹션별 데이터 처리
        if (currentSection == L"GroundLayer")
        {
            wistringstream iss(line);
            wstring tileName;
            int gridX, gridY;

            if (iss >> tileName >> gridX >> gridY)
            {
                pair<int, int> gridPos = make_pair(gridX, gridY);
                m_LayerData.groundLayer[gridPos] = TileData(tileName, gridX * GRID_SIZE, gridY * GRID_SIZE, LayerType::GROUND);
                loadedGroundTiles++;
            }
        }
        else if (currentSection == L"DecorationLayer")
        {
            wistringstream iss(line);
            wstring tileName;
            int gridX, gridY;

            if (iss >> tileName >> gridX >> gridY)
            {
                pair<int, int> gridPos = make_pair(gridX, gridY);
                m_LayerData.decorationLayer[gridPos] = TileData(tileName, gridX * GRID_SIZE, gridY * GRID_SIZE, LayerType::DECORATION);
                loadedDecorationTiles++;
            }
        }
        else if (currentSection == L"CliffLayer")
        {
            for (int x = 0; x < min((int)line.length(), m_RoomWidth); x++)
            {
                if (line[x] == L'1')
                {
                    pair<int, int> gridPos = make_pair(x, currentRow);
                    m_LayerData.cliffLayer[gridPos] = true;
                }
            }
            currentRow++;
        }
        else if (currentSection == L"ColliderLayer")
        {
            for (int x = 0; x < min((int)line.length(), m_RoomWidth); x++)
            {
                if (line[x] == L'1')
                {
                    pair<int, int> gridPos = make_pair(x, currentRow);
                    m_LayerData.colliderLayer[gridPos] = true;
                }
            }
            currentRow++;
        }
    }

    file.close();

    wcout << L"맵 로드 완료: " << filename << endl;
    wcout << L"방 타입: " << GetRoomTypeName() << endl;
    wcout << L"그라운드: " << loadedGroundTiles << L"개, 장식: " << loadedDecorationTiles << L"개" << endl;
    wcout << L"낭떠러지: " << m_LayerData.cliffLayer.size() << L"개, 콜라이더: " << m_LayerData.colliderLayer.size() << L"개" << endl;
}

wstring TileMapEditor::GenerateMapFileName()
{
    filesystem::create_directories(L"Maps");

    time_t now = time(0);
    tm* timeinfo = localtime(&now);

    wchar_t buffer[256];
    wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"Maps/map_%Y%m%d_%H%M%S.txt", timeinfo);

    return wstring(buffer);
}

void TileMapEditor::Release()
{
    m_LayerData.groundLayer.clear();
    m_LayerData.decorationLayer.clear();
    m_LayerData.cliffLayer.clear();
    m_LayerData.colliderLayer.clear();
}