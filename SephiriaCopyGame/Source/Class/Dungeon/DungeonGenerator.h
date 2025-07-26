#pragma once
#include <Engine/Object/Component/Component.h>

class Object;
class SpriteRenderer;
class BoxCollider;

// �� ������ ����ü
struct MapData
{
    int gridSize = 16;
    int roomWidth = 50;
    int roomHeight = 30;
    int roomType = 0;
    std::wstring roomTypeName;

    // ���̾ ������
    struct TileInfo
    {
        std::wstring tileName;
        int x, y;
    };

    std::vector<TileInfo> groundTiles;
    std::vector<TileInfo> decorationTiles;
    std::vector<std::vector<bool>> cliffLayer;
    std::vector<std::vector<bool>> colliderLayer;
};

// Ÿ�� ������Ʈ ����
struct TileObject
{
    Object* tileObject = nullptr;
    SpriteRenderer* spriteRenderer = nullptr;
    BoxCollider* boxCollider = nullptr;
    int gridX = 0;
    int gridY = 0;
    bool hasCollider = false;
};

class DungeonGenerator : public Component
{
public:
    DungeonGenerator(Object* owner);
    DungeonGenerator(const DungeonGenerator& other); // ���� ������
protected:
    Component* CloneImpl() const override;
public:
    // Component �������̽� ����
    virtual void Init() override;
    virtual void Update(float deltaTime) override;
    virtual void Render(HDC hdc) override;
    virtual void OnDestroy() override;

    // ���� ���� ���� �Լ���
    void LoadMapFromFile(const std::wstring& filePath);
    void GenerateDungeon();
    void ClearDungeon();

    // Ÿ�� ���� �� ����
    Object* CreateTileObject(const std::wstring& tileName, int gridX, int gridY, bool hasCollider = false);
    void SetTilePosition(Object* tileObject, int gridX, int gridY);

    // �� ������ ������
    const MapData& GetMapData() const { return m_mapData; }
    const std::vector<TileObject>& GetTileObjects() const { return m_tileObjects; }

    // ����
    void SetTileSize(float size) { m_tileSize = size; }
    float GetTileSize() const { return m_tileSize; }

private:
    // ���� �Ľ� �Լ���
    bool ParseMapFile(const std::vector<std::wstring>& lines);
    void ParseGroundLayer(const std::vector<std::wstring>& lines, int& lineIndex);
    void ParseDecorationLayer(const std::vector<std::wstring>& lines, int& lineIndex);
    void ParseCliffLayer(const std::vector<std::wstring>& lines, int& lineIndex);
    void ParseColliderLayer(const std::vector<std::wstring>& lines, int& lineIndex);

    // ��ƿ��Ƽ �Լ���
    std::vector<std::wstring> SplitString(const std::wstring& str, wchar_t delimiter);
    std::wstring Trim(const std::wstring& str);
    bool StartsWith(const std::wstring& str, const std::wstring& prefix);

    // Ÿ�� ���� ����� �Լ���
    void CreateGroundTiles();
    void CreateDecorationTiles();
    void ApplyColliders();

private:
    MapData m_mapData;
    std::vector<TileObject> m_tileObjects;

    // ��������
    float m_tileSize = 16.0f;
    bool m_isLoaded = false;
    bool m_isGenerated = false;

    // ���ҽ� ����
    std::map<std::wstring, std::wstring> m_tileResourcePaths;

    // ����׿�
    bool m_showDebugInfo = true;
    bool m_showColliders = true;
};