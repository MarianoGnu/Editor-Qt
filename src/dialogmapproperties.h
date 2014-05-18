#ifndef DIALOGMAPPROPERTIES_H
#define DIALOGMAPPROPERTIES_H

#include <QDialog>
#include <QGraphicsPixmapItem>
#include <QTableWidgetItem>
#include <rpg_mapinfo.h>
#include <rpg_map.h>
#include "tools/qencounterdelegate.h"

namespace Ui {
class DialogMapProperties;
}

class DialogMapProperties : public QDialog
{
    Q_OBJECT

public:
    enum Page
    {
        Main    = 0,
        Areas   = 1,
        Dungeon = 2
    };

    explicit DialogMapProperties(RPG::MapInfo &info, RPG::Map &map, Page p = Main, QWidget *parent = 0);
    ~DialogMapProperties();

private slots:
    void on_groupPanorama_toggled(bool arg1);

    void on_groupUseGenerator_toggled(bool arg1);

    void on_groupUpperWall_toggled(bool arg1);

    void on_groupFloorB_toggled(bool arg1);

    void on_groupFloorC_toggled(bool arg1);

    void on_groupObstacleB_toggled(bool arg1);

    void on_groupObstacleC_toggled(bool arg1);

    void on_tableEncounters_itemChanged(QTableWidgetItem *item);

    void on_lineListFilter_textChanged(const QString &arg1);

    void on_listAreas_currentRowChanged(int currentRow);

    void on_lineEncounterAreaName_textChanged(const QString &arg1);

private:
    Ui::DialogMapProperties *ui;

    QGraphicsPixmapItem *m_panoramaItem;
    QGraphicsPixmapItem *m_ceilingItem;
    QGraphicsPixmapItem *m_lowerWallItem;
    QGraphicsPixmapItem *m_upperWallItem;
    QGraphicsPixmapItem *m_floorAItem;
    QGraphicsPixmapItem *m_floorBItem;
    QGraphicsPixmapItem *m_floorCItem;
    QGraphicsPixmapItem *m_obstacleAItem;
    QGraphicsPixmapItem *m_obstacleBItem;
    QGraphicsPixmapItem *m_obstacleCItem;
    QGraphicsPolygonItem *m_areaRectItem;

    QEncounterDelegate *m_encounterDelegate;

    RPG::MapInfo &m_info;
    RPG::Map &m_map;

    std::vector<short> m_generatorLowerLayer;
    std::vector<short> m_generatorUpperLayer;

    std::vector<RPG::MapInfo> m_areas;
    int m_currentArea;
};

#endif // DIALOGMAPPROPERTIES_H
