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
    // �� ����/�ε�
    bool SaveMap(const string& filename, TileMapEditor* tileMapEditor);
    bool LoadMap(const string& filename, TileMapEditor* tileMapEditor);

    // ���� ������ ����
    void AddProp(const PropData& prop);
    void Removeprop(int gridX, int gridY);
    void ClearProps();
    vector<PropData> GetProps() const { return m_Props; }

    // ���� ����
    bool ValidateMapFile(const string& filename);

    // ���� ȣȯ �������� ����
    bool SaveForGame(const string& filename, TileMapEditor* tileMapEditor);

private:
    vector<PropData> m_Props;

    // XML ���� �Լ���
    bool WriteXMLHeader(FILE* file);
    bool WriteRoomConfig(FILE* file, const RoomConfig& config);
    bool WriteTileData(FILE* file, TileMapEditor* tileMapEditor);
    bool WritePropData(FILE* file);
    bool WriteXMLFooter(FILE* file);

    // �ε� ���� �Լ���
    bool ParseXMLFile(const string& filename, RoomConfig& config, TileMapEditor* tileMapEditor);
    void ParseRoomAttributes(const string& roomLine, RoomConfig& config);
    void ParseTileRow(const string& row, TileMapEditor* tileMapEditor, LayerType layer, int y);
    void ParsePropData(const string& propLine);

    // ��ƿ��Ƽ �Լ���
    string EscapeXML(const string& text);
    string GetTileTypeName(int tileType);
    int GetTileTypeFromName(const string& name);
};