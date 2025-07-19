#pragma once
#include "../SingletonManager/SingletonManager.h"
#include "../ConfigManager/ConfigManager.h"
#include "../../Editor/TileMapEditor/TileMapEditor.h"

class TileMapEditor;

struct PropData
{
    int gridX;
    int gridY;
    string propType;
    string spriteName;

    PropData() : gridX(0), gridY(0), propType(""), spriteName("") {}
    PropData(int x, int y, const string& type, const string& sprite)
        : gridX(x), gridY(y), propType(type), spriteName(sprite) {
    }
};

class FileManager : public SingleTon<FileManager>
{
    friend class SingleTon<FileManager>;
private:
    explicit FileManager();
public:
    virtual ~FileManager();

public:
    // 맵 저장/로딩
    bool SaveMap(const string& filename, TileMapEditor* tileMapEditor);
    bool LoadMap(const string& filename, TileMapEditor* tileMapEditor);

    // 프롭 데이터 관리
    void AddProp(const PropData& prop);
    void Removeprop(int gridX, int gridY);
    void ClearProps();
    vector<PropData> GetProps() const { return m_Props; }

    // 파일 검증
    bool ValidateMapFile(const string& filename);

    // 게임 호환 포맷으로 저장
    bool SaveForGame(const string& filename, TileMapEditor* tileMapEditor);

private:
    vector<PropData> m_Props;

    // XML 헬퍼 함수들
    bool WriteXMLHeader(FILE* file);
    bool WriteRoomConfig(FILE* file, const RoomConfig& config);
    bool WriteTileData(FILE* file, TileMapEditor* tileMapEditor);
    bool WritePropData(FILE* file);
    bool WriteXMLFooter(FILE* file);

    // 로딩 헬퍼 함수들
    bool ParseXMLFile(const string& filename, RoomConfig& config, TileMapEditor* tileMapEditor);
    void ParseRoomAttributes(const string& roomLine, RoomConfig& config);
    void ParseTileRow(const string& row, TileMapEditor* tileMapEditor, LayerType layer, int y);
    void ParsePropData(const string& propLine);

    // 유틸리티 함수들
    string EscapeXML(const string& text);
    string GetTileTypeName(int tileType);
    int GetTileTypeFromName(const string& name);
};