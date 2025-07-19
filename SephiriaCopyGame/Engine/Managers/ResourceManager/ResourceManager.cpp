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
	// GDI+ 초기화
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	if (Gdiplus::GdiplusStartup(&GdiplusToken, &gdiplusStartupInput, nullptr) != Gdiplus::Ok)
	{
		return false;
	}

	// 모든 리소스 로드
	LoadAllResources();

	return true;
}

void ResourceManager::Release()
{
	// 모든 비트맵 해제
	for (auto& pair : ImageMap)
	{
		SafeDelete(pair.second);
	}
	ImageMap.clear();

	// GDI+ 종료
	if (GdiplusToken != 0)
	{
		Gdiplus::GdiplusShutdown(GdiplusToken);
		GdiplusToken = 0;
	}
}

void ResourceManager::LoadAllResources()
{
	// Resource 폴더 경로 (실행 파일 기준)
	wstring resourcePath = L"./Resource";

	// Resource 폴더가 존재하는지 확인
	if (std::filesystem::exists(resourcePath) && std::filesystem::is_directory(resourcePath))
	{
		LoadResourcesFromDirectory(resourcePath);
	}
	else
	{
		// 상위 폴더에서 Resource 폴더 찾기 (빌드 설정에 따라 경로가 다를 수 있음)
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
		// 디렉토리를 재귀적으로 탐색
		for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
		{
			if (entry.is_regular_file())
			{
				wstring filePath = entry.path().wstring();
				wstring extension = entry.path().extension().wstring();

				// PNG 파일만 처리
				if (extension == L".png" || extension == L".PNG")
				{
					// 확장자를 제외한 파일명 추출
					wstring fileName = GetFileNameWithoutExtension(entry.path().filename().wstring());

					// 이미 로드된 파일인지 확인 (중복 방지)
					if (ImageMap.find(fileName) == ImageMap.end())
					{
						// 비트맵 로드
						Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(filePath.c_str());

						if (bitmap && bitmap->GetLastStatus() == Gdiplus::Ok)
						{
							ImageMap[fileName] = bitmap;

							// 로드 성공 로그 (디버그용)
							wcout << L"Loaded: " << fileName << L" from " << filePath << endl;
						}
						else
						{
							// 로드 실패시 메모리 해제
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
		// 파일시스템 오류 처리
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

	// 스프라이트를 찾지 못한 경우
	wcout << L"Sprite not found: " << spriteName << endl;
	return nullptr;
}