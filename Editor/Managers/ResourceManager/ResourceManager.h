#pragma once
#include "../SingletonManager/SingletonManager.h"

class ResourceManager : public SingleTon<ResourceManager>
{
	friend class SingleTon<ResourceManager>;
private:
	explicit ResourceManager();
public:
	virtual ~ResourceManager();
public:
	bool Init();
	void Release();
	void LoadTileResources();
	void LoadPropResources();
	Gdiplus::Bitmap* GetTileBitmap(const wstring& fileName);
	Gdiplus::Bitmap* GetPropBitmap(const wstring& fileName);
	vector<wstring> GetTileFileNames() const;
	vector<wstring> GetPropFileNames() const;
	void DrawBitmap(HDC hdc, Gdiplus::Bitmap* bitmap, int x, int y, int width, int height);
	void DrawBitmapSection(HDC hdc, Gdiplus::Bitmap* bitmap, int destX, int destY, int destWidth, int destHeight, int srcX, int srcY, int srcWidth, int srcHeight);
private:
	static ULONG_PTR GdiplusToken;
	static map<wstring, Gdiplus::Bitmap*> TileMap;
	static map<wstring, Gdiplus::Bitmap*> PropMap;
};