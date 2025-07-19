#include "../../pch.h"
#include "TilePalette.h"
#include "../TileMapEditor/TileMapEditor.h"
#include "../../Managers/InputManager/InputManager.h"

TilePalette::TilePalette()
    : m_pTileMapEditor(nullptr), m_PosX(600), m_PosY(50),
    m_Width(200), m_Height(400), m_SelectedTileIndex(0),
    m_CurrentPage(0), m_TilesPerPage(TILES_PER_PAGE)
{
}

TilePalette::~TilePalette()
{
}

void TilePalette::Init(TileMapEditor* tileMapEditor)
{
    m_pTileMapEditor = tileMapEditor;

    // ResourceManager에서 타일 파일명 목록 가져오기
    ResourceManager* resourceManager = ResourceManager::GetInstance();
    m_TileFileNames = resourceManager->GetTileFileNames();

    printf("TilePalette initialized with %zu tiles\n", m_TileFileNames.size());

    // 기본 타일 선택
    if (!m_TileFileNames.empty())
    {
        SelectTile(0);
    }
}

void TilePalette::Update()
{
    HandleInput();
}

void TilePalette::HandleInput()
{
    InputManager* input = InputManager::GetInstance();
    POINT mousePos = input->GetMousePosition();

    // 마우스가 팔레트 영역 안에 있는지 확인
    if (mousePos.x >= m_PosX && mousePos.x < m_PosX + m_Width &&
        mousePos.y >= m_PosY && mousePos.y < m_PosY + m_Height)
    {
        // 클릭으로 타일 선택
        if (input->IsKeyDown(VK_LBUTTON))
        {
            // 페이지 버튼 체크
            if (CheckPageButtons(mousePos.x, mousePos.y))
            {
                return; // 페이지 버튼을 눌렀으면 타일 선택 안함
            }

            int tileIndex = GetTileIndexFromPosition(mousePos.x, mousePos.y);
            if (tileIndex >= 0)
            {
                SelectTile(tileIndex);
            }
        }
    }
}

bool TilePalette::CheckPageButtons(int mouseX, int mouseY)
{
    // 이전 페이지 버튼 (< 버튼)
    RECT prevButtonRect = { m_PosX + 5, m_PosY + m_Height - 40, m_PosX + 35, m_PosY + m_Height - 20 };
    if (mouseX >= prevButtonRect.left && mouseX <= prevButtonRect.right &&
        mouseY >= prevButtonRect.top && mouseY <= prevButtonRect.bottom)
    {
        PrevPage();
        return true;
    }

    // 다음 페이지 버튼 (> 버튼)
    RECT nextButtonRect = { m_PosX + m_Width - 35, m_PosY + m_Height - 40, m_PosX + m_Width - 5, m_PosY + m_Height - 20 };
    if (mouseX >= nextButtonRect.left && mouseX <= nextButtonRect.right &&
        mouseY >= nextButtonRect.top && mouseY <= nextButtonRect.bottom)
    {
        NextPage();
        return true;
    }

    return false;
}

void TilePalette::Render(HDC hdc)
{
    // 팔레트 배경 그리기
    HBRUSH bgBrush = CreateSolidBrush(RGB(64, 64, 64));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, bgBrush);

    Rectangle(hdc, m_PosX, m_PosY, m_PosX + m_Width, m_PosY + m_Height);

    SelectObject(hdc, oldBrush);
    DeleteObject(bgBrush);

    // 타일 그리드 그리기
    DrawTileGrid(hdc);

    // 선택된 타일 하이라이트
    DrawSelectedHighlight(hdc);

    // 페이지 정보 표시
    DrawPageInfo(hdc);
}

void TilePalette::DrawTileGrid(HDC hdc)
{
    ResourceManager* resourceManager = ResourceManager::GetInstance();

    int startIndex = m_CurrentPage * m_TilesPerPage;
    int endIndex = min(startIndex + m_TilesPerPage, static_cast<int>(m_TileFileNames.size()));

    for (int i = startIndex; i < endIndex; ++i)
    {
        int relativeIndex = i - startIndex;
        int row = relativeIndex / TILES_PER_ROW;
        int col = relativeIndex % TILES_PER_ROW;

        int tileX = m_PosX + 5 + col * (DEFAULT_TILE_SIZE + 2);
        int tileY = m_PosY + 25 + row * (DEFAULT_TILE_SIZE + 2);

        // 타일 배경
        HBRUSH tileBgBrush = CreateSolidBrush(RGB(128, 128, 128));
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, tileBgBrush);

        Rectangle(hdc, tileX, tileY, tileX + DEFAULT_TILE_SIZE, tileY + DEFAULT_TILE_SIZE);

        SelectObject(hdc, oldBrush);
        DeleteObject(tileBgBrush);

        // 타일 이미지 그리기
        Gdiplus::Bitmap* bitmap = resourceManager->GetTileBitmap(m_TileFileNames[i]);
        if (bitmap)
        {
            // 타일이 16x16보다 큰 경우 첫 번째 16x16 부분만 표시
            int srcWidth = min(16, static_cast<int>(bitmap->GetWidth()));
            int srcHeight = min(16, static_cast<int>(bitmap->GetHeight()));

            resourceManager->DrawBitmapSection(hdc, bitmap,
                tileX, tileY, DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE,
                0, 0, srcWidth, srcHeight);
        }
        else
        {
            // 이미지가 없는 경우 타일 인덱스 표시
            SetTextColor(hdc, RGB(255, 255, 255));
            SetBkMode(hdc, TRANSPARENT);

            char indexText[8];
            sprintf_s(indexText, "%d", i);
            TextOutA(hdc, tileX + 5, tileY + 5, indexText, strlen(indexText));
        }
    }
}

void TilePalette::DrawSelectedHighlight(HDC hdc)
{
    if (m_SelectedTileIndex < 0 || m_SelectedTileIndex >= static_cast<int>(m_TileFileNames.size()))
        return;

    int startIndex = m_CurrentPage * m_TilesPerPage;
    int endIndex = min(startIndex + m_TilesPerPage, static_cast<int>(m_TileFileNames.size()));

    // 선택된 타일이 현재 페이지에 있는지 확인
    if (m_SelectedTileIndex < startIndex || m_SelectedTileIndex >= endIndex)
        return;

    int relativeIndex = m_SelectedTileIndex - startIndex;
    int row = relativeIndex / TILES_PER_ROW;
    int col = relativeIndex % TILES_PER_ROW;

    int tileX = m_PosX + 5 + col * (DEFAULT_TILE_SIZE + 2);
    int tileY = m_PosY + 25 + row * (DEFAULT_TILE_SIZE + 2);

    // 선택 테두리 그리기
    HPEN highlightPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
    HPEN oldPen = (HPEN)SelectObject(hdc, highlightPen);

    HBRUSH nullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, nullBrush);

    Rectangle(hdc, tileX - 1, tileY - 1,
        tileX + DEFAULT_TILE_SIZE + 1, tileY + DEFAULT_TILE_SIZE + 1);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(highlightPen);
}

void TilePalette::DrawPageInfo(HDC hdc)
{
    int totalPages = (static_cast<int>(m_TileFileNames.size()) + m_TilesPerPage - 1) / m_TilesPerPage;

    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);

    // 페이지 정보
    char pageText[64];
    sprintf_s(pageText, "Page %d/%d", m_CurrentPage + 1, max(1, totalPages));
    TextOutA(hdc, m_PosX + 5, m_PosY + 5, pageText, strlen(pageText));

    // 페이지 버튼들 그리기
    DrawPageButtons(hdc, totalPages);

    // 선택된 타일 정보
    if (m_SelectedTileIndex >= 0 && m_SelectedTileIndex < static_cast<int>(m_TileFileNames.size()))
    {
        // 파일명을 char로 변환 (간단한 변환)
        wstring fileName = m_TileFileNames[m_SelectedTileIndex];
        string fileNameA(fileName.begin(), fileName.end());

        char selectedText[128];
        sprintf_s(selectedText, "Selected: %s", fileNameA.c_str());
        TextOutA(hdc, m_PosX + 5, m_PosY + m_Height - 60, selectedText, strlen(selectedText));
    }
}

void TilePalette::DrawPageButtons(HDC hdc, int totalPages)
{
    // 이전 페이지 버튼
    RECT prevButtonRect = { m_PosX + 5, m_PosY + m_Height - 40, m_PosX + 35, m_PosY + m_Height - 20 };
    HBRUSH buttonBrush = CreateSolidBrush(m_CurrentPage > 0 ? RGB(100, 100, 100) : RGB(60, 60, 60));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, buttonBrush);

    Rectangle(hdc, prevButtonRect.left, prevButtonRect.top, prevButtonRect.right, prevButtonRect.bottom);

    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);
    TextOutA(hdc, prevButtonRect.left + 8, prevButtonRect.top + 2, "<", 1);

    SelectObject(hdc, oldBrush);
    DeleteObject(buttonBrush);

    // 다음 페이지 버튼
    RECT nextButtonRect = { m_PosX + m_Width - 35, m_PosY + m_Height - 40, m_PosX + m_Width - 5, m_PosY + m_Height - 20 };
    buttonBrush = CreateSolidBrush(m_CurrentPage < totalPages - 1 ? RGB(100, 100, 100) : RGB(60, 60, 60));
    oldBrush = (HBRUSH)SelectObject(hdc, buttonBrush);

    Rectangle(hdc, nextButtonRect.left, nextButtonRect.top, nextButtonRect.right, nextButtonRect.bottom);

    TextOutA(hdc, nextButtonRect.left + 8, nextButtonRect.top + 2, ">", 1);

    SelectObject(hdc, oldBrush);
    DeleteObject(buttonBrush);
}

void TilePalette::SelectTile(int index)
{
    if (index < 0 || index >= static_cast<int>(m_TileFileNames.size()))
        return;

    m_SelectedTileIndex = index;

    // 선택된 타일이 현재 페이지에 있지 않으면 페이지 변경
    int targetPage = index / m_TilesPerPage;
    if (targetPage != m_CurrentPage)
    {
        m_CurrentPage = targetPage;
    }

    // TileMapEditor에 선택된 타일 정보 전달
    if (m_pTileMapEditor)
    {
        // 기본적으로 FLOOR 타입으로 설정, 필요에 따라 파일명에 따라 타입 결정 가능
        m_pTileMapEditor->SetSelectedTile(TileType::FLOOR, index);
    }

    printf("Selected tile %d: %ws\n", index, m_TileFileNames[index].c_str());
}

int TilePalette::GetTileIndexFromPosition(int x, int y)
{
    int relativeX = x - (m_PosX + 5);
    int relativeY = y - (m_PosY + 25);

    if (relativeX < 0 || relativeY < 0)
        return -1;

    int col = relativeX / (DEFAULT_TILE_SIZE + 2);
    int row = relativeY / (DEFAULT_TILE_SIZE + 2);

    if (col >= TILES_PER_ROW)
        return -1;

    int relativeIndex = row * TILES_PER_ROW + col;
    int absoluteIndex = m_CurrentPage * m_TilesPerPage + relativeIndex;

    if (absoluteIndex >= static_cast<int>(m_TileFileNames.size()))
        return -1;

    return absoluteIndex;
}

RECT TilePalette::GetTileRect(int index)
{
    int startIndex = m_CurrentPage * m_TilesPerPage;
    int relativeIndex = index - startIndex;

    int row = relativeIndex / TILES_PER_ROW;
    int col = relativeIndex % TILES_PER_ROW;

    RECT rect;
    rect.left = m_PosX + 5 + col * (DEFAULT_TILE_SIZE + 2);
    rect.top = m_PosY + 25 + row * (DEFAULT_TILE_SIZE + 2);
    rect.right = rect.left + DEFAULT_TILE_SIZE;
    rect.bottom = rect.top + DEFAULT_TILE_SIZE;

    return rect;
}

void TilePalette::NextPage()
{
    int totalPages = (static_cast<int>(m_TileFileNames.size()) + m_TilesPerPage - 1) / m_TilesPerPage;
    if (m_CurrentPage < totalPages - 1)
    {
        m_CurrentPage++;
        printf("Tile palette page: %d\n", m_CurrentPage + 1);
    }
}

void TilePalette::PrevPage()
{
    if (m_CurrentPage > 0)
    {
        m_CurrentPage--;
        printf("Tile palette page: %d\n", m_CurrentPage + 1);
    }
}

void TilePalette::SetPosition(int x, int y)
{
    m_PosX = x;
    m_PosY = y;
}

void TilePalette::SetSize(int width, int height)
{
    m_Width = width;
    m_Height = height;
}

void TilePalette::SetTilesPerPage(int tilesPerPage)
{
    m_TilesPerPage = tilesPerPage;
    m_CurrentPage = 0; // 페이지 리셋
}