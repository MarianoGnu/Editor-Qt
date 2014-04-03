#ifndef QGRAPHICSLAYERITEM_H
#define QGRAPHICSLAYERITEM_H

#include <QGraphicsItem>

#include <QPainter>
#include "../core.h"

class QGraphicsLayerItem : public QGraphicsItem
{
public:
    enum LayerAction
    {
        None,
        Draw,
        Select
    };

    explicit QGraphicsLayerItem(Core::Layer layer,
                                std::vector<short> &data,
                                int width,
                                int height,
                                QGraphicsItem *parent = 0);

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    LayerAction primaryAction() const;
    void setPrimaryAction(const LayerAction &primaryAction);

    LayerAction secondaryAction() const;
    void setSecondaryAction(const LayerAction &secondaryAction);

    void resize(int width, int height, short tile_id);

signals:

public slots:

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void cancel();

    bool updateActiveAction();

    void updateArea(int x1, int y1, int x2, int y2);

    short bind(int x, int y);

    void drawFill (int terrain_id, int x, int y);

    inline int _x(int index) {return (index%w);}

    inline int _y(int index) {return (index/w);}

    inline int _index(int x, int y) {return (y*w+x);}

    inline QRectF tileRect(int x, int y) {return QRectF(x*mCore->tileSize(),
                                                        y*mCore->tileSize(),
                                                        mCore->tileSize(),
                                                        mCore->tileSize());}

    inline QRectF tileRect(int index) {return tileRect(_x(index), _y(index));}

private:
    std::vector<short> &map_data;
    std::vector<short> m_data;
    int w;
    int h;
    Core::Layer m_layer;
    LayerAction m_primaryAction;
    LayerAction m_secondaryAction;
    LayerAction m_activeAction;
    bool m_cancelled;
    int cur_x;
    int cur_y;
    int fst_x;
    int fst_y;
    int lst_x;
    int lst_y;

    QPixmap pix;
    QRectF lst_exposed;
    static QBrush grid;
    static void Init();
};

#endif // QGRAPHICSLAYERITEM_H
