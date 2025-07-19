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

private:
	static ULONG_PTR GdiplusToken;
	static map<wstring, Gdiplus::Bitmap*> TileMap;
	static map<wstring, Gdiplus::Bitmap*> PropMap;
};