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

    // ResourceManager���� Ÿ�� ���ϸ� ��� ��������
    ResourceManager* resourceManager = ResourceManager::GetInstance();
    m_TileFileNames = resourceManager->GetTileFileNames();

    printf("TilePalette initialized with %zu tiles\n", m_TileFileNames.size());

    // �⺻ Ÿ�� ����
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

    // ���콺�� �ȷ�Ʈ ���� �ȿ� �ִ��� Ȯ��
    if (mousePos.x >= m_PosX && mousePos.x < m_PosX + m_Width &&
        mousePos.y >= m_PosY && mousePos.y < m_PosY + m_Height)
    {
        // Ŭ������ Ÿ�� ����
        if (input->IsKeyDown(VK_LBUTTON))
        {
            int tileIndex = GetTileIndexFromPosition(mousePos.x, mousePos.y);
            if (tileIndex >= 0)
            {
                SelectTile(tileIndex);
            }
        }
    }

    // ������ ���� (Page Up/Down Ű)
    if (input->IsKeyDown(VK_PRIOR)) // Page Up
    {
        PrevPage();
    }
    if (input->IsKeyDown(VK_NEXT)) // Page Down
    {
        NextPage();
    }

    // Ű����� Ÿ�� ���� (ȭ��ǥ Ű)
    if (input->IsKeyDown(VK_LEFT))
    {
        if (m_SelectedTileIndex > 0)
        {
            SelectTile(m_SelectedTileIndex - 1);
        }
    }
    if (input->IsKeyDown(VK_RIGHT))
    {
        if (m_SelectedTileIndex < static_cast<int>(m_TileFileNames.size()) - 1)
        {
            SelectTile(m_SelectedTileIndex + 1);
        }
    }
    if (input->IsKeyDown(VK_UP))
    {
        if (m_SelectedTileIndex >= TILES_PER_ROW)
        {
            SelectTile(m_SelectedTileIndex - TILES_PER_ROW);
        }
    }
    if (input->IsKeyDown(VK_DOWN))
    {
        if (m_SelectedTileIndex + TILES_PER_ROW < static_cast<int>(m_TileFileNames.size()))
        {
            SelectTile(m_SelectedTileIndex + TILES_PER_ROW);
        }
    }
}

void TilePalette::Render(HDC hdc)
{
    // �ȷ�Ʈ ��� �׸���
    HBRUSH bgBrush = CreateSolidBrush(RGB(64, 64, 64));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, bgBrush);

    Rectangle(hdc, m_PosX, m_PosY, m_PosX + m_Width, m_PosY + m_Height);

    SelectObject(hdc, oldBrush);
    DeleteObject(bgBrush);

    // Ÿ�� �׸��� �׸���
    DrawTileGrid(hdc);

    // ���õ� Ÿ�� ���̶���Ʈ
    DrawSelectedHighlight(hdc);

    // ������ ���� ǥ��
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

        // Ÿ�� ���
        HBRUSH tileBgBrush = CreateSolidBrush(RGB(128, 128, 128));
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, tileBgBrush);

        Rectangle(hdc, tileX, tileY, tileX + DEFAULT_TILE_SIZE, tileY + DEFAULT_TILE_SIZE);

        SelectObject(hdc, oldBrush);
        DeleteObject(tileBgBrush);

        // Ÿ�� �̹��� �׸���
        Gdiplus::Bitmap* bitmap = resourceManager->GetTileBitmap(m_TileFileNames[i]);
        if (bitmap)
        {
            // Ÿ���� 16x16���� ū ��� ù ��° 16x16 �κи� ǥ��
            int srcWidth = min(16, static_cast<int>(bitmap->GetWidth()));
            int srcHeight = min(16, static_cast<int>(bitmap->GetHeight()));

            resourceManager->DrawBitmapSection(hdc, bitmap,
                tileX, tileY, DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE,
                0, 0, srcWidth, srcHeight);
        }
        else
        {
            // �̹����� ���� ��� Ÿ�� �ε��� ǥ��
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

    // ���õ� Ÿ���� ���� �������� �ִ��� Ȯ��
    if (m_SelectedTileIndex < startIndex || m_SelectedTileIndex >= endIndex)
        return;

    int relativeIndex = m_SelectedTileIndex - startIndex;
    int row = relativeIndex / TILES_PER_ROW;
    int col = relativeIndex % TILES_PER_ROW;

    int tileX = m_PosX + 5 + col * (DEFAULT_TILE_SIZE + 2);
    int tileY = m_PosY + 25 + row * (DEFAULT_TILE_SIZE + 2);

    // ���� �׵θ� �׸���
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

    // ������ ����
    char pageText[64];
    sprintf_s(pageText, "Page %d/%d", m_CurrentPage + 1, max(1, totalPages));
    TextOutA(hdc, m_PosX + 5, m_PosY + 5, pageText, strlen(pageText));

    // ���õ� Ÿ�� ����
    if (m_SelectedTileIndex >= 0 && m_SelectedTileIndex < static_cast<int>(m_TileFileNames.size()))
    {
        // ���ϸ��� char�� ��ȯ (������ ��ȯ)
        wstring fileName = m_TileFileNames[m_SelectedTileIndex];
        string fileNameA(fileName.begin(), fileName.end());

        char selectedText[128];
        sprintf_s(selectedText, "Selected: %s", fileNameA.c_str());
        TextOutA(hdc, m_PosX + 5, m_PosY + m_Height - 20, selectedText, strlen(selectedText));
    }
}

void TilePalette::SelectTile(int index)
{
    if (index < 0 || index >= static_cast<int>(m_TileFileNames.size()))
        return;

    m_SelectedTileIndex = index;

    // ���õ� Ÿ���� ���� �������� ���� ������ ������ ����
    int targetPage = index / m_TilesPerPage;
    if (targetPage != m_CurrentPage)
    {
        m_CurrentPage = targetPage;
    }

    // TileMapEditor�� ���õ� Ÿ�� ���� ����
    if (m_pTileMapEditor)
    {
        // �⺻������ FLOOR Ÿ������ ����, �ʿ信 ���� ���ϸ� ���� Ÿ�� ���� ����
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
    m_CurrentPage = 0; // ������ ����
}