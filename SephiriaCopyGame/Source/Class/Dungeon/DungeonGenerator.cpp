#include "pch.h"
#include "DungeonGenerator.h"
#include <Engine/Managers/ResourceManager/ResourceManager.h>
#include <Engine/Managers/SceneManager/SceneManager.h>
#include <Engine/Object/Object/Object.h>
#include <Engine/Object/Component/SpriteRenderer/SpriteRenderer.h>
#include <Engine/Object/Component/Collider/BoxCollider.h>
#include <fstream>
#include <sstream>
#include <iostream>

DungeonGenerator::DungeonGenerator(Object* owner)
    : Component(owner)
    , m_tileSize(64.0f)
    , m_isLoaded(false)
    , m_isGenerated(false)
    , m_showDebugInfo(true)
	, m_showColliders(true)
{
}

DungeonGenerator::DungeonGenerator(const DungeonGenerator& other)
    : Component(other)
    , m_mapData(other.m_mapData)
    , m_tileObjects(other.m_tileObjects)
    , m_tileSize(other.m_tileSize)
    , m_isLoaded(other.m_isLoaded)
    , m_isGenerated(other.m_isGenerated)
    , m_showDebugInfo(other.m_showDebugInfo)
	, m_showColliders(other.m_showColliders)
{
}

Component* DungeonGenerator::CloneImpl() const
{
	return new DungeonGenerator(*this);
}

void DungeonGenerator::Init()
{
    std::wcout << L"DungeonGenerator �ʱ�ȭ��" << std::endl;
}

void DungeonGenerator::Update(float deltaTime)
{
    // �ʿ�� Ÿ�� ������Ʈ���� ������Ʈ ó��
}

void DungeonGenerator::Render(HDC hdc)
{
    Component::Render(hdc);

    if (m_showDebugInfo)
    {
        // ����� ���� ���
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);

        std::wstring debugText = L"Dungeon: " + std::to_wstring(m_tileObjects.size()) + L" tiles";
        TextOut(hdc, 10, 10, debugText.c_str(), debugText.length());

        std::wstring roomInfo = L"Room: " + std::to_wstring(m_mapData.roomWidth) +
            L"x" + std::to_wstring(m_mapData.roomHeight);
        TextOut(hdc, 10, 30, roomInfo.c_str(), roomInfo.length());
    }
}

void DungeonGenerator::OnDestroy()
{
    ClearDungeon();
}

void DungeonGenerator::LoadMapFromFile(const std::wstring& filePath)
{
    std::wifstream file(filePath);
    if (!file.is_open())
    {
        std::wcout << L"�� ������ �� �� �����ϴ�: " << filePath << std::endl;
        return;
    }

    std::vector<std::wstring> lines;
    std::wstring line;
    while (std::getline(file, line))
    {
        lines.push_back(line);
    }
    file.close();

    if (ParseMapFile(lines))
    {
        m_isLoaded = true;
        std::wcout << L"�� ���� �ε� ����: " << filePath << std::endl;
        std::wcout << L"�� ũ��: " << m_mapData.roomWidth << L"x" << m_mapData.roomHeight << std::endl;
        std::wcout << L"�׶��� Ÿ��: " << m_mapData.groundTiles.size() << L"��" << std::endl;
        std::wcout << L"��� Ÿ��: " << m_mapData.decorationTiles.size() << L"��" << std::endl;
    }
    else
    {
        std::wcout << L"�� ���� �Ľ� ����: " << filePath << std::endl;
    }
}

void DungeonGenerator::GenerateDungeon()
{
    if (!m_isLoaded)
    {
        std::wcout << L"�� �����Ͱ� �ε���� �ʾҽ��ϴ�." << std::endl;
        return;
    }

    // ���� ���� ����
    ClearDungeon();

    std::wcout << L"���� ���� ����..." << std::endl;

    // 1. �׶��� Ÿ�� ����
    CreateGroundTiles();

    // 2. ��� Ÿ�� ����
    CreateDecorationTiles();

    // 3. �ݶ��̴� ����
    ApplyColliders();

    m_isGenerated = true;
    std::wcout << L"���� ���� �Ϸ�: " << m_tileObjects.size() << L"�� Ÿ��" << std::endl;
}

void DungeonGenerator::ClearDungeon()
{
    for (auto& tileObj : m_tileObjects)
    {
        if (tileObj.tileObject)
        {
            delete tileObj.tileObject;
        }
    }
    m_tileObjects.clear();
    m_isGenerated = false;
}

Object* DungeonGenerator::CreateTileObject(const std::wstring& tileName, int gridX, int gridY, bool hasCollider)
{
	Scene* currentScene = SceneManager::GetInstance()->GetCurrentScene();
    if (!currentScene)
    {
        std::wcout << L"���� ���� �����ϴ�. Ÿ�� ������Ʈ�� ������ �� �����ϴ�." << std::endl;
        return nullptr;
	}

    // string ��ȯ
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, tileName.c_str(), (int)tileName.size(), NULL, 0, NULL, NULL);
    string tileNamestring(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, tileName.c_str(), (int)tileName.size(), &tileNamestring[0], sizeNeeded, NULL, NULL);

    // �׸���� Ÿ�� ������� �̸� ���ϱ�
    string objectName;
    if (tileNamestring.empty())
    {
        objectName = "Collider_x" + std::to_string(gridX) + "_y" + std::to_string(gridY);
    }
    else
    {
        objectName = tileNamestring + "_x" + std::to_string(gridX) + "_y" + std::to_string(gridY);
    }

    //�̸� ����
    Object* tileObject = currentScene->CreateGameObject(objectName);

    // ��ġ ����
    SetTilePosition(tileObject, gridX, gridY);

    // SpriteRenderer ������Ʈ �߰�
    SpriteRenderer* spriteRenderer = tileObject->AddComponent<SpriteRenderer>();
    spriteRenderer->SetSprite(tileName);
	spriteRenderer->SetSize(64.0f, 64.0f);

    // ���� ���ҽ� �ε� ������ ResourceManager ������ ���� �޶��� �� ����

    // �ݶ��̴��� �ʿ��� ��� �߰�
    BoxCollider* boxCollider = nullptr;
    if (hasCollider)
    {
        boxCollider = tileObject->AddComponent<BoxCollider>();
		Vector2 colliderSize(m_tileSize, m_tileSize);
        boxCollider->SetSize(colliderSize);
    }

    // TileObject ����ü ���� �� �߰�
    TileObject tileObjInfo;
    tileObjInfo.tileObject = tileObject;
    tileObjInfo.spriteRenderer = spriteRenderer;
    tileObjInfo.boxCollider = boxCollider;
    tileObjInfo.gridX = gridX;
    tileObjInfo.gridY = gridY;
    tileObjInfo.hasCollider = hasCollider;

    m_tileObjects.push_back(tileObjInfo);

    return tileObject;
}

void DungeonGenerator::SetTilePosition(Object* tileObject, int gridX, int gridY)
{
    if (!tileObject) return;

    float worldX = gridX * m_tileSize;
    float worldY = gridY * m_tileSize;

    tileObject->SetPosition(worldX, worldY);
}

bool DungeonGenerator::ParseMapFile(const std::vector<std::wstring>& lines)
{
    try
    {
        int lineIndex = 0;
        std::wstring currentSection = L"";

        // �ʱ�ȭ
        m_mapData.cliffLayer.clear();
        m_mapData.colliderLayer.clear();
        m_mapData.groundTiles.clear();
        m_mapData.decorationTiles.clear();

        while (lineIndex < lines.size())
        {
            std::wstring line = Trim(lines[lineIndex]);

            // �� ���̳� �ּ� �ǳʶٱ�
            if (line.empty() || line[0] == L'#')
            {
                lineIndex++;
                continue;
            }

            // ��� ���� �Ľ�
            if (line.find(L'=') != std::wstring::npos)
            {
                size_t pos = line.find(L'=');
                std::wstring key = Trim(line.substr(0, pos));
                std::wstring value = Trim(line.substr(pos + 1));

                if (key == L"GridSize")
                    m_mapData.gridSize = std::stoi(value);
                else if (key == L"RoomWidth")
                    m_mapData.roomWidth = std::stoi(value);
                else if (key == L"RoomHeight")
                    m_mapData.roomHeight = std::stoi(value);
                else if (key == L"RoomType")
                    m_mapData.roomType = std::stoi(value);
                else if (key == L"RoomTypeName")
                    m_mapData.roomTypeName = value;

                lineIndex++;
                continue;
            }

            // ���� ��� üũ
            if (line[0] == L'[' && line.back() == L']')
            {
                currentSection = line.substr(1, line.length() - 2);
                lineIndex++;
                continue;
            }

            // ���� ���� �Ľ�
            if (currentSection == L"GroundLayer")
            {
                ParseGroundLayer(lines, lineIndex);
            }
            else if (currentSection == L"DecorationLayer")
            {
                ParseDecorationLayer(lines, lineIndex);
            }
            else if (currentSection == L"CliffLayer")
            {
                ParseCliffLayer(lines, lineIndex);
                break; // CliffLayer �Ľ� �� �ٷ� ColliderLayer�� �Ѿ
            }
            else
            {
                lineIndex++;
            }
        }

        // ColliderLayer �Ľ�
        for (int i = lineIndex; i < lines.size(); i++)
        {
            if (Trim(lines[i]) == L"[ColliderLayer]")
            {
                lineIndex = i + 1;
                ParseColliderLayer(lines, lineIndex);
                break;
            }
        }

        return true;
    }
    catch (const std::exception& e)
    {
        std::cout << "�� ���� �Ľ� �� ����: " << e.what() << std::endl;
        return false;
    }
}

void DungeonGenerator::ParseGroundLayer(const std::vector<std::wstring>& lines, int& lineIndex)
{
    while (lineIndex < lines.size())
    {
        std::wstring line = Trim(lines[lineIndex]);

        if (line.empty() || line[0] == L'[')
            break;

        if (StartsWith(line, L"TileCount="))
        {
            lineIndex++;
            continue;
        }

        // Ÿ�� ������ �Ľ�: "TileName X Y"
        auto parts = SplitString(line, L' ');
        if (parts.size() >= 3)
        {
            MapData::TileInfo tile;
            tile.tileName = parts[0];
            tile.x = std::stoi(parts[1]);
            tile.y = std::stoi(parts[2]);
            m_mapData.groundTiles.push_back(tile);
        }

        lineIndex++;
    }
}

void DungeonGenerator::ParseDecorationLayer(const std::vector<std::wstring>& lines, int& lineIndex)
{
    while (lineIndex < lines.size())
    {
        std::wstring line = Trim(lines[lineIndex]);

        if (line.empty() || line[0] == L'[')
            break;

        if (StartsWith(line, L"TileCount="))
        {
            lineIndex++;
            continue;
        }

        // Ÿ�� ������ �Ľ�: "TileName X Y"
        auto parts = SplitString(line, L' ');
        if (parts.size() >= 3)
        {
            MapData::TileInfo tile;
            tile.tileName = parts[0];
            tile.x = std::stoi(parts[1]);
            tile.y = std::stoi(parts[2]);
            m_mapData.decorationTiles.push_back(tile);
        }

        lineIndex++;
    }
}

void DungeonGenerator::ParseCliffLayer(const std::vector<std::wstring>& lines, int& lineIndex)
{
    m_mapData.cliffLayer.resize(m_mapData.roomHeight);

    for (int y = 0; y < m_mapData.roomHeight && lineIndex < lines.size(); y++)
    {
        std::wstring line = Trim(lines[lineIndex]);
        if (line.length() >= m_mapData.roomWidth)
        {
            m_mapData.cliffLayer[y].resize(m_mapData.roomWidth);
            for (int x = 0; x < m_mapData.roomWidth; x++)
            {
                m_mapData.cliffLayer[y][x] = (line[x] == L'1');
            }
        }
        lineIndex++;
    }
}

void DungeonGenerator::ParseColliderLayer(const std::vector<std::wstring>& lines, int& lineIndex)
{
    m_mapData.colliderLayer.resize(m_mapData.roomHeight);

    for (int y = 0; y < m_mapData.roomHeight && lineIndex < lines.size(); y++)
    {
        std::wstring line = Trim(lines[lineIndex]);
        if (line.length() >= m_mapData.roomWidth)
        {
            m_mapData.colliderLayer[y].resize(m_mapData.roomWidth);
            for (int x = 0; x < m_mapData.roomWidth; x++)
            {
                m_mapData.colliderLayer[y][x] = (line[x] == L'1');
            }
        }
        lineIndex++;
    }
}

void DungeonGenerator::CreateGroundTiles()
{
    for (const auto& tile : m_mapData.groundTiles)
    {
        CreateTileObject(tile.tileName, tile.x, tile.y, false);
    }

    std::wcout << L"�׶��� Ÿ�� " << m_mapData.groundTiles.size() << L"�� ����" << std::endl;
}

void DungeonGenerator::CreateDecorationTiles()
{
    for (const auto& tile : m_mapData.decorationTiles)
    {
        CreateTileObject(tile.tileName, tile.x, tile.y, false);
    }

    std::wcout << L"��� Ÿ�� " << m_mapData.decorationTiles.size() << L"�� ����" << std::endl;
}

void DungeonGenerator::ApplyColliders()
{
    int colliderCount = 0;

    // �ݶ��̴� ���̾� �����͸� Ȯ���Ͽ� ���� Ÿ�Ͽ� �ݶ��̴� �߰�
    for (int y = 0; y < m_mapData.roomHeight; y++)
    {
        for (int x = 0; x < m_mapData.roomWidth; x++)
        {
            if (y < m_mapData.colliderLayer.size() &&
                x < m_mapData.colliderLayer[y].size() &&
                m_mapData.colliderLayer[y][x])
            {
                // �ش� ��ġ�� �̹� Ÿ���� �ִ��� Ȯ��
                bool foundTile = false;
                for (auto& tileObj : m_tileObjects)
                {
                    if (tileObj.gridX == x && tileObj.gridY == y)
                    {
                        // ���� Ÿ�Ͽ� �ݶ��̴� �߰�
                        if (!tileObj.hasCollider && !tileObj.boxCollider)
                        {
                            BoxCollider* boxCollider = tileObj.tileObject->AddComponent<BoxCollider>();
							Vector2 colliderSize(m_tileSize, m_tileSize);
                            boxCollider->SetSize(colliderSize);
                            //boxCollider->SetLayer(CollisionLayer::Wall);

                            tileObj.boxCollider = boxCollider;
                            tileObj.hasCollider = true;
                            colliderCount++;
                        }
                        foundTile = true;
                        break;
                    }
                }

                // Ÿ���� ���� ��ġ���� ������ �ʴ� �ݶ��̴� Ÿ�� ����
                if (!foundTile)
                {
                    Object* colliderTile = CreateTileObject(L"", x, y, true);
                    colliderCount++;
                }
            }
        }
    }

    std::wcout << L"�ݶ��̴� " << colliderCount << L"�� ����" << std::endl;
}

// ��ƿ��Ƽ �Լ���
std::vector<std::wstring> DungeonGenerator::SplitString(const std::wstring& str, wchar_t delimiter)
{
    std::vector<std::wstring> result;
    std::wstringstream ss(str);
    std::wstring item;

    while (std::getline(ss, item, delimiter))
    {
        if (!item.empty())
            result.push_back(item);
    }

    return result;
}

std::wstring DungeonGenerator::Trim(const std::wstring& str)
{
    size_t start = str.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) return L"";

    size_t end = str.find_last_not_of(L" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool DungeonGenerator::StartsWith(const std::wstring& str, const std::wstring& prefix)
{
    return str.length() >= prefix.length() &&
        str.substr(0, prefix.length()) == prefix;
}