#pragma once
#include "../../Managers/SingletonManager/SingletonManager.h"

struct TileInfo
{
    int tileID;      // Ÿ���� ID (���ҽ� �Ŵ����� Ÿ�� ID�� ��Ī)
    wstring fileName; // Ÿ�� ���ϸ�
};

struct ColliderTile
{
    bool hasCollider; // �ݶ��̴��� �ִ��� ����
};

enum class LayerType
{
    GROUND = 0,      // 1�� ���̾�: Ground
    UPPER_GROUND = 1, // 2�� ���̾�: UpperGround  
    COLLIDER = 2     // 3�� ���̾�: Collider
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

    // ���� ����ȭ�� �޼���
    void SetViewport(int x, int y, int width, int height);
    void UpdateVisibleTiles();

    // ������ ����ȭ
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
    // �� ������ - ���̾�� �и�
    vector<vector<TileInfo>> m_GroundLayer;        // ���̾� 1: Ground (Ÿ�� ID ����)
    vector<vector<TileInfo>> m_UpperGroundLayer;   // ���̾� 2: UpperGround (Ÿ�� ID ����)  
    vector<vector<ColliderTile>> m_ColliderLayer;  // ���̾� 3: Collider (0/1 ����)

    // ������ ����
    LayerType m_CurrentLayer;
    int m_SelectedTileID;
    wstring m_SelectedTileFileName;

    // �� ũ��
    int m_MapWidth;
    int m_MapHeight;
    int m_TileSize;

    // ī�޶�/����Ʈ (���� ����ȭ��)
    int m_ViewportX, m_ViewportY;
    int m_ViewportWidth, m_ViewportHeight;
    int m_VisibleStartX, m_VisibleStartY;
    int m_VisibleEndX, m_VisibleEndY;

    // ���� ����ȭ ����
    bool m_NeedUpdate;

    struct RenderBatch
    {
        vector<RECT> positions;
        Gdiplus::Bitmap* bitmap;
        int tileID;
    };

    map<int, RenderBatch> m_RenderBatches[3]; // ���̾ ��ġ


    // ��Ƽ ���� ����
    struct DirtyRegion
    {
        int x, y, width, height;
        LayerType layer;
    };

    vector<DirtyRegion> m_DirtyRegions; // ������Ʈ�� �ʿ��� ������

    // Ÿ�� ID�� ���ϸ� ���� (���ҽ� �Ŵ����� ����ȭ)
    map<int, wstring> m_TileIDToFileName;
    map<wstring, int> m_FileNameToTileID;

    // UI ����
    bool m_ShowGrid;
    bool m_ShowColliders;

    bool m_BatchesDirty;
};

// �޸� Ǯ�� �ý��� �߰� (ū ���� ���� ����ȭ)
class TileMemoryPool
{
private:
    vector<TileInfo*> m_FreeTileInfos;
    vector<ColliderTile*> m_FreeColliderTiles;
    static const int POOL_SIZE = 10000;

public:
    TileMemoryPool()
    {
        // �̸� �޸� �Ҵ�
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