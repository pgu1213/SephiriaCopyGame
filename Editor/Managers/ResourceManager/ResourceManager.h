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

    // ���� �޼���
    Gdiplus::Bitmap* GetTileBitmap(const wstring& fileName);
    Gdiplus::Bitmap* GetPropBitmap(const wstring& fileName);
    vector<wstring> GetTileFileNames() const;
    vector<wstring> GetPropFileNames() const;

    // ���ο� Ÿ�� ID ���� �޼���
    Gdiplus::Bitmap* GetTileBitmapByID(int tileID);
    int GetTileIDByFileName(const wstring& fileName);
    wstring GetFileNameByTileID(int tileID);
    vector<TileResource> GetAllTileResources() const;
    bool RegisterTileResource(int id, const wstring& fileName, Gdiplus::Bitmap* bitmap);

    // ������ �޼���
    void DrawBitmap(HDC hdc, Gdiplus::Bitmap* bitmap, int x, int y, int width, int height);
    void DrawBitmapSection(HDC hdc, Gdiplus::Bitmap* bitmap, int destX, int destY, int destWidth, int destHeight, int srcX, int srcY, int srcWidth, int srcHeight);

private:
    void AssignTileIDs(); // Ÿ�� ���ϵ鿡 ID�� �ڵ� �Ҵ�

private:
    static ULONG_PTR GdiplusToken;
    static map<wstring, Gdiplus::Bitmap*> TileMap;
    static map<wstring, Gdiplus::Bitmap*> PropMap;

    // Ÿ�� ID ���� ���ο� ���
    static map<int, TileResource> TileResourcesById;    // ID -> Ÿ�� ���ҽ�
    static map<wstring, int> TileFileNameToId;          // ���ϸ� -> ID
    static int NextTileID;                              // ������ �Ҵ��� Ÿ�� ID
};