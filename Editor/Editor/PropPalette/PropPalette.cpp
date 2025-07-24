#include "../../pch.h"
#include "PropPalette.h"
#include "../../Managers/InputManager/InputManager.h"
#include "../../Managers/ResourceManager/ResourceManager.h"

PropPalette::PropPalette()
    : m_SelectedProp(L"")
    , m_SelectedIndex(0)
    , m_PaletteX(1000)
    , m_PaletteY(50)
    , m_PaletteWidth(250)
    , m_PaletteHeight(400)
{
}

PropPalette::~PropPalette()
{
    Release();
}

void PropPalette::Initialize()
{
    m_PropList = ResourceManager::GetInstance()->GetPropNames();
    if (!m_PropList.empty())
    {
        m_SelectedProp = m_PropList[0];
    }
    cout << "프롭 팔레트에 " << m_PropList.size() << "개 프롭 로드됨" << endl;
}

void PropPalette::Update()
{
    auto inputMgr = InputManager::GetInstance();

    if (inputMgr->IsKeyDown(VK_LBUTTON))
    {
        POINT mousePos = inputMgr->GetMousePosition();

        if (mousePos.x >= m_PaletteX && mousePos.x < m_PaletteX + m_PaletteWidth &&
            mousePos.y >= m_PaletteY && mousePos.y < m_PaletteY + m_PaletteHeight)
        {
            int propIndex = (mousePos.y - m_PaletteY - 30) / 20; // 20픽셀 간격
            if (propIndex >= 0 && propIndex < m_PropList.size())
            {
                m_SelectedProp = m_PropList[propIndex];
                m_SelectedIndex = propIndex;
                wcout << L"선택된 프롭: " << m_SelectedProp << endl;
            }
        }
    }
}

void PropPalette::Render(HDC hdc)
{
    if (m_PropList.empty()) return;

    // 팔레트 배경
    RECT paletteRect = { m_PaletteX, m_PaletteY,
                        m_PaletteX + m_PaletteWidth, m_PaletteY + m_PaletteHeight };
    FillRect(hdc, &paletteRect, (HBRUSH)GetStockObject(DKGRAY_BRUSH));
    FrameRect(hdc, &paletteRect, (HBRUSH)GetStockObject(WHITE_BRUSH));

    // 제목
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);
    TextOut(hdc, m_PaletteX + 5, m_PaletteY + 5, L"Prop Palette", 12);

    // 프롭 리스트
    for (int i = 0; i < min((int)m_PropList.size(), 15); i++)
    {
        int textY = m_PaletteY + 30 + i * 20;

        if (i == m_SelectedIndex)
        {
            SetTextColor(hdc, RGB(255, 255, 0)); // 선택된 항목 강조
        }
        else
        {
            SetTextColor(hdc, RGB(200, 200, 200));
        }

        TextOut(hdc, m_PaletteX + 10, textY, m_PropList[i].c_str(), m_PropList[i].length());
    }

    // 선택된 프롭 정보
    if (!m_SelectedProp.empty())
    {
        SetTextColor(hdc, RGB(255, 255, 255));
        wstring selectedInfo = L"Selected: " + m_SelectedProp;
        TextOut(hdc, m_PaletteX + 5, m_PaletteY + m_PaletteHeight + 10,
            selectedInfo.c_str(), selectedInfo.length());
    }
}

void PropPalette::Release()
{
    m_PropList.clear();
}