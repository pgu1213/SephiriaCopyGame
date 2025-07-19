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
private:
	HDC m_hDC;
	class TileMapEditor* m_pTileMapEditor;
};


