#include "qgraphicslayeritem.h"
#include <QGraphicsView>
#include <QScrollBar>
#include "../core.h"

QGraphicsLayerItem::QGraphicsLayerItem(std::vector<short> &data, int &_w, int &_h, QGraphicsItem *parent) :
QGraphicsItem(parent),
m_data(data),
w(_w),
h(_h)
{
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, true);
}

QRectF QGraphicsLayerItem::boundingRect() const
{
    return QRectF(0,0,w*mCore->tileSize(), h*mCore->tileSize());
}

void QGraphicsLayerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    QGraphicsView* view = this->scene()->views()[0];
    if (!view)
        return;

    QSize size = view->size() / view->transform().m11();
    if (size.width() > w*mCore->tileSize()*view->transform().m11())
        size.setWidth(w*mCore->tileSize());
    if (size.height() > h*mCore->tileSize()*view->transform().m11())
        size.setHeight(h*mCore->tileSize());

    QRect rect(view->horizontalScrollBar()->value() / view->transform().m11(),
               view->verticalScrollBar()->value() / view->transform().m11(),
               size.width(), size.height());
    int top = rect.top()/mCore->tileSize();
    int bottom = rect.bottom()/mCore->tileSize()+1;
    int left = rect.left()/mCore->tileSize();
    int right = rect.right()/mCore->tileSize()+1;
    for (int y = top; y < bottom; y++)
        for (int x = left; x < right; x++)
            mCore->renderTile(painter, m_data[_index(x,y)], tileRect(x, y));
}

QRect QGraphicsLayerItem::tileRect(int x, int y)
{
    return QRect(x*mCore->tileSize(),
                 y*mCore->tileSize(),
                 mCore->tileSize(),
                 mCore->tileSize());
}

