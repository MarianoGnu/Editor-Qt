#ifndef QGRAPHICSLAYERITEM_H
#define QGRAPHICSLAYERITEM_H

#include <QGraphicsItem>

class QGraphicsLayerItem : public QGraphicsItem
{
public:
    explicit QGraphicsLayerItem(std::vector<short> &data, int &_w, int &_h, QGraphicsItem *parent = 0);

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:

public slots:

private:
    inline int _index(int x, int y) {return y*w+x;}
    inline static QRect tileRect(int x, int y);

    std::vector<short> &m_data;
    int &w;
    int &h;
};

#endif // QGRAPHICSLAYERITEM_H
