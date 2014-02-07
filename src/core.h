#ifndef CORE_H
#define CORE_H

#define chipsetIsNull() Core::getCore()->tile(0).isNull()
#define mCore() Core::getCore()

/****************************
 *     Binding Constants    *
 * **************************/
#define UP 1
#define DOWN 2
#define LEFT 4
#define RIGHT 8
#define UPLEFT 16
#define UPRIGHT  32
#define DOWNLEFT  64
#define DOWNRIGHT  128
/****************************/
#define EV 0x7FFE

#define CURRENT_PROJECT_KEY "current_project"
#define DEFAULT_DIR_KEY "default_dir"

#include <QPixmap>
#include <QListWidget>
#include "rpg_map.h"
#include "rpg_chipset.h"

class Core
{
public:

    enum Layer
    {
        LOWER,
        UPPER,
        EVENT
    };

    enum Tool
    {
        SELECTION,
        ZOOM,
        PENCIL,
        RECTANGLE,
        CIRCLE,
        FILL
    };

    Core();

    static Core* getCore();

    void LoadMaps();
    void LoadChipset();

    int tileSize();
    void setTileSize(int tileSize);

    QString projectPath();
    void setprojectPath(const QString &projectPath);

    Layer layer();
    void setLayer(const Layer &layer);

    Tool tool();
    void setTool(const Tool &tool);

    QString gameTitle();
    void setGameTitle(const QString &gameTitle);

    RPG::Map *map();
    void setMap(int id);

    QPixmap tile(short tile_id);

    short translate(int terrain_id, int _code = 0, int _scode = 0);
    int translate(short tile_id);

    bool isWater(int terrain_id);
    bool isABWater(int terrain_id);
    bool isDWater(int terrain_id);
    bool isAnimation(int terrain_id);
    bool isDblock(int terrain_id);
    bool isEblock(int terrain_id);
    bool isFblock(int terrain_id);

    private:
    RPG::Map *m_map;
    RPG::Chipset m_chipset;
    int m_tileSize;
    QString m_gameTitle;
    QString m_projectPath;
    Layer m_layer;
    Tool m_tool;
    QMap<int, QPixmap> m_tileCache;
    QMap<int, short> m_dictionary;
    QMap<int, RPG::Map> m_maps;
    static Core *core;
};

#endif // CORE_H
