#pragma once
#include <Engine/Object/Component/Component.h>

class Object;
class SpriteRenderer;
class BoxCollider;

// 맵 데이터 구조체
struct MapData
{
    int gridSize = 16;
    int roomWidth = 50;
    int roomHeight = 30;
    int roomType = 0;
    std::wstring roomTypeName;

    // 레이어별 데이터
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

// 타일 오브젝트 정보
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
    DungeonGenerator(const DungeonGenerator& other); // 복사 생성자
protected:
    Component* CloneImpl() const override;
public:
    // Component 인터페이스 구현
    virtual void Init() override;
    virtual void Update(float deltaTime) override;
    virtual void Render(HDC hdc) override;
    virtual void OnDestroy() override;

    // 던전 생성 관련 함수들
    void LoadMapFromFile(const std::wstring& filePath);
    void GenerateDungeon();
    void ClearDungeon();

    // 타일 생성 및 관리
    Object* CreateTileObject(const std::wstring& tileName, int gridX, int gridY, bool hasCollider = false);
    void SetTilePosition(Object* tileObject, int gridX, int gridY);

    // 맵 데이터 접근자
    const MapData& GetMapData() const { return m_mapData; }
    const std::vector<TileObject>& GetTileObjects() const { return m_tileObjects; }

    // 설정
    void SetTileSize(float size) { m_tileSize = size; }
    float GetTileSize() const { return m_tileSize; }

private:
    // 파일 파싱 함수들
    bool ParseMapFile(const std::vector<std::wstring>& lines);
    void ParseGroundLayer(const std::vector<std::wstring>& lines, int& lineIndex);
    void ParseDecorationLayer(const std::vector<std::wstring>& lines, int& lineIndex);
    void ParseCliffLayer(const std::vector<std::wstring>& lines, int& lineIndex);
    void ParseColliderLayer(const std::vector<std::wstring>& lines, int& lineIndex);

    // 유틸리티 함수들
    std::vector<std::wstring> SplitString(const std::wstring& str, wchar_t delimiter);
    std::wstring Trim(const std::wstring& str);
    bool StartsWith(const std::wstring& str, const std::wstring& prefix);

    // 타일 생성 도우미 함수들
    void CreateGroundTiles();
    void CreateDecorationTiles();
    void ApplyColliders();

private:
    MapData m_mapData;
    std::vector<TileObject> m_tileObjects;

    // 설정값들
    float m_tileSize = 16.0f;
    bool m_isLoaded = false;
    bool m_isGenerated = false;

    // 리소스 관리
    std::map<std::wstring, std::wstring> m_tileResourcePaths;

    // 디버그용
    bool m_showDebugInfo = true;
    bool m_showColliders = true;
};