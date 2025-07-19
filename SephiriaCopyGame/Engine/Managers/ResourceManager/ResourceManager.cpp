#include <pch.h>
#include "ResourceManager.h"
#include <filesystem>

ULONG_PTR ResourceManager::GdiplusToken = 0;
map<wstring, Gdiplus::Bitmap*> ResourceManager::ImageMap;

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	Release();
}

bool ResourceManager::Init()
{
	// GDI+ �ʱ�ȭ
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	if (Gdiplus::GdiplusStartup(&GdiplusToken, &gdiplusStartupInput, nullptr) != Gdiplus::Ok)
	{
		return false;
	}

	// ��� ���ҽ� �ε�
	LoadAllResources();

	return true;
}

void ResourceManager::Release()
{
	// ��� ��Ʈ�� ����
	for (auto& pair : ImageMap)
	{
		SafeDelete(pair.second);
	}
	ImageMap.clear();

	// GDI+ ����
	if (GdiplusToken != 0)
	{
		Gdiplus::GdiplusShutdown(GdiplusToken);
		GdiplusToken = 0;
	}
}

void ResourceManager::LoadAllResources()
{
	// Resource ���� ��� (���� ���� ����)
	wstring resourcePath = L"./Resource";

	// Resource ������ �����ϴ��� Ȯ��
	if (std::filesystem::exists(resourcePath) && std::filesystem::is_directory(resourcePath))
	{
		LoadResourcesFromDirectory(resourcePath);
	}
	else
	{
		// ���� �������� Resource ���� ã�� (���� ������ ���� ��ΰ� �ٸ� �� ����)
		resourcePath = L"../Resource";
		if (std::filesystem::exists(resourcePath) && std::filesystem::is_directory(resourcePath))
		{
			LoadResourcesFromDirectory(resourcePath);
		}
		else
		{
			resourcePath = L"../../Resource";
			if (std::filesystem::exists(resourcePath) && std::filesystem::is_directory(resourcePath))
			{
				LoadResourcesFromDirectory(resourcePath);
			}
		}
	}
}

void ResourceManager::LoadResourcesFromDirectory(const wstring& directory)
{
	try
	{
		// ���丮�� ��������� Ž��
		for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
		{
			if (entry.is_regular_file())
			{
				wstring filePath = entry.path().wstring();
				wstring extension = entry.path().extension().wstring();

				// PNG ���ϸ� ó��
				if (extension == L".png" || extension == L".PNG")
				{
					// Ȯ���ڸ� ������ ���ϸ� ����
					wstring fileName = GetFileNameWithoutExtension(entry.path().filename().wstring());

					// �̹� �ε�� �������� Ȯ�� (�ߺ� ����)
					if (ImageMap.find(fileName) == ImageMap.end())
					{
						// ��Ʈ�� �ε�
						Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(filePath.c_str());

						if (bitmap && bitmap->GetLastStatus() == Gdiplus::Ok)
						{
							ImageMap[fileName] = bitmap;

							// �ε� ���� �α� (����׿�)
							wcout << L"Loaded: " << fileName << L" from " << filePath << endl;
						}
						else
						{
							// �ε� ���н� �޸� ����
							SafeDelete(bitmap);
							wcout << L"Failed to load: " << filePath << endl;
						}
					}
				}
			}
		}
	}
	catch (const std::filesystem::filesystem_error& ex)
	{
		// ���Ͻý��� ���� ó��
		cout << "Filesystem error: " << ex.what() << endl;
	}
}

wstring ResourceManager::GetFileNameWithoutExtension(const wstring& filePath)
{
	size_t lastDot = filePath.find_last_of(L'.');
	if (lastDot != wstring::npos)
	{
		return filePath.substr(0, lastDot);
	}
	return filePath;
}

Bitmap* ResourceManager::GetSprite(const wstring& spriteName)
{
	auto iter = ImageMap.find(spriteName);
	if (iter != ImageMap.end())
	{
		return iter->second;
	}

	// ��������Ʈ�� ã�� ���� ���
	wcout << L"Sprite not found: " << spriteName << endl;
	return nullptr;
}