#pragma once
#include "../SingletonManager/SingletonManager.h"

struct TileResource
{
    int id;
    wstring fileName;
    Gdiplus::Bitmap* bitmap;
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
    void LoadTileResources();
    void LoadPropResources();

    // 기존 메서드
    Gdiplus::Bitmap* GetTileBitmap(const wstring& fileName);
    Gdiplus::Bitmap* GetPropBitmap(const wstring& fileName);
    vector<wstring> GetTileFileNames() const;
    vector<wstring> GetPropFileNames() const;

    // 새로운 타일 ID 관련 메서드
    Gdiplus::Bitmap* GetTileBitmapByID(int tileID);
    int GetTileIDByFileName(const wstring& fileName);
    wstring GetFileNameByTileID(int tileID);
    vector<TileResource> GetAllTileResources() const;
    bool RegisterTileResource(int id, const wstring& fileName, Gdiplus::Bitmap* bitmap);

    // 렌더링 메서드
    void DrawBitmap(HDC hdc, Gdiplus::Bitmap* bitmap, int x, int y, int width, int height);
    void DrawBitmapSection(HDC hdc, Gdiplus::Bitmap* bitmap, int destX, int destY, int destWidth, int destHeight, int srcX, int srcY, int srcWidth, int srcHeight);

private:
    void AssignTileIDs(); // 타일 파일들에 ID를 자동 할당

private:
    static ULONG_PTR GdiplusToken;
    static map<wstring, Gdiplus::Bitmap*> TileMap;
    static map<wstring, Gdiplus::Bitmap*> PropMap;

    // 타일 ID 관련 새로운 멤버
    static map<int, TileResource> TileResourcesById;    // ID -> 타일 리소스
    static map<wstring, int> TileFileNameToId;          // 파일명 -> ID
    static int NextTileID;                              // 다음에 할당할 타일 ID
};