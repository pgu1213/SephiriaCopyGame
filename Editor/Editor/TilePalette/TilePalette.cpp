#include "../../pch.h"
#include "TilePalette.h"
#include "../../Managers/ResourceManager/ResourceManager.h"
#include "../../Managers/InputManager/InputManager.h"

TilePalette::TilePalette()
    : m_SelectedTileID(0)
    , m_SelectedIndex(-1)
    , m_PosX(10), m_PosY(10)
    , m_Width(200), m_Height(400)
    , m_TileDisplaySize(32)
    , m_TilesPerRow(0)
    , m_ScrollOffset(0)
    , m_MaxScrollOffset(0)
    , m_NeedUpdateLayout(true)
{
}

TilePalette::~TilePalette()
{
}

void TilePalette::Initialize()
{
    LoadTileItems();
    UpdateLayout();
}

void TilePalette::Update()
{
    HandleInput();

    if (m_NeedUpdateLayout)
    {
        UpdateLayout();
        m_NeedUpdateLayout = false;
    }
}

void TilePalette::Render(HDC hdc)
{
    // ��� �׸���
    HBRUSH bgBrush = CreateSolidBrush(RGB(240, 240, 240));
    RECT bgRect = { m_PosX, m_PosY, m_PosX + m_Width, m_PosY + m_Height };
    FillRect(hdc, &bgRect, bgBrush);
    DeleteObject(bgBrush);

    // �׵θ� �׸���
    HPEN borderPen = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
    HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);

    MoveToEx(hdc, bgRect.left, bgRect.top, NULL);
    LineTo(hdc, bgRect.right, bgRect.top);
    LineTo(hdc, bgRect.right, bgRect.bottom);
    LineTo(hdc, bgRect.left, bgRect.bottom);
    LineTo(hdc, bgRect.left, bgRect.top);

    SelectObject(hdc, oldPen);
    DeleteObject(borderPen);

    // Ÿ�� �׸��� ������
    RenderTileGrid(hdc);

    // ���� ǥ��
    RenderSelection(hdc);

    // ���� �ؽ�Ʈ
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    RECT titleRect = { m_PosX + 5, m_PosY + 5, m_PosX + m_Width - 5, m_PosY + 25 };
    DrawTextA(hdc, "Tile Palette", -1, &titleRect, DT_LEFT | DT_TOP);
}

void TilePalette::HandleInput()
{
    POINT mousePos = InputManager::GetInstance()->GetMousePosition();

    // �ȷ�Ʈ ���� �������� ó��
    if (mousePos.x >= m_PosX && mousePos.x <= m_PosX + m_Width &&
        mousePos.y >= m_PosY && mousePos.y <= m_PosY + m_Height)
    {
        if (InputManager::GetInstance()->IsKeyDown(VK_LBUTTON))
        {
            int tileIndex = GetTileIndexAt(mousePos.x, mousePos.y);
            if (tileIndex >= 0 && tileIndex < m_TileItems.size())
            {
                m_SelectedIndex = tileIndex;
                m_SelectedTileID = m_TileItems[tileIndex].tileID;
            }
        }

        // ��ũ�� ó��
        if (InputManager::GetInstance()->IsKeyDown(VK_MBUTTON)) // ���콺 �� ��ư
        {
            // ������ ��ũ�� ���� (�����δ� ���콺 �� �޽����� ó���ؾ� ��)
            static bool scrollUp = false;
            scrollUp = !scrollUp;

            if (scrollUp && m_ScrollOffset > 0)
            {
                m_ScrollOffset--;
                m_NeedUpdateLayout = true;
            }
            else if (!scrollUp && m_ScrollOffset < m_MaxScrollOffset)
            {
                m_ScrollOffset++;
                m_NeedUpdateLayout = true;
            }
        }
    }
}

void TilePalette::LoadTileItems()
{
    m_TileItems.clear();

    auto tileResources = ResourceManager::GetInstance()->GetAllTileResources();

    for (const auto& resource : tileResources)
    {
        TilePaletteItem item;
        item.tileID = resource.id;
        item.fileName = resource.fileName;
        item.bitmap = resource.bitmap;

        m_TileItems.push_back(item);
    }

    m_NeedUpdateLayout = true;
}

void TilePalette::UpdateLayout()
{
    if (m_TileItems.empty()) return;

    // �� �࿡ �� Ÿ�� �� ���
    int availableWidth = m_Width - 20; // ���� ����
    m_TilesPerRow = max(1, availableWidth / (m_TileDisplaySize + 5));

    // �� Ÿ���� ǥ�� ��ġ ���
    int contentStartY = m_PosY + 30; // ���� ���� ����
    int availableHeight = m_Height - 40;

    for (int i = 0; i < m_TileItems.size(); ++i)
    {
        int row = (i / m_TilesPerRow) - m_ScrollOffset;
        int col = i % m_TilesPerRow;

        int x = m_PosX + 10 + col * (m_TileDisplaySize + 5);
        int y = contentStartY + row * (m_TileDisplaySize + 5);

        m_TileItems[i].displayRect = { x, y, x + m_TileDisplaySize, y + m_TileDisplaySize };
    }

    // �ִ� ��ũ�� ������ ���
    int totalRows = (m_TileItems.size() + m_TilesPerRow - 1) / m_TilesPerRow;
    int visibleRows = availableHeight / (m_TileDisplaySize + 5);
    m_MaxScrollOffset = max(0, totalRows - visibleRows);
}

void TilePalette::RenderTileGrid(HDC hdc)
{
    for (int i = 0; i < m_TileItems.size(); ++i)
    {
        const auto& item = m_TileItems[i];
        const RECT& rect = item.displayRect;

        // ȭ�鿡 ���̴� Ÿ�ϸ� ������
        if (rect.top >= m_PosY + 30 && rect.bottom <= m_PosY + m_Height)
        {
            if (item.bitmap)
            {
                ResourceManager::GetInstance()->DrawBitmap(hdc, item.bitmap,
                    rect.left, rect.top,
                    rect.right - rect.left, rect.bottom - rect.top);
            }

            // Ÿ�� �׵θ�
            HPEN tilePen = CreatePen(PS_SOLID, 1, RGB(150, 150, 150));
            HPEN oldPen = (HPEN)SelectObject(hdc, tilePen);

            MoveToEx(hdc, rect.left, rect.top, NULL);
            LineTo(hdc, rect.right, rect.top);
            LineTo(hdc, rect.right, rect.bottom);
            LineTo(hdc, rect.left, rect.bottom);
            LineTo(hdc, rect.left, rect.top);

            SelectObject(hdc, oldPen);
            DeleteObject(tilePen);

            // Ÿ�� ID ǥ�� (���� �۾���)
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(255, 255, 255));
            RECT idRect = { rect.left + 2, rect.top + 2, rect.right - 2, rect.top + 15 };
            string idText = to_string(item.tileID);
            DrawTextA(hdc, idText.c_str(), -1, &idRect, DT_LEFT | DT_TOP);
        }
    }
}

void TilePalette::RenderSelection(HDC hdc)
{
    if (m_SelectedIndex >= 0 && m_SelectedIndex < m_TileItems.size())
    {
        const RECT& rect = m_TileItems[m_SelectedIndex].displayRect;

        // ���õ� Ÿ�Ͽ� ���� �׵θ�
        HPEN selectionPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
        HPEN oldPen = (HPEN)SelectObject(hdc, selectionPen);

        MoveToEx(hdc, rect.left - 1, rect.top - 1, NULL);
        LineTo(hdc, rect.right + 1, rect.top - 1);
        LineTo(hdc, rect.right + 1, rect.bottom + 1);
        LineTo(hdc, rect.left - 1, rect.bottom + 1);
        LineTo(hdc, rect.left - 1, rect.top - 1);

        SelectObject(hdc, oldPen);
        DeleteObject(selectionPen);
    }
}

int TilePalette::GetTileIndexAt(int x, int y)
{
    for (int i = 0; i < m_TileItems.size(); ++i)
    {
        const RECT& rect = m_TileItems[i].displayRect;
        if (x >= rect.left && x <= rect.right && y >= rect.top && y <= rect.bottom)
        {
            return i;
        }
    }
    return -1;
}

wstring TilePalette::GetSelectedTileFileName() const
{
    if (m_SelectedIndex >= 0 && m_SelectedIndex < m_TileItems.size())
    {
        return m_TileItems[m_SelectedIndex].fileName;
    }
    return L"";
}