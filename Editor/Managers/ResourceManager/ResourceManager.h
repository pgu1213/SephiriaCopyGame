#pragma once
#include "../SingletonManager/SingletonManager.h"

enum class ResourceType
{
    TILE,
    PROP
};

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

    // 타일/프롭 구분 기능
    bool IsTile(const wstring& spriteName);
    bool IsProp(const wstring& spriteName);
    vector<wstring> GetTileNames();
    vector<wstring> GetPropNames();

private:
    void LoadResourcesFromDirectory(const wstring& directory);
    wstring GetFileNameWithoutExtension(const wstring& filePath);
    void ClassifyResource(const wstring& fileName);

private:
    static ULONG_PTR GdiplusToken;
    static map<wstring, Gdiplus::Bitmap*> ImageMap;
    static set<wstring> TileNames;
    static set<wstring> PropNames;
};