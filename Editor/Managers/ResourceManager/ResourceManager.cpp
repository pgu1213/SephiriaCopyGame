#include "../../pch.h"
#include "ResourceManager.h"
#include <filesystem>

// 정적 멤버 초기화
ULONG_PTR ResourceManager::GdiplusToken = 0;
map<wstring, Gdiplus::Bitmap*> ResourceManager::TileMap;
map<wstring, Gdiplus::Bitmap*> ResourceManager::PropMap;
map<int, TileResource> ResourceManager::TileResourcesById;
map<wstring, int> ResourceManager::TileFileNameToId;
int ResourceManager::NextTileID = 1; // 0은 빈 타일용으로 예약

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
    Release();
}

bool ResourceManager::Init()
{
    // GDI+ 초기화
    GdiplusStartupInput gdiplusStartupInput;
    if (GdiplusStartup(&GdiplusToken, &gdiplusStartupInput, NULL) != Ok)
    {
        return false;
    }
    
    LoadTileResources();
    LoadPropResources();
    AssignTileIDs(); // 타일들에 ID 할당
    
    return true;
}

void ResourceManager::Release()
{
    // 타일 리소스 해제
    for (auto& pair : TileMap)
    {
        if (pair.second)
        {
            delete pair.second;
        }
    }
    TileMap.clear();
    
    // Prop 리소스 해제
    for (auto& pair : PropMap)
    {
        if (pair.second)
        {
            delete pair.second;
        }
    }
    PropMap.clear();
    
    // 타일 ID 관련 데이터 정리
    TileResourcesById.clear();
    TileFileNameToId.clear();
    NextTileID = 1;
    
    // GDI+ 종료
    if (GdiplusToken != 0)
    {
        GdiplusShutdown(GdiplusToken);
        GdiplusToken = 0;
    }
}

void ResourceManager::LoadTileResources()
{
    wstring tilePath = L"../Resources/Tiles/";
    
    try
    {
        for (const auto& entry : filesystem::directory_iterator(tilePath))
        {
            if (entry.is_regular_file())
            {
                wstring fileName = entry.path().filename().wstring();
                wstring extension = entry.path().extension().wstring();
                
                // 이미지 파일만 로드
                if (extension == L".png" || extension == L".jpg" || extension == L".bmp")
                {
                    wstring fullPath = entry.path().wstring();
                    Bitmap* bitmap = new Bitmap(fullPath.c_str());
                    
                    if (bitmap && bitmap->GetLastStatus() == Ok)
                    {
                        TileMap[fileName] = bitmap;
                    }
                    else
                    {
                        delete bitmap;
                    }
                }
            }
        }
    }
    catch (const filesystem::filesystem_error& e)
    {
        // 디렉토리가 없거나 접근할 수 없는 경우
        wcout << L"Tile directory not found or inaccessible: " << tilePath << endl;
    }
}

void ResourceManager::LoadPropResources()
{
    wstring propPath = L"../Resources/Props/";
    
    try
    {
        for (const auto& entry : filesystem::directory_iterator(propPath))
        {
            if (entry.is_regular_file())
            {
                wstring fileName = entry.path().filename().wstring();
                wstring extension = entry.path().extension().wstring();
                
                // 이미지 파일만 로드
                if (extension == L".png" || extension == L".jpg" || extension == L".bmp")
                {
                    wstring fullPath = entry.path().wstring();
                    Bitmap* bitmap = new Bitmap(fullPath.c_str());
                    
                    if (bitmap && bitmap->GetLastStatus() == Ok)
                    {
                        PropMap[fileName] = bitmap;
                    }
                    else
                    {
                        delete bitmap;
                    }
                }
            }
        }
    }
    catch (const filesystem::filesystem_error& e)
    {
        // 디렉토리가 없거나 접근할 수 없는 경우
        wcout << L"Prop directory not found or inaccessible: " << propPath << endl;
    }
}

void ResourceManager::AssignTileIDs()
{
    // 기존 타일들에 순차적으로 ID 할당
    for (const auto& pair : TileMap)
    {
        const wstring& fileName = pair.first;
        Bitmap* bitmap = pair.second;
        
        TileResource resource;
        resource.id = NextTileID;
        resource.fileName = fileName;
        resource.bitmap = bitmap;
        
        TileResourcesById[NextTileID] = resource;
        TileFileNameToId[fileName] = NextTileID;
        
        NextTileID++;
    }
}

Gdiplus::Bitmap* ResourceManager::GetTileBitmap(const wstring& fileName)
{
    auto it = TileMap.find(fileName);
    return (it != TileMap.end()) ? it->second : nullptr;
}

Gdiplus::Bitmap* ResourceManager::GetPropBitmap(const wstring& fileName)
{
    auto it = PropMap.find(fileName);
    return (it != PropMap.end()) ? it->second : nullptr;
}

Gdiplus::Bitmap* ResourceManager::GetTileBitmapByID(int tileID)
{
    auto it = TileResourcesById.find(tileID);
    return (it != TileResourcesById.end()) ? it->second.bitmap : nullptr;
}

int ResourceManager::GetTileIDByFileName(const wstring& fileName)
{
    auto it = TileFileNameToId.find(fileName);
    return (it != TileFileNameToId.end()) ? it->second : 0;
}

wstring ResourceManager::GetFileNameByTileID(int tileID)
{
    auto it = TileResourcesById.find(tileID);
    return (it != TileResourcesById.end()) ? it->second.fileName : L"";
}

vector<TileResource> ResourceManager::GetAllTileResources() const
{
    vector<TileResource> resources;
    for (const auto& pair : TileResourcesById)
    {
        resources.push_back(pair.second);
    }
    return resources;
}

vector<wstring> ResourceManager::GetTileFileNames() const
{
    vector<wstring> fileNames;
    for (const auto& pair : TileMap)
    {
        fileNames.push_back(pair.first);
    }
    return fileNames;
}

vector<wstring> ResourceManager::GetPropFileNames() const
{
    vector<wstring> fileNames;
    for (const auto& pair : PropMap)
    {
        fileNames.push_back(pair.first);
    }
    return fileNames;
}

bool ResourceManager::RegisterTileResource(int id, const wstring& fileName, Gdiplus::Bitmap* bitmap)
{
    // 이미 존재하는 ID인지 확인
    if (TileResourcesById.find(id) != TileResourcesById.end())
    {
        return false;
    }
    
    TileResource resource;
    resource.id = id;
    resource.fileName = fileName;
    resource.bitmap = bitmap;
    
    TileResourcesById[id] = resource;
    TileFileNameToId[fileName] = id;
    TileMap[fileName] = bitmap;
    
    // NextTileID 업데이트
    if (id >= NextTileID)
    {
        NextTileID = id + 1;
    }
    
    return true;
}

void ResourceManager::DrawBitmap(HDC hdc, Gdiplus::Bitmap* bitmap, int x, int y, int width, int height)
{
    if (!bitmap) return;
    
    Graphics graphics(hdc);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor); // 픽셀 아트에 적합
    graphics.DrawImage(bitmap, x, y, width, height);
}

void ResourceManager::DrawBitmapSection(HDC hdc, Gdiplus::Bitmap* bitmap, int destX, int destY, int destWidth, int destHeight, int srcX, int srcY, int srcWidth, int srcHeight)
{
    if (!bitmap) return;
    
    Graphics graphics(hdc);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    
    Rect destRect(destX, destY, destWidth, destHeight);
    graphics.DrawImage(bitmap, destRect, srcX, srcY, srcWidth, srcHeight, UnitPixel);
}