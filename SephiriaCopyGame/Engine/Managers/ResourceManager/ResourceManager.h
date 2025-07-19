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
	void LoadAllResources();
	Bitmap* GetSprite(const wstring& spriteName);
private:
	void LoadResourcesFromDirectory(const wstring& directory);
	wstring GetFileNameWithoutExtension(const wstring& filePath);

private:
	static ULONG_PTR GdiplusToken;
	static map<wstring, Gdiplus::Bitmap*> ImageMap;
};