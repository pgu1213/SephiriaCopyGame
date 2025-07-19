#include "../../pch.h"
#include "ResourceManager.h"
#include <filesystem>

// 정적 멤버 초기화
ULONG_PTR ResourceManager::GdiplusToken = 0;
map<wstring, Gdiplus::Bitmap*> ResourceManager::TileMap;
map<wstring, Gdiplus::Bitmap*> ResourceManager::PropMap;

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
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::Status status = Gdiplus::GdiplusStartup(&GdiplusToken, &gdiplusStartupInput, NULL);

    if (status != Gdiplus::Ok)
    {
        printf("Failed to initialize GDI+. Status: %d\n", status);
        return false;
    }

    printf("GDI+ initialized successfully\n");

    // 리소스 폴더에서 PNG 파일들 로드
    LoadTileResources();
    LoadPropResources();

    return true;
}

void ResourceManager::Release()
{
    // 타일 이미지 해제
    for (auto& pair : TileMap)
    {
        if (pair.second)
        {
            delete pair.second;
        }
    }
    TileMap.clear();

    // 프롭 이미지 해제
    for (auto& pair : PropMap)
    {
        if (pair.second)
        {
            delete pair.second;
        }
    }
    PropMap.clear();

    // GDI+ 해제
    if (GdiplusToken != 0)
    {
        Gdiplus::GdiplusShutdown(GdiplusToken);
        GdiplusToken = 0;
    }

    printf("ResourceManager released\n");
}

void ResourceManager::LoadTileResources()
{
    wstring tileFolder = L"Resource/Tile/";

    try
    {
        if (filesystem::exists(tileFolder))
        {
            for (const auto& entry : filesystem::directory_iterator(tileFolder))
            {
                if (entry.is_regular_file())
                {
                    wstring extension = entry.path().extension().wstring();
                    transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

                    if (extension == L".png")
                    {
                        wstring filePath = entry.path().wstring();
                        wstring fileName = entry.path().filename().wstring();

                        Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(filePath.c_str());

                        if (bitmap && bitmap->GetLastStatus() == Gdiplus::Ok)
                        {
                            TileMap[fileName] = bitmap;
                            printf("Loaded tile: %ws (%dx%d)\n",
                                fileName.c_str(),
                                bitmap->GetWidth(),
                                bitmap->GetHeight());
                        }
                        else
                        {
                            printf("Failed to load tile: %ws\n", fileName.c_str());
                            if (bitmap) delete bitmap;
                        }
                    }
                }
            }

            printf("Total tiles loaded: %zu\n", TileMap.size());
        }
        else
        {
            printf("Tile folder not found: %ws\n", tileFolder.c_str());
        }
    }
    catch (const exception& e)
    {
        printf("Error loading tile resources: %s\n", e.what());
    }
}

void ResourceManager::LoadPropResources()
{
    wstring propFolder = L"Resource/Prop/";

    try
    {
        if (filesystem::exists(propFolder))
        {
            for (const auto& entry : filesystem::directory_iterator(propFolder))
            {
                if (entry.is_regular_file())
                {
                    wstring extension = entry.path().extension().wstring();
                    transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

                    if (extension == L".png")
                    {
                        wstring filePath = entry.path().wstring();
                        wstring fileName = entry.path().filename().wstring();

                        Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(filePath.c_str());

                        if (bitmap && bitmap->GetLastStatus() == Gdiplus::Ok)
                        {
                            PropMap[fileName] = bitmap;
                            printf("Loaded prop: %ws (%dx%d)\n",
                                fileName.c_str(),
                                bitmap->GetWidth(),
                                bitmap->GetHeight());
                        }
                        else
                        {
                            printf("Failed to load prop: %ws\n", fileName.c_str());
                            if (bitmap) delete bitmap;
                        }
                    }
                }
            }

            printf("Total props loaded: %zu\n", PropMap.size());
        }
        else
        {
            printf("Prop folder not found: %ws\n", propFolder.c_str());
        }
    }
    catch (const exception& e)
    {
        printf("Error loading prop resources: %s\n", e.what());
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

void ResourceManager::DrawBitmap(HDC hdc, Gdiplus::Bitmap* bitmap, int x, int y, int width, int height)
{
    if (!bitmap) return;

    Gdiplus::Graphics graphics(hdc);
    graphics.DrawImage(bitmap, x, y, width, height);
}

void ResourceManager::DrawBitmapSection(HDC hdc, Gdiplus::Bitmap* bitmap,
    int destX, int destY, int destWidth, int destHeight,
    int srcX, int srcY, int srcWidth, int srcHeight)
{
    if (!bitmap) return;

    Gdiplus::Graphics graphics(hdc);
    Gdiplus::RectF destRect(static_cast<float>(destX), static_cast<float>(destY),
        static_cast<float>(destWidth), static_cast<float>(destHeight));

    graphics.DrawImage(bitmap, destRect,
        static_cast<float>(srcX), static_cast<float>(srcY),
        static_cast<float>(srcWidth), static_cast<float>(srcHeight),
        Gdiplus::UnitPixel);
}