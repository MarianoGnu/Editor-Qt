#ifndef QGRAPHICSMAPSCENE_H
#define QGRAPHICSMAPSCENE_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QMap>
#include <QMenu>
#include <QUndoStack>
#include <memory>
#include <rpg_map.h>
#include <rpg_mapinfo.h>
#include "QGraphicsLayerItem.h"
#include "../core.h"

class QGraphicsMapScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit QGraphicsMapScene(int id,QGraphicsView *view);
    ~QGraphicsMapScene();

    void Init();
    float scale() const;
    void setScale(float scale);
    QString mapName() const;
    bool isModified() const;
    int id() const;
    int chipsetId() const;
    void setLayerData(Core::Layer layer, std::vector<short> data);
    void setEventData(int id, const RPG::Event &data);
    QMap<int, RPG::Event *> *mapEvents();

signals:
    void actionRunHereTriggered(int map_id, int x, int y);

    void mapChanged();

    void mapSaved();

    void mapReverted();

public slots:
    void redrawMap();

    void onLayerChanged();

    void onToolChanged();

    void Save();

    void Load();

    void undo();

private slots:
    void on_actionRunHere();

    void on_actionSetStartPosition();

    void on_actionNewEvent();

    void on_user_interaction();

    void on_view_V_Scroll();

    void on_view_H_Scroll();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    inline int _x(int index) {return (index%m_map->width);}
    inline int _y(int index) {return (index/m_map->width);}
    inline int _index(int x, int y) {return (y*m_map->width+x);}
    void redrawTile(const Core::Layer &layer,
                    const int &x,
                    const int &y,
                    const QRect &dest_rec);
    short bind(int x, int y);


    QMenu *m_eventMenu;
    QGraphicsLayerItem *m_lowerpix;
    QGraphicsLayerItem *m_upperpix;
    QVector<QGraphicsPixmapItem*> m_eventpixs;
    QVector<QGraphicsLineItem*> m_lines; //TODO: Move to Layer Item
    QUndoStack *m_undoStack;
    std::auto_ptr<RPG::Map> m_map;
    RPG::MapInfo *m_mapInfo;
    RPG::MapInfo *n_mapInfo; //To store unsaved changes
    bool m_init;
    int s_tileSize;
    int lst_x;
    int lst_y;
    QGraphicsView* m_view;
    float m_scale;
    bool m_userInteraction;
};

#endif // QGRAPHICSMAPSCENE_H
