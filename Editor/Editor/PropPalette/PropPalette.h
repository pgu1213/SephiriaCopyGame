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

    // ���� ����
    void SelectProp(int index);
    int GetSelectedPropIndex() const { return m_SelectedPropIndex; }
    string GetSelectedPropName() const;

    // ���� ��ġ/����
    void PlaceProp(int gridX, int gridY);
    void Removeprop(int gridX, int gridY);

    // ������ ���
    void NextPage();
    void PrevPage();

private:
    static const int DEFAULT_PROP_SIZE = 32;
    static const int PROPS_PER_ROW = 6;
    static const int PROPS_PER_PAGE = 24;

    // �ȷ�Ʈ ��ġ �� ũ��
    int m_PosX;
    int m_PosY;
    int m_Width;
    int m_Height;

    // ���� ���� ����
    int m_SelectedPropIndex;
    int m_CurrentPage;

    // ���� ���ϸ� ���
    vector<wstring> m_PropFileNames;
    vector<string> m_PropNames; // ���� �̸����� ��ȯ�� ���

    void HandleInput();
    void DrawPropGrid(HDC hdc);
    void DrawSelectedHighlight(HDC hdc);
    void DrawPageInfo(HDC hdc);

    // ��ǥ ��ȯ
    int GetPropIndexFromPosition(int x, int y);
    RECT GetPropRect(int index);

    // ���� �̸� ��ȯ (�ѱ� ���ϸ��� �����)
    string ConvertPropFileName(const wstring& fileName);
};