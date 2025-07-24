#pragma once

class MainEditor
{
public:
    MainEditor();
    ~MainEditor();

public:
    void Initialize();
    void Update();
    void Render();
    void Release();

public:
    void HandleGlobalInput();
    void RenderUI(HDC hdc);
    void SaveCurrentMap();
    void LoadMap();
    void CreateNewMap();
    void ChangeRoomType();
    void ChangeRoomSize();
    void TogglePropMode();
    void ShowMapFileList();
    void CleanupOldFiles();
    void ValidateCurrentMap();
    void PrintControls();

private:
    // 더블 버퍼링용 멤버
    HDC m_hDC;
    HDC m_memDC;
    HBITMAP m_memBitmap;
    HBITMAP m_oldBitmap;

    class TileMapEditor* m_pTileMapEditor;
    class TilePalette* m_pTilePalette;
    class PropPalette* m_pPropPalette;
    class Camera* m_pCamera;

    bool m_IsInitialized;
    bool m_PropMode;

    int m_ScreenWidth;
    int m_ScreenHeight;
};
