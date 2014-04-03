#include "QGraphicsLayerItem.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QScrollBar>

QBrush QGraphicsLayerItem::grid = QBrush();

QGraphicsLayerItem::QGraphicsLayerItem(Core::Layer layer,
                                       std::vector<short> &data,
                                       int width,
                                       int height,
                                       QGraphicsItem *parent) :
QGraphicsItem(parent),
map_data(data)
{
    m_layer = layer;
    m_data = data;
    w = width;
    h = height;
    m_primaryAction = None;
    m_secondaryAction = None;
    m_activeAction = None;

    Init();
}

void QGraphicsLayerItem::Init()
{
    static bool done = false;

    if (done)
        return;

    QPixmap g(mCore->tileSize(), mCore->tileSize());
    QPainter p(&g);
    p.fillRect(0, 0, mCore->tileSize(), mCore->tileSize(), Qt::magenta);
    p.fillRect(0, 0, mCore->tileSize()/2, mCore->tileSize()/2, Qt::darkMagenta);
    p.fillRect(mCore->tileSize()/2, mCore->tileSize()/2,
               mCore->tileSize()/2, mCore->tileSize()/2, Qt::darkMagenta);
    p.end();
    grid = QBrush(g);

    done = true;
}

QRectF QGraphicsLayerItem::boundingRect() const
{
    return QRectF(0.0, 0.0, w*mCore->tileSize()*scale(), h*mCore->tileSize()*scale());
}

void QGraphicsLayerItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if (!painter->isActive())
        return;
    QGraphicsView* view = this->scene()->views()[0];
    if (!view)
        return;

    QRectF rect = view->viewport()->visibleRegion().boundingRect();
//    rect.translate(view->horizontalScrollBar()->value()/scale(), view->verticalScrollBar()->value()/scale());
    int top = rect.top()/mCore->tileSize();
    int bottom = rect.bottom()/mCore->tileSize()+1;
    int left = rect.left()/mCore->tileSize();
    int right = rect.right()/mCore->tileSize()+1;
    for (int y = top; y < bottom; y++)
        for (int x = left; x < right; x++)
      for (int y = 0; y < h; y++)
          for (int x = 0; x < w; x++)
            mCore->renderTile(painter, m_data[_index(x,y)], tileRect(x, y).toRect());
}

void QGraphicsLayerItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(!isActive() || m_cancelled)
        return;
    if (m_activeAction != None)
    {
        cancel();
        return;
    }
    if (event->button() == Qt::LeftButton)
        m_activeAction = m_primaryAction;
    if (event->button() == Qt::RightButton)
        m_activeAction = m_secondaryAction;
    fst_x = cur_x;
    fst_y = cur_y;
    if (updateActiveAction())
        update();
}

void QGraphicsLayerItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!isActive)
        return;
    if (cur_x == event->pos().x()/mCore->tileSize() &&
            cur_y == event->pos().y()/mCore->tileSize())
        return;
    cur_x = event->pos().x()/mCore->tileSize();
    cur_y = event->pos().y()/mCore->tileSize();
    if (updateActiveAction())
        update();
}

void QGraphicsLayerItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!isActive())
        return;
    if (m_cancelled && !event->buttons())
    {
        m_cancelled = false;
//        onToolChanged();
        return;
    }
    if (m_activeAction == Draw && !m_cancelled)
    {
        m_activeAction = None;
        map_data = m_data;
        //TODO: undo stack
//        if (mCore->layer() == Core::LOWER)
//        {
//            m_undoStack->push(new QUndoDraw(Core::LOWER,
//                                            m_map.get()->lower_layer,
//                                            this));
//            m_map.get()->lower_layer = m_lower;
//        }
//        else
//        {
//            m_undoStack->push(new QUndoDraw(Core::UPPER,
//                                            m_map.get()->upper_layer,
//                                            this));
//            m_map.get()->upper_layer = m_upper;
//        }
        //emit mapChanged();
    }
}

void QGraphicsLayerItem::cancel()
{
    m_cancelled = true;
    m_data = map_data;
    update();
}

bool QGraphicsLayerItem::updateActiveAction()
{
    switch (m_activeAction)
    {
    int x1, y1, x2, y2;
    case Draw:
        switch (mCore->tool())
        {
        case (Core::PENCIL):
            for (int x = cur_x; x < cur_x + mCore->selWidth(); x++)
                for (int y = cur_y; y < cur_y + mCore->selHeight(); y++)
                    m_data[_index(x,y)] = mCore->selection(x-fst_x,y-fst_y);
            updateArea(cur_x-1,cur_y-1,cur_x+mCore->selWidth()+1,cur_y+mCore->selHeight()+1);
            break;
        case (Core::RECTANGLE):
            m_data = map_data;
            x1 = fst_x > cur_x ? cur_x : fst_x;
            x2 = fst_x > cur_x ? fst_x : cur_x;
            y1 = fst_y > cur_y ? cur_y : fst_y;
            y2 = fst_y > cur_y ? fst_y : cur_y;
            for (int x = x1; x <= x2; x++)
                for (int y = y1; y <= y2; y++)
                    m_data[_index(x,y)] = mCore->selection(x-fst_x,y-fst_y);
            updateArea(x1-1, y1-1, x2+1, y2+1);
            break;
        case (Core::CIRCLE):
            //TODO: missing code
            break;
        case (Core::FILL):
            drawFill(mCore->translate(m_data[_index(fst_x,fst_y)]),fst_x,fst_y);
            updateArea(0, 0, w-1 ,h-1);
            break;
        }
        break;
    case Select:
        //TODO: missing code
        break;
    default:
        return false;
    }
    return true;
}

void QGraphicsLayerItem::updateArea(int x1, int y1, int x2, int y2)
{
    if (m_layer != Core::LOWER)
        return;
    //Normalize
    if (x1 < 0)
        x1 = 0;
    if (y1 < 0)
        y1 = 0;
    if (x2 >= w)
        x2 = w - 1;
    if (y2 >= h)
        y2 = h - 1;

    for (int x = x1; x <= x2; x++)
        for (int y = y1; y <= y2; y++)
        if (!mCore->isEblock(mCore->translate(m_data[_index(x, y)])) &&
            !mCore->isAnimation(mCore->translate(m_data[_index(x, y)])))
            m_data[_index(x,y)] = bind(x, y);
}

short QGraphicsLayerItem::bind(int x, int y)
{
#define tile_u mCore->translate(m_data[_index(x, y-1)])
#define tile_d mCore->translate(m_data[_index(x, y+1)])
#define tile_l mCore->translate(m_data[_index(x-1, y)])
#define tile_r mCore->translate(m_data[_index(x+1, y)])
#define tile_ul mCore->translate(m_data[_index(x-1, y-1)])
#define tile_ur mCore->translate(m_data[_index(x+1, y-1)])
#define tile_dl mCore->translate(m_data[_index(x-1, y+1)])
#define tile_dr mCore->translate(m_data[_index(x+1, y+1)])
#define exist_u y > 0
#define exist_d y < h-1
#define exist_l x > 0
#define exist_r x < w-1
#define exist_ul x > 0 && y > 0
#define exist_ur x < w-1 && y > 0
#define exist_dl x > 0 && y < h-1
#define exist_dr x < w-1 && y < h-1
    int _code = 0, _scode = 0;
    int terrain_id = mCore->translate(m_data[_index(x, y)]);
    int u=0,d=0,l=0,r=0,ul=0,ur=0,dl=0,dr=0,sul=0,sur=0,sdl=0,sdr=0;
    if (mCore->isDblock(terrain_id))
    {
        if (exist_u && terrain_id != tile_u)
            u = UP;
        if (exist_u && terrain_id != tile_d)
            d = DOWN;
        if (exist_l && terrain_id != tile_l)
            l = LEFT;
        if (exist_r && terrain_id != tile_r)
            r = RIGHT;
        if (u+l == 0 && exist_ul && terrain_id != tile_ul)
            ul = UPLEFT;
        if (u+r == 0 && exist_ur && terrain_id != tile_ur)
            ur = UPRIGHT;
        if (d+l == 0 && exist_dl && terrain_id != tile_dl)
            dl = DOWNLEFT;
        if (d+r == 0 && exist_dr && terrain_id != tile_dr)
            dr = DOWNRIGHT;
        _code = u+d+l+r+ul+ur+dl+dr;
    }
    else if (mCore->isWater(terrain_id))
    {
        if (exist_u && (!mCore->isWater(tile_u) && !mCore->isAnimation(tile_u)))
            u = UP;
        if (exist_u && (!mCore->isWater(tile_d) && !mCore->isAnimation(tile_d)))
            d = DOWN;
        if (exist_l && (!mCore->isWater(tile_l) && !mCore->isAnimation(tile_l)))
            l = LEFT;
        if (exist_r && (!mCore->isWater(tile_r) && !mCore->isAnimation(tile_r)))
            r = RIGHT;
        if ((u+l) == 0 && exist_ul && !mCore->isWater(tile_ul))
            ul = UPLEFT;
        if ((u+r) == 0 && exist_ur && !mCore->isWater(tile_ur))
            ur = UPRIGHT;
        if ((d+l) == 0 && exist_dl && !mCore->isWater(tile_dl))
            dl = DOWNLEFT;
        if ((d+r) == 0 && exist_r && exist_u && !mCore->isWater(tile_dr))
            dr = DOWNRIGHT;
        _code = u+d+l+r+ul+ur+dl+dr;
        // DeepWater Special Corners
        if (mCore->isDWater(terrain_id))
        {
            if (exist_ul && mCore->isABWater(tile_u) && mCore->isABWater (tile_l) && mCore->isABWater (tile_ul))
                sul = UPLEFT;
            if (exist_ur && mCore->isABWater(tile_u) && mCore->isABWater (tile_r) && mCore->isABWater (tile_ur))
                sur = UPRIGHT;
            if (exist_dl && mCore->isABWater(tile_d) && mCore->isABWater (tile_l) && mCore->isABWater (tile_dl))
                sdl = DOWNLEFT;
            if (exist_dr && mCore->isABWater(tile_d) && mCore->isABWater (tile_r) && mCore->isABWater (tile_dr))
                sdr = DOWNRIGHT;
        }
        else
        {
            if (exist_ul && mCore->isDWater (tile_u) && mCore->isDWater (tile_l) && mCore->isWater(tile_ul))
                sul = UPLEFT;
            if (exist_ur && mCore->isDWater (tile_u) && mCore->isDWater (tile_r) && mCore->isWater(tile_ur))
                sur = UPRIGHT;
            if (exist_dl && mCore->isDWater (tile_d) && mCore->isDWater (tile_l) && mCore->isWater(tile_dl))
                sdl = DOWNLEFT;
            if (exist_dr && mCore->isDWater (tile_d) && mCore->isDWater (tile_r) && mCore->isWater(tile_dr))
                sdr = DOWNRIGHT;
        }
        _scode = sul+sur+sdl+sdr;
    }
    return mCore->translate(terrain_id, _code, _scode);
#undef tile_u
#undef tile_d
#undef tile_l
#undef tile_r
#undef tile_ul
#undef tile_ur
#undef tile_dl
#undef tile_dr
#undef exist_u
#undef exist_d
#undef exist_l
#undef exist_r
#undef exist_ul
#undef exist_ur
#undef exist_dl
#undef exist_dr
}

void QGraphicsLayerItem::drawFill(int terrain_id, int x, int y)
{
    if (x < 0 || x >= w || y < 0 || y >= h)
        return;
    if (terrain_id == mCore->selection(x-fst_x,y-fst_y))
        return;
    if (mCore->translate(m_data[_index(x,y)]) != terrain_id)
        return;
    m_data[_index(x,y)] = mCore->selection(x-fst_x,y-fst_y);
    drawFill(terrain_id, x, y-1);
    drawFill(terrain_id, x-1, y);
    drawFill(terrain_id, x+1, y);
    drawFill(terrain_id, x, y+1);
}

QGraphicsLayerItem::LayerAction QGraphicsLayerItem::secondaryAction() const
{
    return m_secondaryAction;
}

void QGraphicsLayerItem::setSecondaryAction(const LayerAction &secondaryAction)
{
    m_secondaryAction = secondaryAction;
}

QGraphicsLayerItem::LayerAction QGraphicsLayerItem::primaryAction() const
{
    return m_primaryAction;
}

void QGraphicsLayerItem::setPrimaryAction(const LayerAction &primaryAction)
{
    m_primaryAction = primaryAction;
}

