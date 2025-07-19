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
	void ChangeGridSize();
	void ChangeRoomType();
	void TogglePropMode();
	void PrintControls();
private:
	HDC m_hDC;
	class TileMapEditor* m_pTileMapEditor;
	class TilePalette* m_pTilePalette;
	class PropPalette* m_pPropPalette;
	class Camera* m_pCamera;
	bool m_IsInitialized;
	bool m_PropMode;
};


