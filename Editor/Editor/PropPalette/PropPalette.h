#pragma once
#include "../../Managers/ResourceManager/ResourceManager.h"
#include "../../Managers/FileManager/FileManager.h"

class PropPalette
{
public:
    PropPalette();
    ~PropPalette();

public:
    void Init();
    void Update();
    void Render(HDC hdc);

    void SetPosition(int x, int y);
    void SetSize(int width, int height);

    // 프롭 선택
    void SelectProp(int index);
    int GetSelectedPropIndex() const { return m_SelectedPropIndex; }
    string GetSelectedPropName() const;

    // 프롭 배치/제거
    void PlaceProp(int gridX, int gridY);
    void Removeprop(int gridX, int gridY);

    // 페이지 기능
    void NextPage();
    void PrevPage();

private:
    static const int DEFAULT_PROP_SIZE = 32;
    static const int PROPS_PER_ROW = 6;
    static const int PROPS_PER_PAGE = 24;

    // 팔레트 위치 및 크기
    int m_PosX;
    int m_PosY;
    int m_Width;
    int m_Height;

    // 프롭 선택 관련
    int m_SelectedPropIndex;
    int m_CurrentPage;

    // 프롭 파일명 목록
    vector<wstring> m_PropFileNames;
    vector<string> m_PropNames; // 영어 이름으로 변환된 목록

    void HandleInput();
    void DrawPropGrid(HDC hdc);
    void DrawSelectedHighlight(HDC hdc);
    void DrawPageInfo(HDC hdc);

    // 좌표 변환
    int GetPropIndexFromPosition(int x, int y);
    RECT GetPropRect(int index);

    // 프롭 이름 변환 (한글 파일명을 영어로)
    string ConvertPropFileName(const wstring& fileName);
};