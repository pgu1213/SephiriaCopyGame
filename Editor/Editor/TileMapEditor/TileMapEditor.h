#pragma once
#include "../../Managers/SingletonManager/SingletonManager.h"

struct TileInfo
{
    int tileID;      // 타일의 ID (리소스 매니저의 타일 ID와 매칭)
    wstring fileName; // 타일 파일명
};

struct ColliderTile
{
    bool hasCollider; // 콜라이더가 있는지 여부
};

enum class LayerType
{
    GROUND = 0,      // 1번 레이어: Ground
    UPPER_GROUND = 1, // 2번 레이어: UpperGround  
    COLLIDER = 2     // 3번 레이어: Collider
};

class TileMapEditor
{
public:
    TileMapEditor();
    ~TileMapEditor();

public:
    void Initialize();
    void Update();
    void Render(HDC hdc);
    void HandleInput();
    void SetCurrentLayer(LayerType layer) { m_CurrentLayer = layer; }
    LayerType GetCurrentLayer() const { return m_CurrentLayer; }
    void SetSelectedTileID(int tileID);
    void SaveMap(const wstring& fileName);
    void LoadMap(const wstring& fileName);
    void ClearMap();
    void SetGridSize(int width, int height);

    // 성능 최적화용 메서드
    void SetViewport(int x, int y, int width, int height);
    void UpdateVisibleTiles();

    // 렌더링 최적화
    void RenderLayer(HDC hdc, LayerType layer);
    void RenderVisibleTiles(HDC hdc);

private:
    void PlaceTile(int x, int y);
    void RemoveTile(int x, int y);
    void InvalidateRegion(int x, int y, int width, int height, LayerType layer);
    void UpdateRenderBatches();
    void RenderWithBatching(HDC hdc);
    bool IsValidPosition(int x, int y) const;
    void RenderGrid(HDC hdc);
    void RenderColliderLayer(HDC hdc);
    int GetTileIDFromResourceManager(const wstring& fileName);
    wstring GetFileNameFromTileID(int tileID);

private:
    // 맵 데이터 - 레이어별로 분리
    vector<vector<TileInfo>> m_GroundLayer;        // 레이어 1: Ground (타일 ID 저장)
    vector<vector<TileInfo>> m_UpperGroundLayer;   // 레이어 2: UpperGround (타일 ID 저장)  
    vector<vector<ColliderTile>> m_ColliderLayer;  // 레이어 3: Collider (0/1 저장)

    // 에디터 상태
    LayerType m_CurrentLayer;
    int m_SelectedTileID;
    wstring m_SelectedTileFileName;

    // 맵 크기
    int m_MapWidth;
    int m_MapHeight;
    int m_TileSize;

    // 카메라/뷰포트 (성능 최적화용)
    int m_ViewportX, m_ViewportY;
    int m_ViewportWidth, m_ViewportHeight;
    int m_VisibleStartX, m_VisibleStartY;
    int m_VisibleEndX, m_VisibleEndY;

    // 성능 최적화 관련
    bool m_NeedUpdate;

    struct RenderBatch
    {
        vector<RECT> positions;
        Gdiplus::Bitmap* bitmap;
        int tileID;
    };

    map<int, RenderBatch> m_RenderBatches[3]; // 레이어별 배치


    // 더티 영역 관리
    struct DirtyRegion
    {
        int x, y, width, height;
        LayerType layer;
    };

    vector<DirtyRegion> m_DirtyRegions; // 업데이트가 필요한 영역들

    // 타일 ID와 파일명 매핑 (리소스 매니저와 동기화)
    map<int, wstring> m_TileIDToFileName;
    map<wstring, int> m_FileNameToTileID;

    // UI 관련
    bool m_ShowGrid;
    bool m_ShowColliders;

    bool m_BatchesDirty;
};

// 메모리 풀링 시스템 추가 (큰 맵을 위한 최적화)
class TileMemoryPool
{
private:
    vector<TileInfo*> m_FreeTileInfos;
    vector<ColliderTile*> m_FreeColliderTiles;
    static const int POOL_SIZE = 10000;

public:
    TileMemoryPool()
    {
        // 미리 메모리 할당
        for (int i = 0; i < POOL_SIZE; ++i)
        {
            m_FreeTileInfos.push_back(new TileInfo{ 0, L"" });
            m_FreeColliderTiles.push_back(new ColliderTile{ false });
        }
    }

    ~TileMemoryPool()
    {
        for (auto* tile : m_FreeTileInfos)
            delete tile;
        for (auto* collider : m_FreeColliderTiles)
            delete collider;
    }

    TileInfo* GetTileInfo()
    {
        if (!m_FreeTileInfos.empty())
        {
            TileInfo* tile = m_FreeTileInfos.back();
            m_FreeTileInfos.pop_back();
            tile->tileID = 0;
            tile->fileName = L"";
            return tile;
        }
        return new TileInfo{ 0, L"" };
    }

    void ReturnTileInfo(TileInfo* tile)
    {
        if (m_FreeTileInfos.size() < POOL_SIZE)
        {
            m_FreeTileInfos.push_back(tile);
        }
        else
        {
            delete tile;
        }
    }

    ColliderTile* GetColliderTile()
    {
        if (!m_FreeColliderTiles.empty())
        {
            ColliderTile* collider = m_FreeColliderTiles.back();
            m_FreeColliderTiles.pop_back();
            collider->hasCollider = false;
            return collider;
        }
        return new ColliderTile{ false };
    }

    void ReturnColliderTile(ColliderTile* collider)
    {
        if (m_FreeColliderTiles.size() < POOL_SIZE)
        {
            m_FreeColliderTiles.push_back(collider);
        }
        else
        {
            delete collider;
        }
    }
};