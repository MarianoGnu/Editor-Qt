#include "dialogmapproperties.h"
#include "ui_dialogmapproperties.h"
#include <data.h>
#include "core.h"

DialogMapProperties::DialogMapProperties(RPG::MapInfo &info, RPG::Map &map, Page p, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMapProperties),
    m_info(info),
    m_map(map)
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(p);

    for (int terrain = 0; terrain < 162; terrain++)
        m_generatorLowerLayer.push_back(mCore->translate(terrain, UP+DOWN+LEFT+RIGHT));
    for (short tile_id = 10000; tile_id < 10144; tile_id++)
        m_generatorUpperLayer.push_back(tile_id);

    m_panoramaItem = new QGraphicsPixmapItem();
    m_ceilingItem = new QGraphicsPixmapItem();
    m_lowerWallItem = new QGraphicsPixmapItem();
    m_upperWallItem = new QGraphicsPixmapItem();
    m_floorAItem = new QGraphicsPixmapItem();
    m_floorBItem = new QGraphicsPixmapItem();
    m_floorCItem = new QGraphicsPixmapItem();
    m_obstacleAItem = new QGraphicsPixmapItem();
    m_obstacleBItem = new QGraphicsPixmapItem();
    m_obstacleCItem = new QGraphicsPixmapItem();

    ui->lineName->setText(QString::fromStdString(info.name));
    ui->lineBGMname->setText(QString::fromStdString(info.music.name));
    ui->lineBackdropName->setText(QString::fromStdString(info.background_name));
    for (int i = 0; i < (int)Data::chipsets.size(); i++)
        ui->comboTileset->addItem(QString::fromStdString(Data::chipsets[i].name), i+1);
    ui->comboTileset->setCurrentIndex(map.chipset_id-1);
    ui->comboWrapping->setCurrentIndex(map.scroll_type);
    ui->spinDungeonRoomHeight->setValue(map.generator_height);
    ui->spinDungeonRoomWidth->setValue(map.generator_width);
    ui->spinEncontrerRate->setValue(info.encounter_steps);
    ui->spinHeight->setValue(map.height);
    ui->spinWidth->setValue(map.width);
    ui->spinHorizontalScrollSpeed->setValue(map.parallax_sx);
    ui->spinVerticalScrollSpeed->setValue(map.parallax_sy);
    ui->checkHorizontalAutoscroll->setChecked(map.parallax_auto_loop_x);
    ui->checkVerticalAutoscroll->setChecked(map.parallax_auto_loop_y);
    ui->checkDungeonSurroundWithWalls->setChecked(map.generator_surround);
    ui->groupHorizontalScroll->setChecked(map.parallax_loop_x);
    ui->groupVerticalScroll->setChecked(map.parallax_loop_y);
    ui->groupPanorama->setChecked(map.parallax_flag);
    ui->groupUseGenerator->setChecked(map.generator_flag);
    ui->radioBackdropParent->setChecked(info.background_type == 0);
    ui->radioBackdropTerrain->setChecked(info.background_type == 1);
    ui->radioBackdropSpecify->setChecked(info.background_type == 2);
    ui->radioBGMparent->setChecked(info.music_type == 0);
    ui->radioBGMevent->setChecked(info.music_type == 1);
    ui->radioBGMspecify->setChecked(info.music_type == 2);
    ui->radioTeleportParent->setChecked(info.teleport == 0);
    ui->radioTeleportAllow->setChecked(info.teleport == 1);
    ui->radioTeleportForbid->setChecked(info.teleport == 2);
    ui->radioEscapeParent->setChecked(info.escape == 0);
    ui->radioEscapeAllow->setChecked(info.escape == 1);
    ui->radioEscapeForbid->setChecked(info.escape == 2);
    ui->radioSaveParent->setChecked(info.save == 0);
    ui->radioSaveAllow->setChecked(info.save == 1);
    ui->radioSaveForbid->setChecked(info.save == 2);
    ui->radioDungeonSinglePassage->setChecked(map.generator_mode == 0);
    ui->radioDungeonLinkedRooms->setChecked(map.generator_mode == 1);
    ui->radioDungeonMaze->setChecked(map.generator_mode == 2);
    ui->radioDungeonOpenRoom->setChecked(map.generator_mode == 3);
    ui->radioDungeonPassage1_1->setChecked(map.generator_tiles == 0);
    ui->radioDungeonPassage2_2->setChecked(map.generator_tiles == 1);


    m_encounterDelegate = new QEncounterDelegate(this);
    ui->tableEncounters->setItemDelegate(m_encounterDelegate);

    ui->graphicsPanorama->setScene(new QGraphicsScene(this));
    ui->graphicsCeiling->setScene(new QGraphicsScene(this));
    ui->graphicsFloorA->setScene(new QGraphicsScene(this));
    ui->graphicsFloorB->setScene(new QGraphicsScene(this));
    ui->graphicsFloorC->setScene(new QGraphicsScene(this));
    ui->graphicsLowerWall->setScene(new QGraphicsScene(this));
    ui->graphicsObstacleA->setScene(new QGraphicsScene(this));
    ui->graphicsObstacleB->setScene(new QGraphicsScene(this));
    ui->graphicsObstacleC->setScene(new QGraphicsScene(this));
    ui->graphicsUpperWall->setScene(new QGraphicsScene(this));

    ui->graphicsPanorama->scene()->addItem(m_panoramaItem);
    ui->graphicsCeiling->scene()->addItem(m_ceilingItem);
    ui->graphicsFloorA->scene()->addItem(m_floorAItem);
    ui->graphicsFloorB->scene()->addItem(m_floorBItem);
    ui->graphicsFloorC->scene()->addItem(m_floorCItem);
    ui->graphicsLowerWall->scene()->addItem(m_lowerWallItem);
    ui->graphicsObstacleA->scene()->addItem(m_obstacleAItem);
    ui->graphicsObstacleB->scene()->addItem(m_obstacleBItem);
    ui->graphicsObstacleC->scene()->addItem(m_obstacleCItem);
    ui->graphicsUpperWall->scene()->addItem(m_upperWallItem);


    QPixmap pix(32, 32);
    pix.fill();
    mCore->beginPainting(pix);
    mCore->renderTile(map.generator_tile_ids[0], QRect(0,0,32,32));
    mCore->endPainting();
    m_ceilingItem->setPixmap(pix);

    pix.fill();
    mCore->beginPainting(pix);
    mCore->renderTile(map.generator_tile_ids[1], QRect(0,0,32,32));
    mCore->endPainting();
    m_lowerWallItem->setPixmap(pix);

    pix.fill();
    mCore->beginPainting(pix);
    mCore->renderTile(map.generator_tile_ids[2], QRect(0,0,32,32));
    mCore->endPainting();
    m_upperWallItem->setPixmap(pix);

    pix.fill();
    mCore->beginPainting(pix);
    mCore->renderTile(map.generator_tile_ids[3], QRect(0,0,32,32));
    mCore->endPainting();
    m_floorAItem->setPixmap(pix);

    pix.fill();
    mCore->beginPainting(pix);
    mCore->renderTile(map.generator_tile_ids[4], QRect(0,0,32,32));
    mCore->endPainting();
    m_floorBItem->setPixmap(pix);

    pix.fill();
    mCore->beginPainting(pix);
    mCore->renderTile(map.generator_tile_ids[5], QRect(0,0,32,32));
    mCore->endPainting();
    m_floorCItem->setPixmap(pix);

    pix = QPixmap(64, 64);
    pix.fill();
    mCore->beginPainting(pix);
    mCore->renderTile(map.generator_tile_ids[6], QRect(0,0,32,32));
    mCore->renderTile(map.generator_tile_ids[7], QRect(32,0,32,32));
    mCore->renderTile(map.generator_tile_ids[8], QRect(0,32,32,32));
    mCore->renderTile(map.generator_tile_ids[9], QRect(32,32,32,32));
    mCore->endPainting();
    m_obstacleAItem->setPixmap(pix);

    pix.fill();
    mCore->beginPainting(pix);
    mCore->renderTile(map.generator_tile_ids[10], QRect(0,0,32,32));
    mCore->renderTile(map.generator_tile_ids[11], QRect(32,0,32,32));
    mCore->renderTile(map.generator_tile_ids[12], QRect(0,32,32,32));
    mCore->renderTile(map.generator_tile_ids[13], QRect(32,32,32,32));
    mCore->endPainting();
    m_obstacleBItem->setPixmap(pix);

    pix.fill();
    mCore->beginPainting(pix);
    mCore->renderTile(map.generator_tile_ids[14], QRect(0,0,32,32));
    mCore->renderTile(map.generator_tile_ids[15], QRect(32,0,32,32));
    mCore->renderTile(map.generator_tile_ids[16], QRect(0,32,32,32));
    mCore->renderTile(map.generator_tile_ids[17], QRect(32,32,32,32));
    mCore->endPainting();
    m_obstacleCItem->setPixmap(pix);

    if (map.parallax_flag)
    {
        pix = QPixmap(mCore->filePath(PANORAMA,QString::fromStdString(map.parallax_name)));
        if (pix.isNull())
            pix = QPixmap(mCore->rtpPath(PANORAMA,QString::fromStdString(map.parallax_name)));
        m_panoramaItem->setPixmap(pix);
    }

    if (info.parent_map == 0)
    {
        ui->radioBackdropParent->setEnabled(false);
        ui->radioBGMparent->setEnabled(false);
        ui->radioTeleportParent->setEnabled(false);
        ui->radioSaveParent->setEnabled(false);
        ui->radioEscapeParent->setEnabled(false);
    }

    m_areas.push_back(info);
    for (int i = 0; i < (int)Data::treemap.maps.size(); i++)
    {
        if (Data::treemap.maps[i].parent_map == info.ID && Data::treemap.maps[i].type == 2)
            m_areas.push_back(Data::treemap.maps[i]);
    }

    for (int i = 1; i < (int)m_areas.size(); i++)
    {
        ui->listAreas->addItem(QString::fromStdString(m_areas[i].name));
    }

    pix = QPixmap(map.width*16, map.height*16);
    mCore->beginPainting(pix);
    for (int y = 0; y < map.height; y++)
        for (int x = 0; x < map.width; x++)
        {
            mCore->renderTile(map.lower_layer[map.width*y+x], QRect(x*16,y*16,16,16));
            mCore->renderTile(map.upper_layer[map.width*y+x], QRect(x*16,y*16,16,16));
        }
    mCore->endPainting();

    ui->graphicsArea->setScene(new QGraphicsScene(this));
    ui->graphicsArea->scene()->setSceneRect(0, 0, map.width*16, map.height*16);
    ui->graphicsArea->scene()->addItem(new QGraphicsPixmapItem(pix));
    QLinearGradient gradient;
    gradient.setSpread(QGradient::ReflectSpread);
    gradient.setStart(QPointF(0.51, 0.51));
    gradient.setFinalStop(QPointF(1.0, 1.0));
    gradient.setColorAt(0, QColor(252, 252, 252, 200));
    gradient.setColorAt(0.193182, QColor(249, 249, 249, 75));
    gradient.setColorAt(1, QColor(240, 240, 240, 50));
    m_areaRectItem = new QGraphicsPolygonItem();
    m_areaRectItem->setPolygon(QPolygon(QRect(3, 3, pix.width()-6, pix.height()-6), true));
    m_areaRectItem->setPen(QPen(Qt::white));
    m_areaRectItem->setBrush(gradient);
    ui->graphicsArea->scene()->addItem(m_areaRectItem);
    m_currentArea = -1;
    ui->listAreas->setCurrentRow(0);
}

DialogMapProperties::~DialogMapProperties()
{
    delete m_panoramaItem;
    delete m_ceilingItem;
    delete m_lowerWallItem;
    delete m_upperWallItem;
    delete m_floorAItem;
    delete m_floorBItem;
    delete m_floorCItem;
    delete m_obstacleAItem;
    delete m_obstacleBItem;
    delete m_obstacleCItem;

    delete ui;
}

void DialogMapProperties::on_groupPanorama_toggled(bool arg1)
{
    m_panoramaItem->setVisible(arg1);
}

void DialogMapProperties::on_groupUseGenerator_toggled(bool arg1)
{
    m_ceilingItem->setVisible(arg1);
    m_lowerWallItem->setVisible(arg1);
    m_floorAItem->setVisible(arg1);
    m_obstacleAItem->setVisible(arg1);
    if (arg1)
    {
        m_upperWallItem->setVisible(ui->groupUpperWall->isChecked());
        m_floorBItem->setVisible(ui->groupFloorB->isChecked());
        m_floorCItem->setVisible(ui->groupFloorC->isChecked());
        m_obstacleBItem->setVisible(ui->groupObstacleB->isChecked());
        m_obstacleCItem->setVisible(ui->groupObstacleC->isChecked());
    }
    else
    {
        m_upperWallItem->setVisible(false);
        m_floorBItem->setVisible(false);
        m_floorCItem->setVisible(false);
        m_obstacleBItem->setVisible(false);
        m_obstacleCItem->setVisible(false);
    }
}

void DialogMapProperties::on_groupUpperWall_toggled(bool arg1)
{
    m_upperWallItem->setVisible(arg1);
}

void DialogMapProperties::on_groupFloorB_toggled(bool arg1)
{
    m_floorBItem->setVisible(arg1);
}

void DialogMapProperties::on_groupFloorC_toggled(bool arg1)
{
    m_floorCItem->setVisible(arg1);
}

void DialogMapProperties::on_groupObstacleB_toggled(bool arg1)
{
    m_obstacleBItem->setVisible(arg1);
}

void DialogMapProperties::on_groupObstacleC_toggled(bool arg1)
{
    m_obstacleCItem->setVisible(arg1);
}

void DialogMapProperties::on_tableEncounters_itemChanged(QTableWidgetItem *item)
{
    if (item->row() == ui->tableEncounters->rowCount()-1)
    {
        QTableWidgetItem *n_item = new QTableWidgetItem();
        n_item->setData(Qt::DisplayRole, item->data(Qt::DisplayRole));
        n_item->setData(Qt::UserRole, item->data(Qt::UserRole));
        ui->tableEncounters->insertRow(ui->tableEncounters->rowCount()-1);
        ui->tableEncounters->setItem(ui->tableEncounters->rowCount()-2, 0, n_item);
        item->setData(Qt::DisplayRole, "<Add Encounter>");
    }
}

void DialogMapProperties::on_lineListFilter_textChanged(const QString &arg1)
{
    for (int i = 1; i < ui->listAreas->count(); i++)
        ui->listAreas->item(i)->setHidden(ui->listAreas->item(i)->text()
                                          .contains(arg1, Qt::CaseInsensitive));
}

void DialogMapProperties::on_listAreas_currentRowChanged(int currentRow)
{
    if (m_currentArea >= 0 && m_currentArea < (int)m_areas.size())
    {
        //Update changes
        RPG::MapInfo &area = m_areas[m_currentArea];
        area.encounters.clear();
        for (int i = 0; i < ui->tableEncounters->rowCount() - 1; i++)
        {
            RPG::Encounter encounter;
            encounter.ID = i+1;
            encounter.troop_id = ui->tableEncounters->item(i,0)->data(Qt::UserRole).toInt();
            area.encounters.push_back(encounter);
        }
        //TODO: Update area.area_rect;
    }

    while (ui->tableEncounters->rowCount() > 1)
        ui->tableEncounters->removeRow(0);
    RPG::MapInfo info = m_areas[currentRow];
    for (int i = (int)info.encounters.size() - 1; i >= 0; i--)
    {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, QString::fromStdString(Data::troops[info.encounters[i].troop_id-1].name));
        item->setData(Qt::UserRole, info.encounters[i].troop_id);
        ui->tableEncounters->insertRow(0);
        ui->tableEncounters->setItem(0,0,item);
    }
    ui->spinEncontrerRate->setEnabled(currentRow == 0);
    ui->lineEncounterAreaName->setEnabled(currentRow != 0);
    ui->lineEncounterAreaName->setText(ui->listAreas->currentItem()->text());
    if (currentRow == 0)
        m_areaRectItem->setPolygon(QPolygon(QRect(3, 3, m_map.width*16-6, m_map.height*16-6), true));
    else
    {
        QPolygon p;
        p.append(QPoint(info.area_rect.l*16+3, info.area_rect.t*16+3));
        p.append(QPoint(info.area_rect.r*16-3, info.area_rect.t*16+3));
        p.append(QPoint(info.area_rect.r*16-3, info.area_rect.b*16-3));
        p.append(QPoint(info.area_rect.l*16+3, info.area_rect.b*16-3));
        m_areaRectItem->setPolygon(p);
    }
    m_currentArea = currentRow;
}

void DialogMapProperties::on_lineEncounterAreaName_textChanged(const QString &arg1)
{
    if (ui->listAreas->currentRow() == 0)
        return;
    ui->listAreas->currentItem()->setText(arg1);
    m_areas[m_currentArea].name = arg1.toStdString();
}
