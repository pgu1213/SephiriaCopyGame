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
	// TODO : �̹��� �޸� ����
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

    // �̹��� �ε�
    Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(filePath.c_str());

    // �������ͽ��� Ok�� �ƴϰų� ��Ʈ���� nullptr�� ��� = �ε� ����
    if (bitmap == nullptr || bitmap->GetLastStatus() != Gdiplus::Ok)
    {
        delete bitmap;
        return nullptr;
    }

    // ��Ʈ�� ��ȯ
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
