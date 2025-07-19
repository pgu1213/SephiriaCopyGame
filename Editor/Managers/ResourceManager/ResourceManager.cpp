#include "ResourceManager.h"

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