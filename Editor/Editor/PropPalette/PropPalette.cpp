#include "../../pch.h"
#include "PropPalette.h"
#include "../../Managers/InputManager/InputManager.h"

PropPalette::PropPalette()
    : m_PosX(600), m_PosY(370), m_Width(200), m_Height(200),
    m_SelectedPropIndex(0), m_CurrentPage(0)
{
}

PropPalette::~PropPalette()
{
}

void PropPalette::Init()
{
    // ResourceManager에서 프롭 파일명 목록 가져오기
    ResourceManager* resourceManager = ResourceManager::GetInstance();
    m_PropFileNames = resourceManager->GetPropFileNames();

    // 프롭 이름을 영어로 변환
    m_PropNames.clear();
    for (const wstring& fileName : m_PropFileNames)
    {
        string propName = ConvertPropFileName(fileName);
        m_PropNames.push_back(propName);
    }

    printf("PropPalette initialized with %zu props\n", m_PropFileNames.size());

    // 기본 프롭 선택
    if (!m_PropFileNames.empty())
    {
        SelectProp(0);
    }
}

void PropPalette::Update()
{
    HandleInput();
}

void PropPalette::HandleInput()
{
    InputManager* input = InputManager::GetInstance();
    POINT mousePos = input->GetMousePosition();

    // 마우스가 팔레트 영역 안에 있는지 확인
    if (mousePos.x >= m_PosX && mousePos.x < m_PosX + m_Width &&
        mousePos.y >= m_PosY && mousePos.y < m_PosY + m_Height)
    {
        // 클릭으로 프롭 선택
        if (input->IsKeyDown(VK_LBUTTON))
        {
            int propIndex = GetPropIndexFromPosition(mousePos.x, mousePos.y);
            if (propIndex >= 0)
            {
                SelectProp(propIndex);
            }
        }
    }

    // 프롭 모드 토글 (P 키)
    if (input->IsKeyDown('P'))
    {
        printf("Prop placement mode toggled\n");
    }
}

void PropPalette::Render(HDC hdc)
{
    // 팔레트 배경 그리기
    HBRUSH bgBrush = CreateSolidBrush(RGB(80, 80, 80));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, bgBrush);

    Rectangle(hdc, m_PosX, m_PosY, m_PosX + m_Width, m_PosY + m_Height);

    SelectObject(hdc, oldBrush);
    DeleteObject(bgBrush);

    // 제목 표시
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);

    const char* title = "Props";
    TextOutA(hdc, m_PosX + 5, m_PosY + 5, title, strlen(title));

    // 프롭 그리드 그리기
    DrawPropGrid(hdc);

    // 선택된 프롭 하이라이트
    DrawSelectedHighlight(hdc);

    // 페이지 정보 표시
    DrawPageInfo(hdc);
}

void PropPalette::DrawPropGrid(HDC hdc)
{
    ResourceManager* resourceManager = ResourceManager::GetInstance();

    int startIndex = m_CurrentPage * PROPS_PER_PAGE;
    int endIndex = min(startIndex + PROPS_PER_PAGE, static_cast<int>(m_PropFileNames.size()));

    for (int i = startIndex; i < endIndex; ++i)
    {
        int relativeIndex = i - startIndex;
        int row = relativeIndex / PROPS_PER_ROW;
        int col = relativeIndex % PROPS_PER_ROW;

        int propX = m_PosX + 5 + col * (DEFAULT_PROP_SIZE + 2);
        int propY = m_PosY + 25 + row * (DEFAULT_PROP_SIZE + 2);

        // 프롭 배경
        HBRUSH propBgBrush = CreateSolidBrush(RGB(100, 100, 100));
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, propBgBrush);

        Rectangle(hdc, propX, propY, propX + DEFAULT_PROP_SIZE, propY + DEFAULT_PROP_SIZE);

        SelectObject(hdc, oldBrush);
        DeleteObject(propBgBrush);

        // 프롭 이미지 그리기
        Gdiplus::Bitmap* bitmap = resourceManager->GetPropBitmap(m_PropFileNames[i]);
        if (bitmap)
        {
            // 프롭 크기에 맞게 조정해서 표시
            int srcWidth = min(32, static_cast<int>(bitmap->GetWidth()));
            int srcHeight = min(32, static_cast<int>(bitmap->GetHeight()));

            resourceManager->DrawBitmapSection(hdc, bitmap,
                propX, propY, DEFAULT_PROP_SIZE, DEFAULT_PROP_SIZE,
                0, 0, srcWidth, srcHeight);
        }
        else
        {
            // 이미지가 없는 경우 프롭 인덱스 표시
            SetTextColor(hdc, RGB(255, 255, 255));
            SetBkMode(hdc, TRANSPARENT);

            char indexText[8];
            sprintf_s(indexText, "%d", i);
            TextOutA(hdc, propX + 5, propY + 5, indexText, strlen(indexText));
        }
    }
}

void PropPalette::DrawSelectedHighlight(HDC hdc)
{
    if (m_SelectedPropIndex < 0 || m_SelectedPropIndex >= static_cast<int>(m_PropFileNames.size()))
        return;

    int startIndex = m_CurrentPage * PROPS_PER_PAGE;
    int endIndex = min(startIndex + PROPS_PER_PAGE, static_cast<int>(m_PropFileNames.size()));

    // 선택된 프롭이 현재 페이지에 있는지 확인
    if (m_SelectedPropIndex < startIndex || m_SelectedPropIndex >= endIndex)
        return;

    int relativeIndex = m_SelectedPropIndex - startIndex;
    int row = relativeIndex / PROPS_PER_ROW;
    int col = relativeIndex % PROPS_PER_ROW;

    int propX = m_PosX + 5 + col * (DEFAULT_PROP_SIZE + 2);
    int propY = m_PosY + 25 + row * (DEFAULT_PROP_SIZE + 2);

    // 선택 테두리 그리기
    HPEN highlightPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
    HPEN oldPen = (HPEN)SelectObject(hdc, highlightPen);

    HBRUSH nullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, nullBrush);

    Rectangle(hdc, propX - 1, propY - 1,
        propX + DEFAULT_PROP_SIZE + 1, propY + DEFAULT_PROP_SIZE + 1);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(highlightPen);
}

void PropPalette::DrawPageInfo(HDC hdc)
{
    int totalPages = (static_cast<int>(m_PropFileNames.size()) + PROPS_PER_PAGE - 1) / PROPS_PER_PAGE;

    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);

    // 선택된 프롭 정보
    if (m_SelectedPropIndex >= 0 && m_SelectedPropIndex < static_cast<int>(m_PropNames.size()))
    {
        char selectedText[64];
        sprintf_s(selectedText, "Selected: %s", m_PropNames[m_SelectedPropIndex].c_str());
        TextOutA(hdc, m_PosX + 5, m_PosY + m_Height - 20, selectedText, strlen(selectedText));
    }
}

void PropPalette::SelectProp(int index)
{
    if (index < 0 || index >= static_cast<int>(m_PropFileNames.size()))
        return;

    m_SelectedPropIndex = index;

    // 선택된 프롭이 현재 페이지에 있지 않으면 페이지 변경
    int targetPage = index / PROPS_PER_PAGE;
    if (targetPage != m_CurrentPage)
    {
        m_CurrentPage = targetPage;
    }

    printf("Selected prop %d: %s\n", index, m_PropNames[index].c_str());
}

void PropPalette::PlaceProp(int gridX, int gridY)
{
    if (m_SelectedPropIndex < 0 || m_SelectedPropIndex >= static_cast<int>(m_PropNames.size()))
        return;

    //PropData prop(gridX, gridY, m_PropNames[m_SelectedPropIndex], "");
    //FileManager::GetInstance()->AddProp(prop);

    printf("Placed prop '%s' at (%d, %d)\n",
        m_PropNames[m_SelectedPropIndex].c_str(), gridX, gridY);
}

void PropPalette::Removeprop(int gridX, int gridY)
{
    //FileManager::GetInstance()->Removeprop(gridX, gridY);
    printf("Removed prop at (%d, %d)\n", gridX, gridY);
}

string PropPalette::GetSelectedPropName() const
{
    if (m_SelectedPropIndex >= 0 && m_SelectedPropIndex < static_cast<int>(m_PropNames.size()))
    {
        return m_PropNames[m_SelectedPropIndex];
    }
    return "";
}

int PropPalette::GetPropIndexFromPosition(int x, int y)
{
    int relativeX = x - (m_PosX + 5);
    int relativeY = y - (m_PosY + 25);

    if (relativeX < 0 || relativeY < 0)
        return -1;

    int col = relativeX / (DEFAULT_PROP_SIZE + 2);
    int row = relativeY / (DEFAULT_PROP_SIZE + 2);

    if (col >= PROPS_PER_ROW)
        return -1;

    int relativeIndex = row * PROPS_PER_ROW + col;
    int absoluteIndex = m_CurrentPage * PROPS_PER_PAGE + relativeIndex;

    if (absoluteIndex >= static_cast<int>(m_PropFileNames.size()))
        return -1;

    return absoluteIndex;
}

RECT PropPalette::GetPropRect(int index)
{
    int startIndex = m_CurrentPage * PROPS_PER_PAGE;
    int relativeIndex = index - startIndex;

    int row = relativeIndex / PROPS_PER_ROW;
    int col = relativeIndex % PROPS_PER_ROW;

    RECT rect;
    rect.left = m_PosX + 5 + col * (DEFAULT_PROP_SIZE + 2);
    rect.top = m_PosY + 25 + row * (DEFAULT_PROP_SIZE + 2);
    rect.right = rect.left + DEFAULT_PROP_SIZE;
    rect.bottom = rect.top + DEFAULT_PROP_SIZE;

    return rect;
}

void PropPalette::NextPage()
{
    int totalPages = (static_cast<int>(m_PropFileNames.size()) + PROPS_PER_PAGE - 1) / PROPS_PER_PAGE;
    if (m_CurrentPage < totalPages - 1)
    {
        m_CurrentPage++;
        printf("Prop palette page: %d\n", m_CurrentPage + 1);
    }
}

void PropPalette::PrevPage()
{
    if (m_CurrentPage > 0)
    {
        m_CurrentPage--;
        printf("Prop palette page: %d\n", m_CurrentPage + 1);
    }
}

void PropPalette::SetPosition(int x, int y)
{
    m_PosX = x;
    m_PosY = y;
}

void PropPalette::SetSize(int width, int height)
{
    m_Width = width;
    m_Height = height;
}

string PropPalette::ConvertPropFileName(const wstring& fileName)
{
    // 파일 확장자 제거
    wstring nameOnly = fileName;
    size_t dotPos = nameOnly.find_last_of(L'.');
    if (dotPos != wstring::npos)
    {
        nameOnly = nameOnly.substr(0, dotPos);
    }

    // 간단한 변환 (실제로는 더 정교한 매핑 테이블이 필요)
    // 예시 파일명들을 게임에서 사용하는 프롭 이름으로 변환
    map<wstring, string> propNameMap = {
        {L"상자", "Box01"},
        {L"상자1", "Box01"},
        {L"상자2", "Box00"},
        {L"횃불", "Torch"},
        {L"돌", "Stone_0"},
        {L"돌1", "Stone_1"},
        {L"돌2", "Stone_2"},
        {L"돌3", "Stone_3"},
        {L"버섯", "Mushroom0"},
        {L"버섯1", "Mushroom1"},
        {L"버섯2", "Mushroom2"},
        {L"화로", "Brazier"},
        {L"배럴", "Barrel00"},
        {L"자루", "Pouch"},
        {L"나무상자", "PackagedWood"},
        {L"뼈", "CaveBone_0"},
        {L"뼈1", "CaveBone_1"},
        {L"바리케이트", "Barricade_V"},
        {L"철도차", "RailCar_Static"},
        {L"레일", "Rail_Horizontal"},
        {L"벽", "Wall_Wooden_NormalMiddle"},
        {L"울타리", "Wall_Wooden_Fence"}
    };

    auto it = propNameMap.find(nameOnly);
    if (it != propNameMap.end())
    {
        return it->second;
    }

    // 매핑되지 않은 경우 기본 변환 (wstring을 string으로)
    string result;
    for (wchar_t wc : nameOnly)
    {
        if (wc < 128) // ASCII 범위
        {
            result += static_cast<char>(wc);
        }
        else
        {
            result += "Prop"; // 기본 이름
            break;
        }
    }

    if (result.empty())
    {
        result = "DefaultProp";
    }

    return result;
}