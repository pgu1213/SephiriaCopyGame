#include "../../pch.h"
#include "TilePalette.h"
#include "../../Managers/ResourceManager/ResourceManager.h"

TilePalette::TilePalette()
    : m_SelectedTile(L"")
    , m_ScrollOffset(0)
    , m_SelectedIndex(0)
    , m_IsVisible(true)
    , m_PaletteX(1000)
    , m_PaletteY(50)
    , m_PaletteWidth(250)
    , m_PaletteHeight(400)
    , m_TileSize(48)
{
}

TilePalette::~TilePalette()
{
    Release();
}

void TilePalette::Initialize()
{
    LoadTileList();
    if (!m_TileList.empty())
    {
        m_SelectedTile = m_TileList[0];
    }
}

void TilePalette::LoadTileList()
{
    m_TileList = ResourceManager::GetInstance()->GetTileNames();
    cout << "타일 팔레트에 " << m_TileList.size() << "개 타일 로드됨" << endl;
}

void TilePalette::Update()
{
    HandleInput();
}

void TilePalette::HandleInput()
{
    // 팔레트 스크롤
    if (GetAsyncKeyState(VK_PRIOR) & 0x8000) // Page Up
    {
        m_ScrollOffset = max(0, m_ScrollOffset - 1);
    }
    if (GetAsyncKeyState(VK_NEXT) & 0x8000) // Page Down
    {
        int maxScroll = max(0, (int)m_TileList.size() - (m_PaletteHeight / m_TileSize));
        m_ScrollOffset = min(maxScroll, m_ScrollOffset + 1);
    }

    // 마우스 클릭으로 타일 선택
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
    {
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(GetActiveWindow(), &mousePos);

        if (mousePos.x >= m_PaletteX && mousePos.x < m_PaletteX + m_PaletteWidth &&
            mousePos.y >= m_PaletteY && mousePos.y < m_PaletteY + m_PaletteHeight)
        {
            int tileIndex = (mousePos.y - m_PaletteY) / m_TileSize + m_ScrollOffset;
            if (tileIndex >= 0 && tileIndex < m_TileList.size())
            {
                m_SelectedTile = m_TileList[tileIndex];
                m_SelectedIndex = tileIndex;
            }
        }
    }
}

void TilePalette::RenderTileList(HDC hdc)
{
    if (m_TileList.empty()) return;

    // 팔레트 영역 클리어
    RECT paletteRect = { m_PaletteX, m_PaletteY + 30,
                        m_PaletteX + m_PaletteWidth, m_PaletteY + m_PaletteHeight };
    FillRect(hdc, &paletteRect, (HBRUSH)GetStockObject(DKGRAY_BRUSH));

    int tilesPerRow = (m_PaletteWidth - 10) / m_TileSize;
    int startY = m_PaletteY + 35;
    int maxVisibleRows = (m_PaletteHeight - 60) / m_TileSize;

    Graphics graphics(hdc);

    for (int i = m_ScrollOffset; i < m_TileList.size(); i++)
    {
        int relativeIndex = i - m_ScrollOffset;
        int row = relativeIndex / tilesPerRow;
        int col = relativeIndex % tilesPerRow;

        // 표시 가능한 영역을 벗어나면 중단
        if (row >= maxVisibleRows) break;

        int tileX = m_PaletteX + col * m_TileSize + 5;
        int tileY = startY + row * m_TileSize;

        // 타일 배경 (선택된 타일은 다른 색상)
        RECT tileRect = { tileX - 2, tileY - 2,
                         tileX + m_TileSize, tileY + m_TileSize };

        if (i == m_SelectedIndex)
        {
            FillRect(hdc, &tileRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
        }
        else
        {
            FillRect(hdc, &tileRect, (HBRUSH)GetStockObject(GRAY_BRUSH));
        }

        // 타일 이미지 렌더링
        Bitmap* tileSprite = ResourceManager::GetInstance()->GetSprite(m_TileList[i]);
        if (tileSprite)
        {
            // 이미지를 타일 크기에 맞게 스케일링
            graphics.DrawImage(tileSprite, tileX, tileY, m_TileSize - 4, m_TileSize - 4);
        }
        else
        {
            // 이미지가 없는 경우 텍스트로 표시
            SetTextColor(hdc, RGB(255, 255, 255));
            SetBkMode(hdc, TRANSPARENT);

            // 파일명의 앞 4글자만 표시
            wstring shortName = m_TileList[i].substr(0, min(4, (int)m_TileList[i].length()));
            TextOut(hdc, tileX + 2, tileY + m_TileSize / 2 - 8, shortName.c_str(), shortName.length());
        }

        // 선택된 타일 테두리 강조
        if (i == m_SelectedIndex)
        {
            HPEN yellowPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
            HPEN oldPen = (HPEN)SelectObject(hdc, yellowPen);

            Rectangle(hdc, tileX - 2, tileY - 2, tileX + m_TileSize, tileY + m_TileSize);

            SelectObject(hdc, oldPen);
            DeleteObject(yellowPen);
        }
    }

    // 스크롤 인디케이터
    if (m_TileList.size() > maxVisibleRows * tilesPerRow)
    {
        int scrollBarX = m_PaletteX + m_PaletteWidth - 10;
        int scrollBarY = m_PaletteY + 30;
        int scrollBarHeight = m_PaletteHeight - 60;

        // 스크롤바 배경
        RECT scrollRect = { scrollBarX, scrollBarY, scrollBarX + 8, scrollBarY + scrollBarHeight };
        FillRect(hdc, &scrollRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

        // 스크롤바 썸
        int totalRows = (m_TileList.size() + tilesPerRow - 1) / tilesPerRow;
        int thumbHeight = max(10, scrollBarHeight * maxVisibleRows / totalRows);
        int thumbY = scrollBarY + (scrollBarHeight - thumbHeight) * m_ScrollOffset / max(1, totalRows - maxVisibleRows);

        RECT thumbRect = { scrollBarX + 1, thumbY, scrollBarX + 7, thumbY + thumbHeight };
        FillRect(hdc, &thumbRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
    }

    // 현재 페이지 정보 표시
    int totalPages = (m_TileList.size() + (tilesPerRow * maxVisibleRows) - 1) / (tilesPerRow * maxVisibleRows);
    int currentPage = (m_ScrollOffset / maxVisibleRows) + 1;

    if (totalPages > 1)
    {
        SetTextColor(hdc, RGB(200, 200, 200));
        SetBkMode(hdc, TRANSPARENT);

        wstring pageInfo = L"Page " + to_wstring(currentPage) + L"/" + to_wstring(totalPages);
        TextOut(hdc, m_PaletteX + 5, m_PaletteY + m_PaletteHeight - 15,
            pageInfo.c_str(), pageInfo.length());
    }
}

void TilePalette::Render(HDC hdc)
{
    if (!m_IsVisible || m_TileList.empty()) return;

    // 팔레트 배경
    RECT paletteRect = { m_PaletteX, m_PaletteY,
                        m_PaletteX + m_PaletteWidth, m_PaletteY + m_PaletteHeight };
    FillRect(hdc, &paletteRect, (HBRUSH)GetStockObject(GRAY_BRUSH));
    FrameRect(hdc, &paletteRect, (HBRUSH)GetStockObject(WHITE_BRUSH));

    // 제목
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);
    TextOut(hdc, m_PaletteX + 5, m_PaletteY + 5, L"Tile Palette", 12);

    // 타일 리스트 렌더링
    int tilesPerRow = m_PaletteWidth / m_TileSize;
    int startY = m_PaletteY + 30;

    for (int i = m_ScrollOffset; i < m_TileList.size(); i++)
    {
        int row = (i - m_ScrollOffset) / tilesPerRow;
        int col = (i - m_ScrollOffset) % tilesPerRow;

        int tileX = m_PaletteX + col * m_TileSize + 5;
        int tileY = startY + row * m_TileSize;

        if (tileY + m_TileSize > m_PaletteY + m_PaletteHeight) break;

        // 타일 이미지 렌더링
        Bitmap* tileSprite = ResourceManager::GetInstance()->GetSprite(m_TileList[i]);
        if (tileSprite)
        {
            Graphics graphics(hdc);
            graphics.DrawImage(tileSprite, tileX, tileY, m_TileSize - 2, m_TileSize - 2);
        }

        // 선택된 타일 강조
        if (i == m_SelectedIndex)
        {
            RECT selectRect = { tileX - 1, tileY - 1,
                               tileX + m_TileSize - 1, tileY + m_TileSize - 1 };
            FrameRect(hdc, &selectRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
        }
    }

    // 선택된 타일 정보 표시
    if (!m_SelectedTile.empty())
    {
        wstring selectedInfo = L"Selected: " + m_SelectedTile;
        TextOut(hdc, m_PaletteX + 5, m_PaletteY + m_PaletteHeight + 10,
            selectedInfo.c_str(), selectedInfo.length());
    }
}

void TilePalette::Release()
{
    m_TileList.clear();
}