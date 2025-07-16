#include <pch.h>
#include "ResourceManager.h"

ULONG_PTR ResourceManager::GdiplusToken = 0;
map<wstring, Gdiplus::Bitmap*> ResourceManager::ImageMap;

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

bool ResourceManager::Init()
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::Status gdiplusStartupStatus = GdiplusStartup(&GdiplusToken, &gdiplusStartupInput, NULL);

	printf("GDI+ Startup Status: %d\n", gdiplusStartupStatus);

    return gdiplusStartupStatus == Gdiplus::Ok;
}

void ResourceManager::Release()
{
	// TODO : 이미지 메모리 해제
    /*for (auto const& [key, val] : ImageMap)
    {
        delete val;
    }
    ImageMap.clear();

    Gdiplus::GdiplusShutdown(GdiplusToken);*/
}

Bitmap* ResourceManager::LoadSprite(const wstring& filePath)
{
    if (ImageMap.count(filePath))
    {
        return ImageMap[filePath];
    }

    // 이미지 로드
    Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(filePath.c_str());

    // 스테이터스가 Ok가 아니거나 비트맵이 nullptr인 경우 = 로드 실패
    if (bitmap == nullptr || bitmap->GetLastStatus() != Gdiplus::Ok)
    {
        delete bitmap;
        return nullptr;
    }

    // 비트맵 반환
    ImageMap[filePath] = bitmap;
    return bitmap;
}

Bitmap* ResourceManager::GetSprite(const wstring& filePath)
{
    if (ImageMap.count(filePath))
    {
        return ImageMap[filePath];
    }

    return nullptr;
}
