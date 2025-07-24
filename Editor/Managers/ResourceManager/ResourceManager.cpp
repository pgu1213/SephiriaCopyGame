#include "../../pch.h"
#include "ResourceManager.h"
#include <filesystem>

ULONG_PTR ResourceManager::GdiplusToken = 0;
map<wstring, Gdiplus::Bitmap*> ResourceManager::ImageMap;
set<wstring> ResourceManager::TileNames;
set<wstring> ResourceManager::PropNames;

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
    Release();
}

bool ResourceManager::Init()
{
    GdiplusStartupInput gdiplusStartupInput;
    if (GdiplusStartup(&GdiplusToken, &gdiplusStartupInput, NULL) != Ok)
    {
        return false;
    }
    return true;
}

void ResourceManager::Release()
{
    for (auto& pair : ImageMap)
    {
        if (pair.second)
        {
            delete pair.second;
        }
    }
    ImageMap.clear();
    TileNames.clear();
    PropNames.clear();

    if (GdiplusToken)
    {
        GdiplusShutdown(GdiplusToken);
        GdiplusToken = 0;
    }
}

void ResourceManager::LoadAllResources()
{
    // ���丮�� ������ ����
    filesystem::create_directories(L"Resources/Tiles");
    filesystem::create_directories(L"Resources/Props");

    LoadResourcesFromDirectory(L"Resources/Tiles");
    LoadResourcesFromDirectory(L"Resources/Props");

    cout << "�ε�� Ÿ��: " << TileNames.size() << "��" << endl;
    cout << "�ε�� ����: " << PropNames.size() << "��" << endl;
}

void ResourceManager::LoadResourcesFromDirectory(const wstring& directory)
{
    try
    {
        for (const auto& entry : filesystem::directory_iterator(directory))
        {
            if (entry.is_regular_file())
            {
                wstring extension = entry.path().extension().wstring();
                transform(extension.begin(), extension.end(), extension.begin(), towlower);

                if (extension == L".png" || extension == L".jpg" || extension == L".bmp")
                {
                    wstring fullPath = entry.path().wstring();
                    wstring fileName = GetFileNameWithoutExtension(fullPath);

                    Bitmap* bitmap = new Bitmap(fullPath.c_str());
                    if (bitmap && bitmap->GetLastStatus() == Ok)
                    {
                        ImageMap[fileName] = bitmap;
                        ClassifyResource(fileName);
                        wcout << L"�ε��: " << fileName << endl;
                    }
                    else
                    {
                        delete bitmap;
                        wcout << L"�ε� ����: " << fullPath << endl;
                    }
                }
            }
        }
    }
    catch (const filesystem::filesystem_error& ex)
    {
        cout << "���丮 ���� ����: " << ex.what() << endl;
    }
}

wstring ResourceManager::GetFileNameWithoutExtension(const wstring& filePath)
{
    filesystem::path path(filePath);
    return path.stem().wstring();
}

void ResourceManager::ClassifyResource(const wstring& fileName)
{
    // ������ �з� ���: ���ϸ� Ư�� Ű���尡 ������ �з�
    wstring lowerFileName = fileName;
    transform(lowerFileName.begin(), lowerFileName.end(), lowerFileName.begin(), towlower);

    // ���� Ű�����
    vector<wstring> propKeywords = { L"prop", L"object", L"item", L"furniture", L"decoration" };

    bool isProp = false;
    for (const auto& keyword : propKeywords)
    {
        if (lowerFileName.find(keyword) != wstring::npos)
        {
            isProp = true;
            break;
        }
    }

    if (isProp)
    {
        PropNames.insert(fileName);
    }
    else
    {
        TileNames.insert(fileName);
    }
}

Bitmap* ResourceManager::GetSprite(const wstring& spriteName)
{
    auto it = ImageMap.find(spriteName);
    if (it != ImageMap.end())
    {
        return it->second;
    }
    return nullptr;
}

bool ResourceManager::IsTile(const wstring& spriteName)
{
    return TileNames.find(spriteName) != TileNames.end();
}

bool ResourceManager::IsProp(const wstring& spriteName)
{
    return PropNames.find(spriteName) != PropNames.end();
}

vector<wstring> ResourceManager::GetTileNames()
{
    return vector<wstring>(TileNames.begin(), TileNames.end());
}

vector<wstring> ResourceManager::GetPropNames()
{
    return vector<wstring>(PropNames.begin(), PropNames.end());
}