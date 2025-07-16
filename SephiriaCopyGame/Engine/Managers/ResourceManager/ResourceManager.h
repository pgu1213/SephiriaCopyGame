#pragma once
#include <Engine/Managers/SingletonManager/SingletonManager.h>

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
	Bitmap* LoadSprite(const wstring& filePath);
	Bitmap* GetSprite(const wstring& filePath);
private:
	static ULONG_PTR GdiplusToken;
	static map<wstring, Gdiplus::Bitmap*> ImageMap;
};